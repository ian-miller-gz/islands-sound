// SPDX-License-Identifier: AGPL-3.0-or-later
#include <algorithm>

#include "piano.acts.hpp"

namespace {
using namespace SOUND::VIEWS::ROLL;

constexpr STRING::Hot INVERTED = "invert";
constexpr STRING::Hot REVERSED = "reverse";

struct Bounds {
  Whole low = 0, high = 0;
};

auto pitched(const Vector<Pip> &pips) -> ::Bounds {
  const auto [low, high] = std::ranges::minmax(pips, {}, &Pip::pitch);
  return {low.pitch, high.pitch};
}

auto timed(const Vector<Pip> &pips) -> ::Bounds {
  ::Bounds span = {pips.front().at, 0};
  for (const Pip &pip : pips) {
    span.low = std::min(span.low, pip.at);
    span.high = std::max(span.high, pip.at + pip.length);
  }
  return span;
}

auto moving(const Vector<Landing> &landings) -> Flag {
  return std::ranges::any_of(landings, [](const Landing &one) {
    return one.at != one.pip.at || one.pitch != one.pip.pitch;
  });
}

void wrote(const Vector<Landing> &landings, STRING::Hot act) {
  if (::moving(landings)) landed(landings, act);
}

}  // namespace

void SOUND::VIEWS::ROLL::inverted(GUI::Handle page) {
  const Vector<Pip> pips = seated(page);
  if (pips.empty()) return;
  const ::Bounds range = ::pitched(pips);
  Vector<Landing> landings;
  for (const Pip &pip : pips)
    landings.push_back({pip, pip.at, range.low + range.high - pip.pitch});
  ::wrote(landings, ::INVERTED);
}

void SOUND::VIEWS::ROLL::reversed(GUI::Handle page) {
  const Vector<Pip> pips = seated(page);
  if (pips.empty()) return;
  const ::Bounds span = ::timed(pips);
  Vector<Landing> landings;
  for (const Pip &pip : pips) {
    const Integer by = Integer(span.low + span.high - pip.length) -
                       Integer(pip.at) - Integer(pip.at);
    const Whole at = clipped(pip, by);
    if (at == NONE) return;
    landings.push_back({pip, at, pip.pitch});
  }
  ::wrote(landings, ::REVERSED);
}
