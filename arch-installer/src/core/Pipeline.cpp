#include "core/Pipeline.hpp"
#include <fstream>
#include <iostream>
#include <stdexcept>

static std::string esc(const std::string& s) {
  std::string out="\'";
  for (char c: s) out += (c=='\'') ? "'\"'\"'" : std::string(1,c);
  out += '\'';
  return out;
}

Pipeline::Pipeline(Context& ctx, IRunner& runner) : ctx_(ctx), runner_(runner) {}

void Pipeline::addSteps(std::vector<Step> steps) {
  for (auto& s : steps) steps_.push_back(std::move(s));
}

bool Pipeline::execute() {
  try {
    for (const auto& s : steps_) {
      if (!execStep(s)) throw std::runtime_error("step failed");
      if (!s.guardKey.empty()) {
        guardStack_.push(s.guardKey);
      }
    }
    rollback();
    return true;
  } catch (const std::exception& e) {
    std::cerr << "[ERROR] " << e.what() << "\n";
    rollback();
    return false;
  }
}

bool Pipeline::execStep(const Step& s) {
  switch (s.type) {
    case StepType::Run:
      runner_.run(s.cmd);
      return true;
    case StepType::Chroot:
      runner_.run("arch-chroot " + ctx_.mountRoot + " /bin/bash -c " + esc(s.cmd));
      return true;
    case StepType::Capture: {
      auto val = runner_.capture(s.cmd);
      if (!s.key.empty()) ctx_.vars[s.key] = val;
      return true;
    }
    case StepType::File: {
      std::ofstream f(s.path, std::ios::trunc);
      if (!f) throw std::runtime_error("cannot write file: " + s.path);
      f << s.content;
      runner_.run("chmod " + s.mode + " " + s.path);
      return true;
    }
    case StepType::Service: {
      if (s.action == "enable")
        runner_.run("arch-chroot " + ctx_.mountRoot + " systemctl enable " + s.name);
      else if (s.action == "start")
        runner_.run("arch-chroot " + ctx_.mountRoot + " systemctl start " + s.name);
      else if (s.action == "disable")
        runner_.run("arch-chroot " + ctx_.mountRoot + " systemctl disable " + s.name);
      else
        throw std::runtime_error("unknown service action: " + s.action);
      return true;
    }
    case StepType::UserSet: {
      runner_.run("arch-chroot " + ctx_.mountRoot + " id -u " + s.username + " || useradd -m -g users -G wheel -s " + s.shell + " " + s.username);
      runner_.run("arch-chroot " + ctx_.mountRoot + " bash -lc " + esc("echo " + s.username + ":" + s.password + " | chpasswd"));
      return true;
    }
  }
  return false;
}

void Pipeline::rollback() {
  while (!guardStack_.empty()) {
    auto g = guardStack_.top();
    guardStack_.pop();
    try {
      if (g == "mnt_boot") {
        runner_.run("umount " + ctx_.mountRoot + "/boot");
      } else if (g == "mnt_root") {
        runner_.run("umount " + ctx_.mountRoot);
      } else if (g == "swap_on") {
        runner_.run("swapoff " + ctx_.devSwap);
      }
    } catch (...) {
      std::cerr << "[WARN] rollback failed for guard: " << g << "\n";
    }
  }
}
