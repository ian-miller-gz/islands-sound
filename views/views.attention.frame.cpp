// SPDX-License-Identifier: AGPL-3.0-or-later
#include "../control.hpp"
#include "views.attention.hpp"
#include "views.split.hpp"

namespace {
using namespace SOUND;
namespace ATTENTION = SOUND::VIEWS::ATTENTION;

constexpr STRING::Hot LIST = "tracks";

void follow() {
  const ATTENTION::Slot &slot = ATTENTION::get(ATTENTION::shown());
  const String rows = String(::LIST) + "." + VIEWS::ROWS;
  GUI::set(VIEWS::SPLIT::home(), rows.c_str(), GUI::Cursor{slot.track});
  CONTROL::aim(slot.node);
}

}  // namespace

void SOUND::VIEWS::ATTENTION::frame() {
  const Whole now = held(SPLIT::focused());
  if (now == shown()) return;
  attended(get(shown()));
  aimed(get(shown()));
  show(now);
  ::follow();
}
