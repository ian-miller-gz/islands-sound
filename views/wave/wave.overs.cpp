// SPDX-License-Identifier: AGPL-3.0-or-later
#include <algorithm>
#include <format>

#include "wave.overs.hpp"

namespace {
using namespace SOUND;
namespace WAVE = SOUND::VIEWS::WAVE;

constexpr STRING::Hot LINE = "wave overs {} {}";
constexpr STRING::Hot WORN[] = {WAVE::PEAK, WAVE::OVER};
constexpr Whole LEAST = 1;

Vector<Flag> worn;

void wear(GUI::Handle page, const String &id, Whole at, Flag over) {
  if (at >= ::worn.size()) ::worn.resize(at + 1, false);
  ::worn[at] = over;
  GUI::set(page, id.c_str(), GUI::Style{::WORN[over]});
}

auto counted(const WAVE::Clip &clip, Whole step) -> Whole {
  Whole count = 0;
  for (Whole opens = 0; opens < clip.frames; opens += step) {
    const PEAKS::Peak peak =
      WAVE::peak(clip, opens, std::min(step, clip.frames - opens));
    if (WAVE::clipped(peak.least, peak.most)) ++count;
  }
  return count;
}

}  // namespace

auto SOUND::VIEWS::WAVE::clipped(Float least, Float most) -> Flag {
  return most >= CEILING || least <= -CEILING;
}

void SOUND::VIEWS::WAVE::dressed(GUI::Handle page, const String &id, Whole at) {
  ::wear(page, id, at, false);
}

void SOUND::VIEWS::WAVE::dressed(
  GUI::Handle page, const String &id, Whole at, const PEAKS::Peak &peak) {
  const Flag over = clipped(peak.least, peak.most);
  if (at < ::worn.size() && ::worn[at] == over) return;
  ::wear(page, id, at, over);
}

void SOUND::VIEWS::WAVE::overs(SHELL::Session &session) {
  const Float scale = scaled();
  if (scale <= ORIGIN) return;
  const Whole step = std::max(Whole(COLUMN / scale), ::LEAST);
  const Vector<Clip> seated = placed();
  for (Whole plate = 0; plate < seated.size(); ++plate) {
    const Whole count = ::counted(seated[plate], step);
    if (count > 0) session.print(std::format(::LINE, plate, count));
  }
}
