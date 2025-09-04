#pragma once
#include "Context.hpp"
#include "Command.hpp"
#include "Runner.hpp"
#include <stack>
#include <unordered_map>
#include <vector>

class Pipeline {
public:
  Pipeline(Context& ctx, IRunner& runner);

  void addSteps(std::vector<Step> steps);
  bool execute();

private:
  bool execStep(const Step& s);
  void rollback();

  Context& ctx_;
  IRunner& runner_;
  std::vector<Step> steps_;
  std::stack<std::string> guardStack_;
  std::unordered_map<std::string, std::string> captured_;
};
