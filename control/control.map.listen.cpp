// SPDX-License-Identifier: AGPL-3.0-or-later
#include <chrono>

#include "control.map.hpp"

namespace {
using namespace SOUND;

constexpr std::chrono::seconds QUIET{20};

struct Listen {
  Address address;
  std::chrono::steady_clock::time_point since;
};
Listen listen;

}  // namespace

void SOUND::CONTROL::MAP::arm(const Address &address) {
  ::listen = {.address = address, .since = std::chrono::steady_clock::now()};
}

void SOUND::CONTROL::MAP::cancel() { ::listen = {}; }

auto SOUND::CONTROL::MAP::listening() -> Address { return ::listen.address; }

auto SOUND::CONTROL::MAP::stale() -> Flag {
  if (::listen.address.node.empty()) return false;
  return std::chrono::steady_clock::now() - ::listen.since >= ::QUIET;
}
