// SPDX-License-Identifier: AGPL-3.0-or-later
#include <island/gui/nga.hpp>

#include "../../boards.hpp"
#include "lanes.internal.hpp"

namespace {
using namespace SOUND;

constexpr STRING::Hot HEADED = "header";
constexpr STRING::Hot DRESSED = "dress";
constexpr STRING::Hot LABELED = "label";
constexpr STRING::Hot INK = "muted";
constexpr Float LETTER = 12.0f;

Vector<String> stood;
Whole born = 0;

auto cell(const String &id) -> String { return id + "." + ::LABELED; }

auto header(const String &id) -> String { return id + "." + ::HEADED; }

auto dress(const String &id) -> String {
  return ::header(id) + "." + ::DRESSED;
}

void heading(const String &id) {
  const GUI::Handle page = VIEWS::document();
  const String head = ::header(id);
  BOARDS::place(
    page, VIEWS::ARRANGEMENT::board().c_str(), "panel", head, {},
    {VIEWS::ARRANGEMENT::GUTTER, 0.0f});
  GUI::set(page, head.c_str(), GUI::Style{::HEADED});
  GUI::set(page, head.c_str(), GUI::Depth{VIEWS::ARRANGEMENT::HEADER});
  GUI::NGA::set(page, head.c_str(), GUI::NGA::Pinned{GUI::NGA::Pinned::ACROSS});
  BOARDS::place(page, head.c_str(), "panel", ::dress(id), {}, {});
}

void naming(const String &id) {
  const GUI::Handle page = VIEWS::document();
  const String label = ::cell(id);
  BOARDS::place(
    page, VIEWS::ARRANGEMENT::board().c_str(), "label", label, {},
    {VIEWS::ARRANGEMENT::NAMED, ::LETTER});
  GUI::set(page, label.c_str(), GUI::Style{::INK});
  GUI::set(page, label.c_str(), GUI::Size{::LETTER});
  GUI::NGA::set(
    page, label.c_str(), GUI::NGA::Pinned{GUI::NGA::Pinned::ACROSS});
  GUI::set(page, label.c_str(), GUI::Depth{VIEWS::ARRANGEMENT::CREST});
}

void build(const Vector<VIEWS::ARRANGEMENT::Stand> &stands) {
  const GUI::Handle page = VIEWS::document();
  for (Whole row = stands.size(); row < ::stood.size(); ++row)
    for (const String &part : {::header(::stood[row]), ::cell(::stood[row])})
      GUI::NODES::remove(page, part.c_str());
  for (Whole row = ::stood.size(); row < stands.size(); ++row) {
    ::heading(stands[row].id);
    ::naming(stands[row].id);
  }
}

}  // namespace

void SOUND::VIEWS::ARRANGEMENT::cells(const Vector<Stand> &stands) {
  const GUI::Handle page = VIEWS::document();
  if (VIEWS::reborn(::born)) ::stood.clear();
  if (stands.size() != ::stood.size()) ::build(stands);
  ::stood.clear();
  for (const Stand &stand : stands) ::stood.push_back(stand.id);
  for (Whole row = 0; row < stands.size(); ++row) {
    const String head = ::header(stands[row].id);
    const String worn = ::dress(stands[row].id);
    GUI::set(page, head.c_str(), GUI::Position{0.0f, rail(row)});
    GUI::set(page, head.c_str(), GUI::Extent{GUTTER, depth(row)});
    GUI::set(page, worn.c_str(), GUI::Extent{GUTTER, depth(row)});
    GUI::set(page, worn.c_str(), GUI::Style{stands[row].dress.c_str()});
    GUI::set(
      page, ::cell(stands[row].id).c_str(),
      GUI::Position{INSET, rail(row) + CURB + PAD});
    GUI::set(
      page, ::cell(stands[row].id).c_str(),
      GUI::Text{stands[row].laden ? String(stands[row].kind) : String()});
  }
}
