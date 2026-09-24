// SPDX-License-Identifier: AGPL-3.0-or-later
#include <algorithm>
#include <format>

#include <island/gui/nga.hpp>

#include "../../boards.hpp"
#include "../../inventory.hpp"
#include "../../score.hpp"
#include "../../transport.hpp"
#include "piano.internal.hpp"

namespace {
using namespace SOUND;

constexpr STRING::Hot STEM = "rail";
constexpr STRING::Hot LABELED = "label";
constexpr STRING::Hot INK = "muted";
constexpr Float LETTER = 12.0f;
constexpr Float INSET = 6.0f;
constexpr Float PAD = 1.0f;
constexpr Float RAISED = 2.0f;
constexpr Float ORIGIN = 0.0f;
constexpr STRING::Hot ADRIFT = "no notes lane on the chosen track";
constexpr STRING::Hot WROTE = "write";

Whole stood = 0;
Whole born = 0;
Vector<Key> worn;
Vector<Float> stands;

auto key(Whole section, Whole pitch) -> String {
  return VIEWS::ROLL::sectioned(::STEM, section, pitch);
}

auto cell(const String &id) -> String { return id + "." + ::LABELED; }

void said(GUI::Handle page, STRING::Hot text) {
  GUI::set(page, VIEWS::ROLL::NOTICE, GUI::Text{text});
}

auto lettered(Whole pitch, const Key &key) -> String {
  const String name = VIEWS::ROLL::named(pitch);
  const Whole degree = SCORE::degree(key, pitch);
  if (degree == NONE) return name;
  return name.empty() ? std::format("{}", degree)
                      : std::format("{} {}", degree, name);
}

void redress(GUI::Handle page, Whole section, const Key &key) {
  for (Whole pitch = 0; pitch <= VIEWS::ROLL::HIGHEST; ++pitch) {
    const String id = ::key(section, pitch);
    GUI::set(
      page, id.c_str(), GUI::Style{VIEWS::ROLL::dressed(pitch, key).c_str()});
    GUI::set(page, ::cell(id).c_str(), GUI::Text{::lettered(pitch, key)});
  }
}

void seat(GUI::Handle page, Whole section) {
  for (Whole pitch = 0; pitch <= VIEWS::ROLL::HIGHEST; ++pitch) {
    const String id = ::key(section, pitch);
    BOARDS::place(
      page, VIEWS::ROLL::board().c_str(), "button", id,
      {::ORIGIN, VIEWS::ROLL::lane(pitch)},
      {VIEWS::ROLL::RAILED, VIEWS::ROLL::STEP});
    GUI::set(page, id.c_str(), GUI::Depth{::RAISED});
    GUI::NGA::set(page, id.c_str(), GUI::NGA::Pinned{GUI::NGA::Pinned::ACROSS});
    BOARDS::place(
      page, id.c_str(), "label", ::cell(id), {::INSET, ::PAD},
      {VIEWS::ROLL::RAILED, ::LETTER});
    GUI::set(page, ::cell(id).c_str(), GUI::Style{::INK});
    GUI::set(page, ::cell(id).c_str(), GUI::Size{::LETTER});
  }
}

void strike(GUI::Handle page, Whole section) {
  for (Whole pitch = 0; pitch <= VIEWS::ROLL::HIGHEST; ++pitch) {
    const String id = ::key(section, pitch);
    BOARDS::drop(page, ::cell(id));
    BOARDS::drop(page, id);
  }
}

void relaid(GUI::Handle page, const Vector<VIEWS::ROLL::Keyed> &sections) {
  for (Whole section = sections.size(); section < ::stood; ++section)
    ::strike(page, section);
  for (Whole section = ::stood; section < sections.size(); ++section)
    ::seat(page, section);
  for (Whole section = 0; section < sections.size(); ++section)
    ::redress(page, section, sections[section].key);
  ::stood = sections.size();
}

auto placed(
  const Vector<VIEWS::ROLL::Keyed> &sections, Whole section, Float pan,
  Float zoom) -> Float {
  const Float natural = (sections[section].from - pan) * zoom;
  Float at = section == 0 ? ::ORIGIN : std::max(::ORIGIN, natural);
  if (section + 1 < sections.size())
    at = std::min(
      at, (sections[section + 1].from - pan) * zoom - VIEWS::ROLL::RAILED);
  return at;
}

void laid(GUI::Handle page, const Vector<VIEWS::ROLL::Keyed> &sections) {
  const Float pan = GUI::NGA::GET::pan(page, VIEWS::ROLL::board().c_str()).x;
  const Float zoom =
    GUI::NGA::GET::zoom(page, VIEWS::ROLL::board().c_str()).value;
  ::stands.resize(sections.size(), -VIEWS::ROLL::RAILED - 1.0f);
  for (Whole section = 0; section < sections.size(); ++section) {
    const Float at = ::placed(sections, section, pan, zoom);
    if (at == ::stands[section]) continue;
    ::stands[section] = at;
    const Flag shown = at > -VIEWS::ROLL::RAILED;
    for (Whole pitch = 0; pitch <= VIEWS::ROLL::HIGHEST; ++pitch) {
      const String id = ::key(section, pitch);
      GUI::set(page, id.c_str(), GUI::Visibility{shown});
      GUI::set(page, id.c_str(), GUI::Position{at, VIEWS::ROLL::lane(pitch)});
    }
  }
}

auto snapped(Whole snap) -> Whole {
  return VIEWS::ROLL::laid(VIEWS::ROLL::marked(), snap);
}

void wrote(GUI::Handle page, Whole pitch) {
  const VIEWS::ROLL::Bench bench = VIEWS::ROLL::bench();
  const Whole pulse = ::snapped(bench.snap);
  const VIEWS::ROLL::Aim aim = VIEWS::ROLL::claimed(pulse);
  if (aim.stock == NONE) return ::said(page, ADRIFT);
  Score score = INVENTORY::held().stocks[aim.stock].score;
  SCORE::write(
    score, {.pitch = pitch,
            .at = VIEWS::ROLL::inside(aim, pulse),
            .length = bench.length * bench.snap,
            .velocity = bench.loud / VIEWS::ROLL::FULL});
  HISTORY::take(aim.stock, INVENTORY::held().stocks[aim.stock]);
  INVENTORY::score(aim.stock, score);
  HISTORY::wrote(::WROTE, aim.stock, INVENTORY::held().stocks[aim.stock]);
  ::said(page, "");
}

void pressed(GUI::Handle page) {
  for (Whole section = 0; section < ::stood; ++section)
    for (Whole pitch = 0; pitch <= VIEWS::ROLL::HIGHEST; ++pitch)
      if (GUI::GET::clicked(page, ::key(section, pitch).c_str()))
        return ::wrote(page, pitch);
}

}  // namespace

