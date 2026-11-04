// SPDX-License-Identifier: AGPL-3.0-or-later
#include "graph.browse.internal.hpp"
#include "graph.keys.hpp"
#include "../views.hotkeys.hpp"

namespace {
using namespace SOUND;
namespace BROWSE = SOUND::VIEWS::WIRED::BROWSE;
namespace KEYS = INPUT::KEYS;
namespace HOTKEYS = SOUND::VIEWS::HOTKEYS;

constexpr Whole BROWSER = 'B', ANY = 0;
constexpr STRING::Hot UP = "up";

void spread(GUI::Handle, const KEYS::Event &) {
  BROWSE::quick(!BROWSE::quicked());
}

void raised(GUI::Handle, const KEYS::Event &) {
  BROWSE::quick(false);
  BROWSE::enter(String());
}

void dropped(GUI::Handle, const KEYS::Event &) { VIEWS::WIRED::raise(false); }

void climbed(GUI::Handle, const KEYS::Event &) {
  const String filing = VIEWS::WIRED::filed();
  if (!filing.empty()) BROWSE::enter(BROWSE::up(filing));
}

void opened(GUI::Handle page, const KEYS::Event &) {
  const Vector<BROWSE::Offer> rows = BROWSE::shown(VIEWS::WIRED::filed());
  const Whole row = GUI::GET::cursor(page, BROWSE::ROWS);
  if (row < rows.size() && rows[row].directory) BROWSE::enter(rows[row].among);
}

struct Stroke {
  KEYS::Action action = KEYS::TEXT;
  Whole letter = ::ANY;
  Flag control = false, shift = false, browsing = false;
  void (*act)(GUI::Handle page, const KEYS::Event &key) = nullptr;
  STRING::Hot name = "";
};

constexpr Stroke STROKES[] = {
  {.letter = ::BROWSER, .control = true, .act = ::spread, .name = "add"},
  {.letter = ::BROWSER,
   .control = true,
   .shift = true,
   .act = ::raised,
   .name = "browse"},
  {.letter = ::BROWSER,
   .control = true,
   .shift = true,
   .browsing = true,
   .act = ::dropped,
   .name = "close"},
  {.action = KEYS::LEFT, .browsing = true, .act = ::climbed, .name = ::UP},
  {.action = KEYS::ERASE, .browsing = true, .act = ::climbed, .name = ::UP},
  {.action = KEYS::RIGHT, .browsing = true, .act = ::opened, .name = "open"},
  {.browsing = true, .act = BROWSE::typed},
  {.action = KEYS::DELETE, .act = VIEWS::WIRED::deleted, .name = "delete"}};

[[maybe_unused]] const Flag listed =
  HOTKEYS::offer(HOTKEYS::FAMILY::GRAPH, ::STROKES);

auto matched(const ::Stroke &stroke, const KEYS::Event &key) -> Flag {
  if (stroke.action != key.action) return false;
  if (stroke.browsing != VIEWS::WIRED::browsing()) return false;
  if (key.action != KEYS::TEXT || stroke.letter == ::ANY) return true;
  return VIEWS::folded(key.codepoint) == stroke.letter &&
         key.control == stroke.control && key.shift == stroke.shift;
}

}  // namespace

void SOUND::VIEWS::WIRED::keys() {
  const KEYS::Event key = VIEWS::struck();
  if (VIEWS::journaled(key)) return;
  for (const ::Stroke &stroke : ::STROKES)
    if (::matched(stroke, key)) return stroke.act(VIEWS::document(), key);
}
