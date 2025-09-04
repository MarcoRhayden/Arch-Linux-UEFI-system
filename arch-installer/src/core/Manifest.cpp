#include "core/Manifest.hpp"
#include "core/Context.hpp"
#include "core/Command.hpp"
#include <yaml-cpp/yaml.h>
#include <regex>
#include <stdexcept>
#include <sstream>
#include <vector>
#include <string>

namespace {
  std::string getEnvOrEmpty(const std::string& k) {
    const char* v = std::getenv(k.c_str());
    return v ? std::string(v) : std::string();
  }

  std::string readStrExpand(const YAML::Node& n, const char* key, const Context& ctx, bool optional=false) {
    if (!n[key]) {
      if (optional) return {};
      throw std::runtime_error(std::string("manifest missing required key: ") + key);
    }
    auto s = n[key].as<std::string>();
    return Manifest::expand(s, ctx);
  }

  std::string readStrRaw(const YAML::Node& n, const char* key, bool optional=false) {
    if (!n[key]) {
      if (optional) return {};
      throw std::runtime_error(std::string("manifest missing required key: ") + key);
    }
    return n[key].as<std::string>();
  }

  std::vector<std::string> readStrListExpand(const YAML::Node& n, const char* key, const Context& ctx, bool optional=false) {
    std::vector<std::string> out;
    if (!n[key]) {
      if (optional) return out;
      throw std::runtime_error(std::string("manifest missing required key: ") + key);
    }
    for (auto&& it : n[key]) {
      out.push_back(Manifest::expand(it.as<std::string>(), ctx));
    }
    return out;
  }

  StepType parseType(const std::string& s) {
    if (s == "run")     return StepType::Run;
    if (s == "chroot")  return StepType::Chroot;
    if (s == "capture") return StepType::Capture;
    if (s == "file")    return StepType::File;
    if (s == "service") return StepType::Service;
    if (s == "userset") return StepType::UserSet;
    throw std::runtime_error("unknown step type: " + s);
  }

} // namespace

namespace Manifest {

std::string expand(const std::string& in, const Context& ctx) {
  static const std::regex re(R"(\$\{([A-Za-z_][A-Za-z0-9_]*)\})");
  std::string out;
  out.reserve(in.size());

  std::sregex_iterator it(in.begin(), in.end(), re);
  std::sregex_iterator end;
  size_t last = 0;

  for (; it != end; ++it) {
    auto m = *it;
    out.append(in, last, static_cast<size_t>(m.position()) - last);
    std::string key = m.str(1);

    auto kv = ctx.vars.find(key);
    if (kv != ctx.vars.end()) {
      out += kv->second;
    } else {
      out += getEnvOrEmpty(key);
    }
    last = static_cast<size_t>(m.position() + m.length());
  }
  out.append(in, last, std::string::npos);
  return out;
}

std::vector<Step> load(const std::string& path, const Context& ctx) {
  YAML::Node root = YAML::LoadFile(path);
  if (!root || !root["steps"] || !root["steps"].IsSequence()) {
    throw std::runtime_error("manifest invalid or missing 'steps': " + path);
  }

  std::vector<Step> steps;
  steps.reserve(root["steps"].size());

  for (auto&& node : root["steps"]) {
    if (!node["type"]) throw std::runtime_error("step missing 'type' in " + path);
    auto typeStr = node["type"].as<std::string>();
    auto t = parseType(typeStr);

    Step s;
    s.type = t;

    switch (t) {
      case StepType::Run: {
        s.cmd = readStrExpand(node, "cmd", ctx);
        s.guardKey = readStrRaw(node, "guard_key", /*optional=*/true);
        break;
      }
      case StepType::Chroot: {
        s.cmd = readStrExpand(node, "cmd", ctx);
        break;
      }
      case StepType::Capture: {
        s.cmd = readStrExpand(node, "cmd", ctx);
        s.key = readStrRaw(node, "key", /*optional=*/true);
        if (s.key.empty() && node["key"])
          s.key = readStrExpand(node, "key", ctx);
        break;
      }
      case StepType::File: {
        s.path = readStrExpand(node, "path", ctx);
        s.mode = readStrRaw(node, "mode", /*optional=*/true);
        if (node["template"]) {
          s.content = readStrExpand(node, "template", ctx);
        } else {
          s.content = readStrExpand(node, "content", ctx);
        }
        if (s.mode.empty()) s.mode = "0644";
        break;
      }
      case StepType::Service: {
        s.action = readStrRaw(node, "action");
        s.name   = readStrExpand(node, "name", ctx);
        break;
      }
      case StepType::UserSet: {
        s.username = readStrExpand(node, "username", ctx);
        s.password = readStrExpand(node, "password", ctx);
        s.shell    = node["shell"] ? readStrExpand(node, "shell", ctx) : "/bin/bash";
        s.groups   = node["groups"] ? readStrListExpand(node, "groups", ctx, /*optional=*/true)
                                    : std::vector<std::string>{};
        break;
      }
    }

    steps.push_back(std::move(s));
  }

  return steps;
}

} // namespace Manifest
