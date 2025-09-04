#include "Arch.hpp"
#include "core/Context.hpp"
#include <iostream>

int main(int argc, char** argv) {
  Context ctx;

  for (int i=1;i<argc;++i) {
    std::string a(argv[i]);
    if (a.rfind("--desktop=",0)==0) ctx.desktop = a.substr(10);
    else if (a.rfind("--dry-run=",0)==0) ctx.dryRun = (a.substr(10)=="1");
    else if (a.rfind("--dev-root=",0)==0) ctx.devRoot = a.substr(11);
    else if (a.rfind("--dev-swap=",0)==0) ctx.devSwap = a.substr(11);
    else if (a.rfind("--dev-boot=",0)==0) ctx.devBoot = a.substr(11);
  }

  std::cout << "[INFO] desktop=" << ctx.desktop
            << " dryRun=" << (ctx.dryRun?"true":"false") << "\n";

  Arch::Installer installer(ctx);
  bool ok = installer.run();
  return ok ? 0 : 1;
}
