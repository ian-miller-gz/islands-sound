// SPDX-License-Identifier: AGPL-3.0-or-later
#include <island/gui/nga.hpp>

#include "piano.beams.hpp"
#include "piano.weights.hpp"

namespace {
using namespace SOUND;
namespace ROLL = SOUND::VIEWS::ROLL;
namespace WEIGHTS = SOUND::VIEWS::ROLL::WEIGHTS;

constexpr STRING::Hot UP[] = {"", "\U0001D16E", "\U0001D16F"};
constexpr Float BOW = 0.0f, THICKNESS = 0.2f, RISE = 2.0f, STILL = 0.0f;
constexpr Float RANK = WEIGHTS::BEAM + WEIGHTS::GAP;
constexpr Float FULL = ROLL::SPACE * ROLL::STAFF::SCALE;

struct Ink {
  Float wide = 0.0f, top = 0.0f;
};
constexpr Ink FLAG = {0.274f, 1.009f};

}  // namespace

auto SOUND::VIEWS::ROLL::STAFF::scaled() -> Scale {
  const GUI::NGA::Zoom zoom =
    GUI::NGA::GET::zoom(VIEWS::document(), board().c_str());
  const Float side = HEADS::side(plates()) / ::FULL;
  if (zoom.value <= ::STILL || zoom.down <= ::STILL) return {side, side};
  return {side / zoom.value, side / zoom.down};
}

void SOUND::VIEWS::ROLL::STAFF::hang(
  const String &id, const Stem &stem, Scale scale) {
  const GUI::Handle page = VIEWS::document();
  const Float lift = (BASELINE - ::FLAG.top) * EM * scale.down;
  GUI::set(page, id.c_str(), GUI::Text{::UP[stem.beams]});
  GUI::set(page, id.c_str(), GUI::Position{anchored(stem), stem.tip - lift});
  GUI::set(page, id.c_str(), GUI::Size{EM * scale.across});
  GUI::set(
    page, id.c_str(),
    GUI::Extent{::FLAG.wide * EM * scale.across, EM * scale.down});
}

void SOUND::VIEWS::ROLL::STAFF::bow(
  const String &id, const Stem &stem, Whole rank, Scale scale) {
  const Float stand = WEIGHTS::drawn(WEIGHTS::Across{WEIGHTS::STEM});
  const Float step = WEIGHTS::drawn(WEIGHTS::Down{::RANK});
  const GUI::Position near = {
    anchored(stem) + HALF * stand, stem.tip - Float(rank) * step};
  const GUI::Position far = {
    near.x + ::FLAG.wide * EM * scale.across,
    near.y - ::RISE * SPACE * scale.down};
  cut(id, Ends{near, far});
  const GUI::Handle page = VIEWS::document();
  GUI::set(
    page, id.c_str(),
    GUI::Border{WEIGHTS::drawn(WEIGHTS::Across{::THICKNESS})});
  GUI::set(page, id.c_str(), GUI::Shape{GUI::Shape::RISE});
  GUI::set(page, id.c_str(), GUI::Bend{::BOW});
}
