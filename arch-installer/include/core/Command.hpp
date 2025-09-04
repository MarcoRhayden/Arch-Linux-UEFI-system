#pragma once
#include <string>
#include <vector>

enum class StepType { Run, Chroot, Capture, File, Service, UserSet };

struct Step {
  StepType type{};
  std::string cmd;                 // Run/Chroot/Capture
  std::string path;                // File
  std::string content;             // File
  std::string mode = "0644";       // File
  std::string action;              // Service: enable/start/disable/stop
  std::string name;                // Service name
  std::string username;            // UserSet
  std::vector<std::string> groups; // UserSet
  std::string shell = "/bin/bash"; // UserSet
  std::string password;            // UserSet
  std::string key;                 // Capture key
  std::string guardKey;            // Guard p/ rollback
};

inline Step Run(std::string cmd, std::string guardKey = {}) {
  Step s; s.type = StepType::Run; s.cmd = std::move(cmd); s.guardKey = std::move(guardKey); return s;
}
inline Step Chroot(std::string cmd) {
  Step s; s.type = StepType::Chroot; s.cmd = std::move(cmd); return s;
}
inline Step Capture(std::string cmd, std::string key) {
  Step s; s.type = StepType::Capture; s.cmd = std::move(cmd); s.key = std::move(key); return s;
}
inline Step File(std::string path, std::string content, std::string mode = "0644") {
  Step s; s.type = StepType::File; s.path = std::move(path); s.content = std::move(content); s.mode = std::move(mode); return s;
}
inline Step ServiceEnable(std::string name) {
  Step s; s.type = StepType::Service; s.action = "enable"; s.name = std::move(name); return s;
}
inline Step ServiceStart(std::string name) {
  Step s; s.type = StepType::Service; s.action = "start"; s.name = std::move(name); return s;
}
inline Step UserAddOrUpdate(std::string user, std::string password,
                            std::vector<std::string> groups = {"users","wheel"},
                            std::string shell = "/bin/bash") {
  Step s; s.type = StepType::UserSet;
  s.username = std::move(user); s.password = std::move(password);
  s.groups = std::move(groups); s.shell = std::move(shell);
  return s;
}
