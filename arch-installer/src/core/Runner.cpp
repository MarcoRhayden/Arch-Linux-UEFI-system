#include "core/Runner.hpp"
#include <array>
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <iostream>
#include <stdexcept>
#include <sys/wait.h>

static void checkStatus(int status, const std::string& cmd) {
  if (status == -1) throw std::runtime_error("system() failed for: " + cmd);
  if (WIFEXITED(status)) {
    int code = WEXITSTATUS(status);
    if (code != 0) throw std::runtime_error("command exit " + std::to_string(code) + ": " + cmd);
    return;
  }
  if (WIFSIGNALED(status)) {
    int sig = WTERMSIG(status);
    throw std::runtime_error("command killed by signal " + std::to_string(sig) + ": " + cmd);
  }
  throw std::runtime_error("unknown command termination: " + cmd);
}

int ShellRunner::run(const std::string& cmd) {
  std::cout << "[RUN] " << cmd << "\n";
  int st = std::system(cmd.c_str());
  checkStatus(st, cmd);
  return 0;
}

std::string ShellRunner::capture(const std::string& cmd) {
  std::cout << "[CAPTURE] " << cmd << "\n";
  std::array<char, 256> buf{};
  std::string out;
  FILE* pipe = popen(cmd.c_str(), "r");
  if (!pipe) throw std::runtime_error("popen failed: " + cmd);
  while (fgets(buf.data(), (int)buf.size(), pipe)) out += buf.data();
  pclose(pipe);
  while (!out.empty() && (out.back()=='\n'||out.back()=='\r')) out.pop_back();
  return out;
}

int DryRunRunner::run(const std::string& cmd) {
  std::cout << "[dry-run] " << cmd << "\n";
  return 0;
}

std::string DryRunRunner::capture(const std::string& cmd) {
  std::cout << "[dry-run capture] " << cmd << "\n";
  return "dryrun";
}
