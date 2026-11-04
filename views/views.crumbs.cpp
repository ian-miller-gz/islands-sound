// SPDX-License-Identifier: AGPL-3.0-or-later
#include "../score.hpp"
#include "trace/trace.face.hpp"
#include "views.crumbs.hpp"
#include "views.internal.hpp"
#include "views.split.hpp"

namespace {
using namespace SOUND;
using VIEWS::SPLIT::BELOW;
using VIEWS::SPLIT::UPPER;

constexpr STRING::Hot APART = " › ";

struct Door {
  STRING::Hot id = "";
  Whole region = NONE;
};

constexpr Door DOORS[] = {{"views.crumbs", UPPER}, {"split.crumbs", BELOW}};

auto stands(Whole region) -> Flag {
  if (region == BELOW) return VIEWS::SPLIT::lower() != NONE;
  return !String(VIEWS::standing()).empty();
}

void served(GUI::Handle page, const ::Door &door) {
  if (GUI::GET::clicked(page, door.id)) VIEWS::TRACE::unscope(door.region);
  const String text = VIEWS::CRUMBS::read(door.region);
  const Flag shown = !text.empty();
  if (GUI::GET::visibility(page, door.id) != shown)
    GUI::set(page, door.id, GUI::Visibility{shown});
  if (GUI::GET::text(page, door.id) != text)
    GUI::set(page, door.id, GUI::Text{text});
}

}  // namespace

auto SOUND::VIEWS::CRUMBS::read(Whole region) -> String {
  if (!::stands(region)) return {};
  String said;
  for (const String &part : TRACE::crumbs(region))
    said += (said.empty() ? "" : ::APART) + part;
  return said;
}

void SOUND::VIEWS::CRUMBS::frame() {
  const GUI::Handle page = sheet();
  for (const ::Door &door : ::DOORS) ::served(page, door);
}