void SOUND::VIEWS::ROLL::SHED::rail() {
  const GUI::Handle page = document();
  if (VIEWS::reborn(::born)) {
    ::stood = 0;
    ::worn.clear();
    return;
  }
  ::relaid(page, {});
  ::stands.clear();
}

void SOUND::VIEWS::ROLL::rail() {
  const GUI::Handle page = document();
  if (VIEWS::reborn(::born)) {
    ::stood = 0;
    ::worn.clear();
  }
  const Vector<Keyed> sections = keyed(::ORIGIN);
  if (::stood == 0 || rekeyed(::worn)) {
    ::relaid(page, sections);
    ::stands.clear();
  }
  ::laid(page, sections);
  ::pressed(page);
}

void SOUND::VIEWS::ROLL::rail(SHELL::Session &session) {
  session.print(std::format(
    "rail keys {} held west notice {}", ::stood == 0 ? 0 : HIGHEST + 1,
    GUI::GET::text(document(), NOTICE)));
  const GUI::Handle page = document();
  const Vector<Keyed> sections = keyed(::ORIGIN);
  const Float pan = GUI::NGA::GET::pan(page, board().c_str()).x;
  const Float zoom = GUI::NGA::GET::zoom(page, board().c_str()).value;
  for (Whole section = 1; section < sections.size(); ++section)
    session.print(std::format(
      "rail {} at {} stands {:g}", section, sections[section].key.at,
      ::placed(sections, section, pan, zoom)));
  aimed(session);
}
