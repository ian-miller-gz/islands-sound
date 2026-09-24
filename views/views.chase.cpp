// SPDX-License-Identifier: AGPL-3.0-or-later
#include <island/gui/window.hpp>

#include "../transport.hpp"
#include "views.internal.hpp"

namespace {
using namespace SOUND;

constexpr Float HAIR = 0.0f;

Flag chasing = true;
Flag seeking = false;
Flag within = false;

}  // namespace

auto SOUND::VIEWS::chasing() -> Flag { return ::chasing; }

void SOUND::VIEWS::chasing(Flag on) {
  if (on) ::seeking = true;
  ::chasing = on;
}

void SOUND::VIEWS::wandered() {
  if (TRANSPORT::marker().playing) ::chasing = false;
}

auto SOUND::VIEWS::chased(Float pan, Float at, Float seen) -> Float {
  Float taken = pan;
  if (::seeking) {
    ::seeking = false;
    taken = GUI::SAC::WINDOW::centre(pan, at, ::HAIR, seen);
  } else if (::chasing && TRANSPORT::marker().playing) {
    taken = GUI::SAC::WINDOW::chase(pan, at, seen);
  }
  ::within = at >= taken && at <= taken + seen;
  return taken;
}

auto SOUND::VIEWS::seen() -> Flag { return ::within; }
