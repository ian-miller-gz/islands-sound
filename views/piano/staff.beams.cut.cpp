// SPDX-License-Identifier: AGPL-3.0-or-later
#include "piano.beams.hpp"
#include "piano.weights.hpp"

namespace {
using namespace SOUND;
namespace WEIGHTS = SOUND::VIEWS::ROLL::WEIGHTS;
}  // namespace

void SOUND::VIEWS::ROLL::STAFF::cut(const String &id, const Box &box) {
  const GUI::Handle page = VIEWS::document();
  GUI::set(page, id.c_str(), box.at);
  GUI::set(page, id.c_str(), box.size);
}

void SOUND::VIEWS::ROLL::STAFF::cut(const String &id, const Ends &ends) {
  const GUI::Handle page = VIEWS::document();
  const GUI::Position run = {
    ends.far.x - ends.near.x, ends.far.y - ends.near.y};
  GUI::set(page, id.c_str(), ends.near);
  GUI::set(page, id.c_str(), GUI::Ends{run});
  GUI::set(
    page, id.c_str(),
    GUI::Border{WEIGHTS::drawn(WEIGHTS::Across{WEIGHTS::BEAM})});
}
