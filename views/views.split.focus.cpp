// SPDX-License-Identifier: AGPL-3.0-or-later
#include "../graph.hpp"
#include "views.internal.hpp"
#include "views.split.internal.hpp"

namespace {
using namespace SOUND;
using VIEWS::SPLIT::BELOW;
using VIEWS::SPLIT::UPPER;

struct Owner {
  String head, view;
  Whole region = NONE;
};

Whole fact = UPPER;

auto owners() -> Vector<Owner> & {
  static Vector<Owner> rows;
  return rows;
}

auto row(const String &name) -> Whole {
  const Vector<VIEWS::View> &units = VIEWS::roster();
  for (Whole at = 0; at < units.size(); ++at)
    if (!units[at].panel && name == units[at].name) return at;
  return NONE;
}

auto region(Whole at, Whole own) -> Whole {
  if (at == NONE) return NONE;
  if (VIEWS::SPLIT::shared() && at == VIEWS::chosen()) return own;
  if (at == VIEWS::chosen()) return UPPER;
  return at == VIEWS::SPLIT::lower() ? BELOW : NONE;
}

auto headed(const String &id) -> Whole {
  const String head = id.substr(0, id.find(VIEWS::SPLIT::DOT));
  const Whole at = ::row(head);
  if (at != NONE) return ::region(at, NONE);
  for (const ::Owner &owner : ::owners())
    if (head == owner.head) return ::region(::row(owner.view), owner.region);
  return NONE;
}

}  // namespace

void SOUND::VIEWS::SPLIT::pressed(GUI::Handle page) {
  if (!open()) return;
  if (!GUI::GET::pressed(page)) return;
  if (GUI::GET::echoed(page)) return void(::fact = BELOW);
  const String id = landed(page);
  const Whole place = placed(id);
  const Whole head = place == NONE ? ::headed(id) : place;
  const Whole region = id.empty() ? spotted(page) : head;
  if (region != NONE) ::fact = region;
}

auto SOUND::VIEWS::SPLIT::focused() -> Whole { return open() ? ::fact : UPPER; }

void SOUND::VIEWS::SPLIT::focus(Whole region) { ::fact = region; }

auto SOUND::VIEWS::SPLIT::aimed() -> const View * {
  const Whole at = focused() == BELOW ? lower() : NONE;
  return at == NONE ? found(String(standing())) : &roster()[at];
}

auto SOUND::VIEWS::SPLIT::heads() -> Vector<String> {
  Vector<String> words;
  for (const ::Owner &owner : ::owners()) words.push_back(owner.head);
  return words;
}

void SOUND::VIEWS::SPLIT::owned(
  STRING::Hot head, STRING::Hot view, Whole region) {
  const ::Owner row = {head, view, region};
  for (::Owner &owner : ::owners())
    if (owner.head == row.head) {
      owner = row;
      return;
    }
  ::owners().push_back(row);
}
