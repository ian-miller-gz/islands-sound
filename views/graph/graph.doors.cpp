// SPDX-License-Identifier: AGPL-3.0-or-later
#include <algorithm>
#include <format>

#include "../../graph.hpp"
#include "graph.internal.hpp"

namespace {
using namespace SOUND;

constexpr STRING::Hot PALETTE = "graph.palette";
constexpr STRING::Hot DOOR = "graph.palette.door";
constexpr STRING::Hot PLUS = "graph.plus";
constexpr STRING::Hot HINTED = "hint";
constexpr STRING::Hot WORN = "place";
constexpr Float LETTER = 14.0f;
constexpr Float PAD = 8.0f;
constexpr Float WEST = 8.0f;
constexpr Float APART = 6.0f;
constexpr Float NORTH = 3.0f;
constexpr Float TALL = 24.0f;
constexpr Float BAND = 30.0f;
constexpr Float SQUARE = 24.0f;

Whole stood = 0;
Whole born = 0;

auto named(Whole at) -> String { return std::format("{}.{}", ::DOOR, at); }

auto wide(const String &word) -> Float {
  return ::PAD * 2.0f + VIEWS::WIRED::ADVANCE * ::LETTER * Float(word.size());
}

void build(Whole count) {
  const GUI::Handle page = VIEWS::document();
  for (Whole at = count; at < ::stood; ++at)
    GUI::NODES::remove(page, ::named(at).c_str());
  for (Whole at = ::stood; at < count; ++at) {
    const String id = ::named(at);
    GUI::NODES::create(page, ::PALETTE, "button", id.c_str());
    GUI::set(page, id.c_str(), GUI::Style{::WORN});
    GUI::set(page, id.c_str(), GUI::Size{::LETTER});
    const String hint = id + "." + ::HINTED;
    GUI::NODES::create(page, id.c_str(), "label", hint.c_str());
    GUI::set(page, hint.c_str(), GUI::Visibility{false});
  }
  ::stood = count;
}

}  // namespace

void SOUND::VIEWS::WIRED::doors() {
  const GUI::Handle page = VIEWS::document();
  const Vector<PALETTE::Door> rows = PALETTE::doors();
  if (VIEWS::reborn(::born)) ::stood = 0;
  if (rows.size() != ::stood) ::build(rows.size());
  Float across = ::WEST;
  for (Whole at = 0; at < rows.size(); ++at) {
    const String id = ::named(at);
    const Float run = ::wide(rows[at].word);
    GUI::set(page, id.c_str(), GUI::Position{across, ::NORTH});
    GUI::set(page, id.c_str(), GUI::Extent{run, ::TALL});
    GUI::set(page, id.c_str(), GUI::Text{rows[at].word});
    GUI::set(
      page, (id + "." + ::HINTED).c_str(),
      GUI::Text{PALETTE::sentence(rows[at])});
    if (GUI::GET::clicked(page, id.c_str())) {
      if (browsing() && filed() == rows[at].filing)
        raise(false);
      else
        raise(rows[at].filing);
    }
    across += run + ::APART;
  }
  GUI::set(page, ::PLUS, GUI::Position{across, ::NORTH});
  GUI::set(page, ::PALETTE, GUI::Extent{across + ::SQUARE + ::PAD, ::BAND});
}
