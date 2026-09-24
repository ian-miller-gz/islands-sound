// SPDX-License-Identifier: AGPL-3.0-or-later
#include <island/gui/nga.hpp>

#include "../../boards.hpp"
#include "../../control.hpp"
#include "lanes.internal.hpp"

namespace {
using namespace SOUND;

constexpr STRING::Hot FOLDED = "fold";
constexpr Float CARET = 12.0f;
constexpr STRING::Hot ARMED = "arm";
constexpr STRING::Hot MARKED = "armed";
constexpr STRING::Hot PLAIN = "icon";
constexpr STRING::Hot DOT = "\u25cf";

struct Folded {
  String track, lane;
};

Vector<Folded> folded;

Vector<VIEWS::ARRANGEMENT::Stand> stood;
Whole born = 0;

auto caret(const String &id) -> String { return id + "." + ::FOLDED; }

auto armer(const String &id) -> String { return id + "." + ::ARMED; }

auto seated(Whole row) -> Float {
  return VIEWS::ARRANGEMENT::rail(row) + VIEWS::ARRANGEMENT::CURB +
         VIEWS::ARRANGEMENT::PAD;
}

void fold(const String &track, const String &lane) {
  for (Whole at = 0; at < ::folded.size(); ++at)
    if (::folded[at].track == track && ::folded[at].lane == lane)
      return (void)::folded.erase(::folded.begin() + at);
  ::folded.push_back({track, lane});
}

void settle(const Vector<VIEWS::ARRANGEMENT::Stand> &stands) {
  Vector<Folded> kept;
  for (const Folded &fold : ::folded)
    for (const VIEWS::ARRANGEMENT::Stand &stand : stands)
      if (stand.name == fold.track && stand.standing == fold.lane) {
        kept.push_back(fold);
        break;
      }
  ::folded = kept;
}

auto arming(Whole track) -> Flag {
  return CONTROL::reel().armed && CONTROL::driven() == track;
}

void arm(Whole track) {
  if (::arming(track)) return CONTROL::arm(false);
  CONTROL::drive(track);
  CONTROL::arm(true);
}

void door(const String &id) {
  const GUI::Handle page = VIEWS::document();
  for (const String &part : {::caret(id), ::armer(id)}) {
    BOARDS::place(
      page, VIEWS::ARRANGEMENT::board().c_str(), "button", part, {},
      {::CARET, ::CARET});
    GUI::NGA::set(
      page, part.c_str(), GUI::NGA::Pinned{GUI::NGA::Pinned::ACROSS});
    GUI::set(page, part.c_str(), GUI::Depth{VIEWS::ARRANGEMENT::CREST});
  }
  GUI::set(page, ::armer(id).c_str(), GUI::Text{String(::DOT)});
}

void build(const Vector<VIEWS::ARRANGEMENT::Stand> &stands) {
  const GUI::Handle page = VIEWS::document();
  for (Whole row = stands.size(); row < ::stood.size(); ++row)
    for (const String &part :
         {::caret(::stood[row].id), ::armer(::stood[row].id)})
      GUI::NODES::remove(page, part.c_str());
  for (Whole row = ::stood.size(); row < stands.size(); ++row)
    ::door(stands[row].id);
}

void carets(const Vector<VIEWS::ARRANGEMENT::Stand> &stands) {
  const GUI::Handle page = VIEWS::document();
  for (Whole row = 0; row < stands.size(); ++row) {
    const String id = ::caret(stands[row].id);
    GUI::set(
      page, id.c_str(),
      GUI::Position{
        VIEWS::ARRANGEMENT::GUTTER - ::CARET - VIEWS::ARRANGEMENT::INSET,
        ::seated(row)});
    GUI::set(page, id.c_str(), GUI::Visibility{stands[row].laden});
    GUI::set(
      page, id.c_str(),
      GUI::Text{
        VIEWS::ARRANGEMENT::shut(stands[row].name, stands[row].standing)
          ? String("+")
          : String("-")});
  }
}

void arms(const Vector<VIEWS::ARRANGEMENT::Stand> &stands) {
  const GUI::Handle page = VIEWS::document();
  for (Whole row = 0; row < stands.size(); ++row) {
    const String id = ::armer(stands[row].id);
    GUI::set(
      page, id.c_str(),
      GUI::Position{
        VIEWS::ARRANGEMENT::GUTTER - ::CARET * 2.0f -
          VIEWS::ARRANGEMENT::INSET * 2.0f,
        ::seated(row)});
    GUI::set(page, id.c_str(), GUI::Visibility{stands[row].hears});
    GUI::set(
      page, id.c_str(),
      GUI::Style{::arming(stands[row].track) ? ::MARKED : ::PLAIN});
  }
}

}  // namespace

auto SOUND::VIEWS::ARRANGEMENT::shut(const String &track, const String &lane)
  -> Flag {
  for (const Folded &fold : ::folded)
    if (fold.track == track && fold.lane == lane) return true;
  return false;
}

void SOUND::VIEWS::ARRANGEMENT::doors() {
  const GUI::Handle page = document();
  for (const Stand &stand : ::stood) {
    if (GUI::GET::clicked(page, ::caret(stand.id).c_str()))
      ::fold(stand.name, stand.standing);
    if (GUI::GET::clicked(page, ::armer(stand.id).c_str())) ::arm(stand.track);
  }
}

void SOUND::VIEWS::ARRANGEMENT::doors(const Vector<Stand> &stands) {
  ::settle(stands);
  if (VIEWS::reborn(::born)) ::stood.clear();
  if (stands.size() != ::stood.size()) ::build(stands);
  ::stood = stands;
  ::carets(stands);
  ::arms(stands);
}
