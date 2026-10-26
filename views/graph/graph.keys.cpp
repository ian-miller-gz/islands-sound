// SPDX-License-Identifier: AGPL-3.0-or-later
#include <island/input.hpp>

#include "graph.browse.internal.hpp"
#include "graph.keys.hpp"

namespace {
using namespace SOUND;
namespace BROWSE = SOUND::VIEWS::WIRED::BROWSE;
namespace KEYS = INPUT::KEYS;

constexpr Whole BROWSER = 'B';
constexpr Whole FIRST = '!';
constexpr Whole LAST = '~';

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

void typed(GUI::Handle page, const KEYS::Event &key) {
  const String run =
    GUI::GET::text(page, BROWSE::FIND) + static_cast<char>(key.codepoint);
  GUI::edit(page, BROWSE::FIND);
  GUI::set(page, BROWSE::FIND, GUI::Text{run});
  GUI::set(page, BROWSE::FIND, GUI::Caret{run.size(), run.size()});
  GUI::set(page, BROWSE::ROWS, GUI::Cursor{0});
}

void deleted(GUI::Handle page, const KEYS::Event &) {
  VIEWS::WIRED::deleted(page);
}

struct Stroke {
  KEYS::Action action = KEYS::TEXT;
  Whole letter = 0;
  Flag control = false, shift = false;
  Flag browsing = false, printable = false;
  void (*act)(GUI::Handle page, const KEYS::Event &key) = nullptr;
};

constexpr Stroke STROKES[] = {
  {.letter = ::BROWSER, .control = true, .act = ::spread},
  {.letter = ::BROWSER, .control = true, .shift = true, .act = ::raised},
  {.letter = ::BROWSER,
   .control = true,
   .shift = true,
   .browsing = true,
   .act = ::dropped},
  {.action = KEYS::LEFT, .browsing = true, .act = ::climbed},
  {.action = KEYS::ERASE, .browsing = true, .act = ::climbed},
  {.action = KEYS::RIGHT, .browsing = true, .act = ::opened},
  {.browsing = true, .printable = true, .act = ::typed},
  {.action = KEYS::DELETE, .act = ::deleted}};

auto matched(const ::Stroke &stroke, const KEYS::Event &key) -> Flag {
  if (stroke.action != key.action) return false;
  if (stroke.browsing != VIEWS::WIRED::browsing()) return false;
  if (key.action != KEYS::TEXT) return true;
  if (stroke.printable)
    return !key.control && key.codepoint >= ::FIRST && key.codepoint <= ::LAST;
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
