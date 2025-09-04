#pragma once
#include <string>

struct IRunner {
  virtual ~IRunner() = default;
  virtual int run(const std::string& cmd) = 0;
  virtual std::string capture(const std::string& cmd) = 0;
};

struct ShellRunner : IRunner {
  int run(const std::string& cmd) override;
  std::string capture(const std::string& cmd) override;
};

struct DryRunRunner : IRunner {
  int run(const std::string& cmd) override;
  std::string capture(const std::string& cmd) override;
};
