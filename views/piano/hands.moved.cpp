// SPDX-License-Identifier: AGPL-3.0-or-later
#include <algorithm>

#include "../../inventory.hpp"
#include "../../score.hpp"
#include "../../transport.hpp"
#include "piano.acts.hpp"
#include "piano.hands.hpp"

namespace {
using namespace SOUND;
namespace ROLL = SOUND::VIEWS::ROLL;

constexpr STRING::Hot NUDGED = "nudge";

auto barred(Whole at) -> Integer {
  return Integer(SCORE::pulsed(TRANSPORT::bar(at), TRANSPORT::paced(at)));
}

auto travel(const ROLL::Step &step, Whole at) -> Integer {
  return step.grains * Integer(ROLL::grained()) + step.bars * ::barred(at);
}

auto octaved(Whole pitch, Integer octaves) -> Whole {
  const Integer wanted = Integer(pitch) + octaves * Integer(ROLL::SEMITONES);
  return wanted < 0 || wanted > Integer(ROLL::HIGHEST) ? NONE : Whole(wanted);
}

auto pitched(const ROLL::Pip &pip, const ROLL::Step &step) -> Whole {
  if (step.octaves != 0) return ::octaved(pip.pitch, step.octaves);
  if (step.rows == 0) return pip.pitch;
  const Whole pitch = ROLL::HANDS::raised(pip, -step.rows, pip.at);
  return pitch == pip.pitch ? NONE : pitch;
}

auto afloat(const ROLL::Pip &pip, Integer by) -> Whole {
  const Integer wanted = Integer(pip.at) + by;
  return wanted < 0 ? NONE : Whole(wanted);
}

auto planned(
  const Vector<ROLL::Pip> &pips, const ROLL::Step &step,
  Vector<ROLL::Landing> &landings) -> Flag {
  if (pips.empty()) return false;
  const Integer by =
    ::travel(step, std::ranges::min(pips, {}, &ROLL::Pip::at).at);
  for (const ROLL::Pip &pip : pips) {
    const Whole pitch = ::pitched(pip, step);
    const Whole at =
      pip.stock == NONE ? ::afloat(pip, by) : ROLL::clipped(pip, by);
    if (pitch == NONE || at == NONE) return false;
    landings.push_back({pip, at, pitch});
  }
  return true;
}

void floated(const Vector<ROLL::Landing> &landings) {
  Vector<ROLL::Pip> marks;
  for (const ROLL::Landing &one : landings) {
    ROLL::shifted(one.pip.index, one.at, one.pitch);
    marks.push_back(one.pip);
  }
  ROLL::follow(marks);
}

}  // namespace

auto SOUND::VIEWS::ROLL::clipped(const Pip &pip, Integer by) -> Whole {
  const Integer wanted = Integer(pip.at) + by;
  for (const Span &span : spans()) {
    if (span.stock != pip.stock || pip.at < span.opens) continue;
    if (pip.at >= span.closes) continue;
    if (wanted < Integer(span.opens) || wanted >= Integer(span.closes))
      return NONE;
    return Whole(wanted);
  }
  return NONE;
}

void SOUND::VIEWS::ROLL::moved(GUI::Handle page, const Step &step) {
  Vector<Pip> pips;
  loosed(pips);
  const Flag floating = !pips.empty();
  if (!floating) pips = seated(page);
  Vector<Landing> landings;
  if (!::planned(pips, step, landings)) return;
  if (floating) return ::floated(landings);
  landed(landings, ::NUDGED);
}
