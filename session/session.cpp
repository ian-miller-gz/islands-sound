// SPDX-License-Identifier: AGPL-3.0-or-later
#include <algorithm>

#include "session.doors.hpp"

namespace {

SOUND::Session standing;

constexpr Whole KEPT = 8;

}  // namespace

auto SOUND::SESSION::held() -> const Session & { return standing; }

void SOUND::SESSION::adopt(const Session &document) { standing = document; }

void SOUND::SESSION::seated(const String &plugin) {
  if (plugin.empty()) return;
  Vector<String> &kept = standing.recents;
  kept.erase(std::remove(kept.begin(), kept.end(), plugin), kept.end());
  kept.insert(kept.begin(), plugin);
  if (kept.size() > ::KEPT) kept.resize(::KEPT);
}
