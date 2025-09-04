#include "core/NetCheck.hpp"
#include <netdb.h>
#include <cstring>

namespace netcheck {
bool hasConnectivity() {
  addrinfo hints{};
  hints.ai_family = AF_UNSPEC;
  hints.ai_socktype = SOCK_STREAM;
  addrinfo* res = nullptr;
  int rc = getaddrinfo("www.google.com", "80", &hints, &res);
  if (res) freeaddrinfo(res);
  return rc == 0;
}
}
