#pragma once
#include <string>
#include <vector>
#include "core/Context.hpp"
#include "core/Command.hpp"

namespace Manifest {
  std::string expand(const std::string& in, const Context& ctx);
  std::vector<Step> load(const std::string& path, const Context& ctx);
}
