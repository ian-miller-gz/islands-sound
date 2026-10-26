// SPDX-License-Identifier: AGPL-3.0-or-later
#include <island/gui/seat.hpp>

#include "piano.paper.hpp"

namespace {
using namespace SOUND;
namespace PAPER = SOUND::VIEWS::ROLL::PAPER;

constexpr STRING::Hot BARE = "clear";
constexpr Float UNSEEN = 0.0f;

PAPER::Bed kept;

auto span(Whole at) -> String {
  return GUI::SAC::SEAT::named(::kept.id.c_str(), at);
}

}  // namespace

void SOUND::VIEWS::ROLL::PAPER::range(const Bed &bed) {
  const GUI::Handle page = document();
  const Flag laid = papered();
  ::kept = bed;
  for (Whole at = 0; at < bed.count; ++at) {
    const String id = ::span(at);
    GUI::set(page, id.c_str(), GUI::Style{laid ? ::BARE : bed.wash});
  }
}

auto SOUND::VIEWS::ROLL::PAPER::washed() -> Whole {
  for (Whole at = 0; at < ::kept.count; ++at)
    if (GUI::GET::color(document(), ::span(at).c_str()).a > ::UNSEEN) return 1;
  return 0;
}
