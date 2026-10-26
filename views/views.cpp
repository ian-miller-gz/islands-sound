// SPDX-License-Identifier: AGPL-3.0-or-later
#include <island/graphics/windows.hpp>

#include "views.draw.hpp"
#include "views.internal.hpp"
#include "views.menu.acts.hpp"
#include "views.split.hpp"

namespace {
using namespace SOUND;

constexpr STRING::Hot LAYOUT = "sound.gui";

auto running(Whole row) -> Flag {
  const Flag up = VIEWS::standings()[row] || row == VIEWS::chosen();
  const Flag on = up || row == VIEWS::SPLIT::lower();
  return on && VIEWS::registry()[row].run != nullptr;
}

}  // namespace

auto SOUND::VIEWS::open(STRING::Hot assets) -> Status {
  ICONS::create();
  settled();
  sheet() = GUI::mount(assets, ::LAYOUT);
  if (sheet() == GUI::NONE) return 1;
  SPLIT::mount(assets, ::LAYOUT);
  Vector<View> &units = registry();
  for (Whole row = 0; row < units.size(); ++row) {
    standings()[row] = units[row].panel;
    if (units[row].opening && !units[row].panel) chosen() = row;
    if (units[row].seat != nullptr) units[row].seat();
  }
  show();
  return 0;
}

auto SOUND::VIEWS::choose(const String &name) -> Flag {
  const Vector<View> &units = registry();
  for (Whole row = 0; row < units.size(); ++row)
    if (name == units[row].name && !units[row].panel) {
      SPLIT::traded(row);
      chosen() = row;
      show();
      return true;
    }
  return false;
}

void SOUND::VIEWS::frame() {
  if (sheet() == GUI::NONE) return;
  dressed();
  rebirth();
  glide();
  GUI::focus(sheet(), true);
  const GFX::Viewport viewport = GFX::WINDOWS::MAIN::viewport();
  GUI::place(sheet(), viewport);
  lift();
  polled();
  chose();
  deck();
  column();
  SPLIT::frame();
  wheeled();
  MENU::frame();
  MENU::keys();
  file();
  exporting();
  settings();
  doors();
  for (Whole row = 0; row < registry().size(); ++row)
    if (::running(row)) registry()[row].run();
  show();
  draw(viewport);
}

void SOUND::VIEWS::close() {
  if (sheet() != GUI::NONE) GUI::remove(sheet());
  sheet() = GUI::NONE;
}
