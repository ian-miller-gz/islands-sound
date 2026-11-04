// SPDX-License-Identifier: AGPL-3.0-or-later
#include "piano.acts.hpp"

namespace {
using namespace SOUND::VIEWS::ROLL;

struct Door {
  STRING::Hot id = nullptr;
  void (*act)(GUI::Handle page) = nullptr;
};

constexpr Door DOORS[] = {
  {"piano.invert", inverted},
  {"piano.reverse", reversed},
};

}  // namespace

void SOUND::VIEWS::ROLL::mirrors(GUI::Handle page) {
  for (const ::Door &door : ::DOORS)
    if (GUI::GET::clicked(page, door.id)) door.act(page);
}
