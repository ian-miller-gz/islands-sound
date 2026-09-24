// SPDX-License-Identifier: AGPL-3.0-or-later
#include <algorithm>
#include <utility>

#include <metrics.hpp>

#include <island/graphics/backend/passes.hpp>
#include <island/graphics/windows.hpp>

#include "../boards.hpp"
#include "views.internal.hpp"

namespace {
using namespace SOUND;

constexpr STRING::Hot LAYOUT = "sound.gui";
constexpr STRING::Hot NIGHT = "night";
constexpr STRING::Hot DAY = "day";

Vector<VIEWS::View> registered;
GUI::Handle page = GUI::NONE;
Whole viewed = 0;
Vector<Flag> up;

auto ground() -> GFX::Color {
  return GUI::GET::style(page, GUI::GET::theme() == GUI::DARK ? NIGHT : DAY)
    .color;
}

void draw(const GFX::Viewport &viewport) {
  METRICS::gauge("sound.nodes", METRICS::Sample(BOARDS::seated()));
  METRICS::count("sound.restructured", GUI::GET::restructured(page));
  GFX::Pass pass = {.clear = ground(), .viewport = viewport};
  {
    METRICS::Scope span("sound.flush");
    GUI::flush(page, pass);
  }
  GFX::PASSES::submit(std::move(pass));
}

}  // namespace

auto SOUND::VIEWS::registry() -> Vector<View> & { return ::registered; }
auto SOUND::VIEWS::sheet() -> GUI::Handle & { return ::page; }
auto SOUND::VIEWS::chosen() -> Whole & { return ::viewed; }
auto SOUND::VIEWS::standings() -> Vector<Flag> & { return ::up; }

auto SOUND::VIEWS::roster() -> const Vector<View> & { return ::registered; }

auto SOUND::VIEWS::offer(const View &unit) -> Flag {
  if (unit.name == nullptr || unit.name[0] == '\0') return false;
  if (found(unit.name) != nullptr) return false;
  ::registered.push_back(unit);
  std::sort(
    ::registered.begin(), ::registered.end(),
    [](const View &one, const View &two) {
      return String(one.name) < String(two.name);
    });
  ::up.assign(::registered.size(), false);
  return true;
}

auto SOUND::VIEWS::found(const String &name) -> const View * {
  for (const View &unit : ::registered)
    if (name == unit.name) return &unit;
  return nullptr;
}

auto SOUND::VIEWS::document() -> GUI::Handle { return ::page; }

auto SOUND::VIEWS::open(STRING::Hot assets) -> Status {
  ICONS::create();
  settled();
  ::page = GUI::mount(assets, ::LAYOUT);
  if (::page == GUI::NONE) return 1;
  for (Whole row = 0; row < ::registered.size(); ++row) {
    ::up[row] = ::registered[row].panel;
    if (::registered[row].opening && !::registered[row].panel) ::viewed = row;
    if (::registered[row].seat != nullptr) ::registered[row].seat();
  }
  show();
  return 0;
}

auto SOUND::VIEWS::standing() -> STRING::Hot {
  return ::viewed < ::registered.size() ? ::registered[::viewed].name : "";
}

auto SOUND::VIEWS::choose(const String &name) -> Flag {
  for (Whole row = 0; row < ::registered.size(); ++row)
    if (name == ::registered[row].name && !::registered[row].panel) {
      ::viewed = row;
      show();
      return true;
    }
  return false;
}

auto SOUND::VIEWS::raised(const String &name) -> Flag {
  for (Whole row = 0; row < ::registered.size(); ++row)
    if (name == ::registered[row].name) return ::up[row];
  return false;
}

auto SOUND::VIEWS::raise(const String &name, Flag on) -> Flag {
  for (Whole row = 0; row < ::registered.size(); ++row)
    if (name == ::registered[row].name && ::registered[row].panel) {
      ::up[row] = on;
      show();
      return true;
    }
  return false;
}

auto SOUND::VIEWS::owns(const String &name, const String &control) -> Flag {
  const View *unit = found(name);
  if (unit == nullptr) return false;
  for (STRING::Hot held : unit->controls)
    if (control == held) return true;
  return false;
}

void SOUND::VIEWS::frame() {
  if (::page == GUI::NONE) return;
  dressed();
  rebirth();
  glide();
  GUI::focus(::page, true);
  const GFX::Viewport viewport = GFX::WINDOWS::MAIN::viewport();
  GUI::place(::page, viewport);
  lift();
  GUI::poll();
  chose();
  deck();
  column();
  wheeled();
  MENU::frame();
  file();
  exporting();
  settings();
  doors();
  for (Whole row = 0; row < ::registered.size(); ++row)
    if ((::up[row] || row == ::viewed) && ::registered[row].run != nullptr)
      ::registered[row].run();
  show();
  ::draw(viewport);
}

void SOUND::VIEWS::close() {
  if (::page != GUI::NONE) GUI::remove(::page);
  ::page = GUI::NONE;
}
