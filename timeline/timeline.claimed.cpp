// SPDX-License-Identifier: AGPL-3.0-or-later
#include <algorithm>

#include "timeline.internal.hpp"

auto SOUND::TIMELINE::claimed(const Vector<String> &claims, const String &node)
  -> Flag {
  if (claims.empty()) return true;
  return std::find(claims.begin(), claims.end(), node) != claims.end();
}
