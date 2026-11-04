// SPDX-License-Identifier: AGPL-3.0-or-later
#include <format>

#include "views.attention.hpp"
#include "views.internal.hpp"
#include "views.split.hpp"

namespace {
using namespace SOUND;
namespace ATTENTION = SOUND::VIEWS::ATTENTION;
using VIEWS::SPLIT::BELOW;
using VIEWS::SPLIT::UPPER;

constexpr Whole FIRST = 1;
constexpr Float GAP = 4.0f;

struct Door {
  STRING::Hot id = "";
  Whole region = NONE;
};

constexpr Door DOORS[] = {
  {"views.attention", UPPER}, {"split.attention", BELOW}};

auto said(Whole slot) -> String { return std::format("{}", slot + ::FIRST); }

auto rows() -> Vector<String> {
  Vector<String> labels;
  for (Whole slot = 0; slot < ATTENTION::SLOTS; ++slot)
    labels.push_back(::said(slot));
  return labels;
}

void raised(GUI::Handle page, const ::Door &door) {
  const GUI::Position at = GUI::GET::origin(page, door.id);
  const GUI::Extent size = GUI::GET::measured(page, door.id);
  VIEWS::MENU::raise(
    ::rows(), at.x, at.y + size.h + ::GAP, door.id,
    ATTENTION::held(door.region));
}

void served(GUI::Handle page, const ::Door &door) {
  const Whole taken = VIEWS::MENU::taken(door.id);
  if (taken != NONE) ATTENTION::hold(door.region, taken);
  if (GUI::GET::clicked(page, door.id)) {
    VIEWS::SPLIT::focus(door.region);
    ::raised(page, door);
  }
  const String text = ::said(ATTENTION::held(door.region));
  if (GUI::GET::text(page, door.id) != text)
    GUI::set(page, door.id, GUI::Text{text});
}

}  // namespace

void SOUND::VIEWS::ATTENTION::doors() {
  const GUI::Handle page = sheet();
  for (const ::Door &door : ::DOORS) ::served(page, door);
}
