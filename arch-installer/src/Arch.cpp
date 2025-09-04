#include "Arch.hpp"
#include <iostream>
#include "core/NetCheck.hpp"
#include "core/PartitionCheck.hpp"
#include "core/Pipeline.hpp"
#include "core/Runner.hpp"
#include "core/Manifest.hpp"

namespace Arch {

Installer::Installer(Context ctx) : ctx_(std::move(ctx)) {
  ctx_.vars["dev_boot"] = ctx_.devBoot;
  ctx_.vars["dev_swap"] = ctx_.devSwap;
  ctx_.vars["dev_root"] = ctx_.devRoot;
}

bool Installer::run() {
  if (!netcheck::hasConnectivity()) {
    std::cerr << "[ERROR] No internet connection.\n";
    return false;
  }
  if (!partcheck::sanity(ctx_)) {
    std::cerr << "[ERROR] Wrong partitions table or devices not readable.\n";
    return false;
  }

  ShellRunner shell;
  DryRunRunner dry;
  IRunner& runner = ctx_.dryRun ? static_cast<IRunner&>(dry)
                                : static_cast<IRunner&>(shell);

  Pipeline pipe(ctx_, runner);

  pipe.addSteps(Manifest::load("manifests/base.yml", ctx_));
  pipe.addSteps(Manifest::load("manifests/bootloader.yml", ctx_));

  if (ctx_.desktop == "xfce") {
    pipe.addSteps(Manifest::load("manifests/desktops/xfce.yml", ctx_));
  } else if (ctx_.desktop == "i3") {
    pipe.addSteps(Manifest::load("manifests/desktops/i3.yml", ctx_));
  }

  bool ok = pipe.execute();
  if (ok) std::cout << "\n[OK] Arch Linux installation pipeline finished.\n";
  return ok;
}

} // namespace Arch
