// SPDX-License-Identifier: AGPL-3.0-or-later
#include "views.curves.hpp"

void SOUND::VIEWS::stroke(
  GUI::Handle page, const String &id, const Fall &fall) {
  const Curve drawn = curve(fall.shape);
  GUI::set(page, id.c_str(), fall.at);
  GUI::set(page, id.c_str(), GUI::Ends{{fall.run, -fall.deep}});
  GUI::set(page, id.c_str(), GUI::Shape{drawn.run});
  GUI::set(page, id.c_str(), GUI::Bend{drawn.bend});
  GUI::set(page, id.c_str(), GUI::Border{fall.weight});
}
