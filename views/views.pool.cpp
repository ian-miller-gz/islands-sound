// SPDX-License-Identifier: AGPL-3.0-or-later
#include "views.hpp"

namespace {
using namespace SOUND;

constexpr Whole SPARE = 2;

}  // namespace

auto SOUND::VIEWS::sized(Pool &pool, Whole wanted, Float seen) -> Whole {
  if (seen != pool.seen) {
    pool.seen = seen;
    return wanted;
  }
  if (wanted > pool.stood || wanted * ::SPARE < pool.stood) return wanted;
  return pool.stood;
}

void SOUND::VIEWS::built(Pool &pool, Whole count) {
  pool.stood = count;
  if (pool.lit > count) pool.lit = count;
}

void SOUND::VIEWS::light(Pool &pool, Named named, Whole count) {
  const GUI::Handle page = document();
  for (Whole at = count; at < pool.stood; ++at)
    GUI::set(page, named(at).c_str(), GUI::Visibility{false});
  for (Whole at = pool.lit; at < count; ++at)
    GUI::set(page, named(at).c_str(), GUI::Visibility{true});
  pool.lit = count;
}

auto SOUND::VIEWS::reborn(Pool &pool) -> Flag {
  if (!reborn(pool.born)) return false;
  pool.stood = 0;
  pool.lit = 0;
  pool.seen = 0.0f;
  return true;
}
