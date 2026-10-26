// SPDX-License-Identifier: AGPL-3.0-or-later
#include <format>

#include <island/gui/nga.hpp>

#include "../../kind.hpp"
#include "../../timeline.hpp"
#include "wave.notes.hpp"
#include "wave.overs.hpp"

namespace {
using namespace SOUND;
namespace WAVE = SOUND::VIEWS::WAVE;

constexpr STRING::Hot LINE = "wave clips {} zoom {:g}";

void bound() {
  GUI::NGA::set(
    VIEWS::document(), WAVE::board().c_str(),
    GUI::NGA::Bounds{
      WAVE::ORIGIN, GUI::NGA::Bounds::NONE, WAVE::ORIGIN, WAVE::ORIGIN});
}

void run() {
  ::bound();
  WAVE::hands();
  WAVE::keyed();
  WAVE::ruler();
  WAVE::clips();
  WAVE::peaks();
  WAVE::lines();
  WAVE::onsets();
  WAVE::notes();
  WAVE::grips();
  WAVE::handles();
  WAVE::say();
  WAVE::shifts();
  WAVE::menu();
  WAVE::rows();
}

void state(SHELL::Session &session) {
  session.print(std::format(::LINE, WAVE::placed().size(), WAVE::scaled()));
  WAVE::clips(session);
  WAVE::overs(session);
  WAVE::onsets(session);
  WAVE::notes(session);
  WAVE::rows(session);
}

void shed() {
  WAVE::SHED::peaks();
  WAVE::SHED::lines();
  WAVE::SHED::onsets();
  WAVE::SHED::notes();
  WAVE::SHED::band();
  WAVE::SHED::grips();
  WAVE::SHED::handles();
  WAVE::SHED::reading();
  WAVE::SHED::shifts();
  WAVE::SHED::clips();
  WAVE::SHED::ruler();
}

auto least() -> GUI::Extent { return {WAVE::least(), WAVE::ORIGIN}; }

}  // namespace

auto SOUND::VIEWS::WAVE::face() -> const View & {
  static const View unit = {
    .name = NAME,
    .flat = true,
    .least = ::least,
    .controls = {ZOOM, PAN},
    .run = ::run,
    .state = ::state,
    .overview = overview,
    .shed = ::shed};
  return unit;
}

auto SOUND::VIEWS::WAVE::board() -> String {
  return String(NAME) + "." + BOARD;
}

auto SOUND::VIEWS::WAVE::strip() -> String {
  return String(NAME) + "." + RULER;
}

auto SOUND::VIEWS::WAVE::laned() -> LANE::Mark {
  return LANE::edited(KIND::AUDIO);
}

auto SOUND::VIEWS::WAVE::placed() -> Vector<Clip> {
  const LANE::Mark mark = laned();
  if (mark.lane == NONE) return {};
  return TIMELINE::spans(mark.track, mark.lane);
}
