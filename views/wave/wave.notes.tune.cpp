// SPDX-License-Identifier: AGPL-3.0-or-later
#include <algorithm>
#include <cmath>

#include "../../timeline.hpp"
#include "wave.notes.hpp"

namespace {
using namespace SOUND;
namespace WAVE = SOUND::VIEWS::WAVE;
using ARRANGEMENT::TRACK::LANE::Tune;

constexpr STRING::Hot INK = "found";
constexpr STRING::Hot LEAF = "tie";
constexpr Float CENT = Float(TIMELINE::SHIFT::SEMITONE);

auto tie(const String &bar) -> String { return bar + "." + ::LEAF; }

auto cents(const WAVE::Clip &clip, const CONTOUR::Found &found) -> Integer {
  const Float middle =
    (Float(found.at) + Float(found.frames) * WAVE::HALF) / clip.ratio;
  for (const Tune &tune : clip.tunes)
    if (middle >= Float(tune.at) && middle < Float(tune.at + tune.frames))
      return tune.cents;
  return 0;
}

}  // namespace

auto SOUND::VIEWS::WAVE::sung(const Clip &clip) -> Vector<Sung> {
  Vector<Sung> sung;
  for (const CONTOUR::Found &found : notes(clip)) {
    const Integer cents = ::cents(clip, found);
    const Float was =
      Float(found.pitch) + (cents == 0 ? ORIGIN : Float(found.cents) / ::CENT);
    sung.push_back({found, was, was + Float(cents) / ::CENT});
  }
  return sung;
}

auto SOUND::VIEWS::WAVE::banded(const Vector<Sung> &sung) -> Band {
  if (sung.empty()) return {};
  Band band = {sung.front().was, sung.front().was};
  for (const Sung &one : sung) {
    band.low = std::min({band.low, one.was, one.now});
    band.high = std::max({band.high, one.was, one.now});
  }
  return band;
}

void SOUND::VIEWS::WAVE::tie(GUI::Handle page, const String &bar) {
  const String id = ::tie(bar);
  GUI::NODES::create(page, bar.c_str(), PANEL, id.c_str());
  GUI::set(page, id.c_str(), GUI::Style{::INK});
}

void SOUND::VIEWS::WAVE::tie(
  GUI::Handle page, const String &bar, Float run, Float rise) {
  const String id = ::tie(bar);
  const Float middle = GUI::GET::extent(page, bar.c_str()).h * HALF;
  const Float hair = COLUMN / scaled();
  GUI::set(page, id.c_str(), GUI::Visibility{rise != ORIGIN});
  GUI::set(
    page, id.c_str(),
    GUI::Position{run * HALF - hair * HALF, middle + std::min(rise, ORIGIN)});
  GUI::set(page, id.c_str(), GUI::Extent{hair, std::abs(rise)});
}
