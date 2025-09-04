#include "core/PartitionCheck.hpp"
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>

static bool isBlk(const char* p){
  struct stat st{};
  if (stat(p, &st) != 0) return false;
  return S_ISBLK(st.st_mode);
}
static bool readable(const char* p){
  int fd = open(p, O_RDONLY | O_CLOEXEC);
  if (fd < 0) return false;
  close(fd);
  return true;
}

namespace partcheck {
bool sanity(const Context& ctx) {
  if (!isBlk(ctx.devBoot.c_str()) || !readable(ctx.devBoot.c_str())) return false;
  if (!isBlk(ctx.devSwap.c_str()) || !readable(ctx.devSwap.c_str())) return false;
  if (!isBlk(ctx.devRoot.c_str()) || !readable(ctx.devRoot.c_str())) return false;
  return true;
}
}
