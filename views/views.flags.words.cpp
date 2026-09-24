// SPDX-License-Identifier: AGPL-3.0-or-later
#include "views.flags.hpp"
#include "views.internal.hpp"

namespace {
using namespace SOUND;

constexpr STRING::Hot INK = "tagword";
constexpr STRING::Hot WORD = "field";
constexpr Float ABOVE = 3.0f;
constexpr Float LETTER = 11.0f;
constexpr Float ORIGIN = 0.0f;

auto named(const String &stem) -> String { return stem + "." + VIEWS::LABEL; }

}  // namespace

void SOUND::VIEWS::word(const String &cloth, const String &stem) {
  const GUI::Handle page = document();
  const String id = ::named(stem);
  GUI::NODES::create(page, cloth.c_str(), ::WORD, id.c_str());
  GUI::set(page, id.c_str(), GUI::Style{::INK});
  GUI::set(page, id.c_str(), GUI::Depth{::ABOVE});
  GUI::set(page, id.c_str(), GUI::Size{::LETTER});
  GUI::set(page, id.c_str(), GUI::Position{::ORIGIN, ::ORIGIN});
  GUI::set(page, id.c_str(), GUI::Visibility{false});
}

void SOUND::VIEWS::unword(const String &stem) {
  GUI::NODES::remove(document(), ::named(stem).c_str());
}

void SOUND::VIEWS::worded(
  const String &cloth, const String &stem, const String &text) {
  const GUI::Handle page = document();
  const String id = ::named(stem);
  const Flag editing = String(GUI::GET::editing(page)) == id;
  GUI::set(page, id.c_str(), GUI::Visibility{editing});
  GUI::set(page, id.c_str(), GUI::GET::extent(page, cloth.c_str()));
  if (!editing) GUI::set(page, id.c_str(), GUI::Text{text});
  GUI::set(page, (cloth + "." + NAMED).c_str(), GUI::Text{text});
}
