// SPDX-License-Identifier: AGPL-3.0-or-later
#include <algorithm>

#include "views.internal.hpp"

namespace {
using namespace SOUND;

Vector<VIEWS::View> registered;
GUI::Handle page = GUI::NONE;
Whole viewed = 0;
Vector<Flag> up;

}  // namespace

auto SOUND::VIEWS::registry() -> Vector<View> & { return ::registered; }
auto SOUND::VIEWS::sheet() -> GUI::Handle & { return ::page; }
auto SOUND::VIEWS::chosen() -> Whole & { return ::viewed; }
auto SOUND::VIEWS::standings() -> Vector<Flag> & { return ::up; }

auto SOUND::VIEWS::roster() -> const Vector<View> & { return ::registered; }

auto SOUND::VIEWS::document() -> GUI::Handle { return ::page; }

auto SOUND::VIEWS::standing() -> STRING::Hot {
  return ::viewed < ::registered.size() ? ::registered[::viewed].name : "";
}

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
