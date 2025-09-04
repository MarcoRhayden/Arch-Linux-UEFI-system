#pragma once
#include "core/Context.hpp"

namespace Arch {
class Installer {
public:
  explicit Installer(Context ctx);
  bool run();
private:
  Context ctx_;
};
} // namespace Arch
