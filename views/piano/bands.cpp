// SPDX-License-Identifier: AGPL-3.0-or-later
#include <format>

#include <island/gui/nga.hpp>
#include <island/gui/rows.hpp>

#include "../../boards.hpp"
#include "../../score.hpp"
#include "../../transport.hpp"
#include "piano.internal.hpp"

namespace {
using namespace SOUND;

constexpr STRING::Hot STEM = "key";
constexpr STRING::Hot WHITE = "white";
constexpr STRING::Hot BLACK = "black";
constexpr STRING::Hot OUT = "out";
constexpr STRING::Hot TONIC = "tonic";
constexpr Whole FIRST = 1;
constexpr Whole SHARPS[] = {1, 3, 6, 8, 10};
constexpr Integer BELOW = 1;
constexpr Float ORIGIN = 0.0f;
constexpr Float SPARE = 0.5f;

Whole stood = 0;
Whole born = 0;
Float reach = ::ORIGIN;
Vector<Key> worn;

auto standing(const Key &key) -> Flag {
  return SCORE::degree(key, key.tonic) != NONE;
}

auto dark(Whole pitch) -> Flag {
  for (Whole note : ::SHARPS)
    if (pitch % VIEWS::ROLL::SEMITONES == note) return true;
  return false;
}

void seat(GUI::Handle page, Whole section, const VIEWS::ROLL::Keyed &laid) {
  for (Whole pitch = 0; pitch <= VIEWS::ROLL::HIGHEST; ++pitch) {
    const String id = VIEWS::ROLL::sectioned(::STEM, section, pitch);
    BOARDS::place(
      page, VIEWS::ROLL::board().c_str(), "panel", id,
      {laid.from, VIEWS::ROLL::lane(pitch)},
      {laid.to - laid.from, VIEWS::ROLL::STEP});
    GUI::set(
      page, id.c_str(),
      GUI::Style{VIEWS::ROLL::dressed(pitch, laid.key).c_str()});
  }
}

void strike(GUI::Handle page, Whole section) {
  for (Whole pitch = 0; pitch <= VIEWS::ROLL::HIGHEST; ++pitch)
    BOARDS::drop(page, VIEWS::ROLL::sectioned(::STEM, section, pitch));
}

void relaid(GUI::Handle page, const Vector<VIEWS::ROLL::Keyed> &sections) {
  for (Whole section = sections.size(); section < ::stood; ++section)
    ::strike(page, section);
  for (Whole section = 0; section < sections.size(); ++section) {
    if (section >= ::stood || section == 0 && ::stood == 0) {
      ::seat(page, section, sections[section]);
      continue;
    }
    for (Whole pitch = 0; pitch <= VIEWS::ROLL::HIGHEST; ++pitch) {
      const String id = VIEWS::ROLL::sectioned(::STEM, section, pitch);
      GUI::set(
        page, id.c_str(),
        GUI::Position{sections[section].from, VIEWS::ROLL::lane(pitch)});
      GUI::set(
        page, id.c_str(),
        GUI::Extent{
          sections[section].to - sections[section].from, VIEWS::ROLL::STEP});
      GUI::set(
        page, id.c_str(),
        GUI::Style{VIEWS::ROLL::dressed(pitch, sections[section].key).c_str()});
    }
  }
  ::stood = sections.size();
}

}  // namespace

auto SOUND::VIEWS::ROLL::marked() -> Whole {
  return SCORE::pulsed(TRANSPORT::marker().position, TRANSPORT::held().tempos);
}

auto SOUND::VIEWS::ROLL::sectioned(STRING::Hot stem, Whole section, Whole pitch)
  -> String {
  if (section == 0) return std::format("{}.{}.{}", board(), stem, pitch);
  return std::format("{}.{}.{}.{}", board(), stem, section, pitch);
}

auto SOUND::VIEWS::ROLL::keyed(Float reach) -> Vector<Keyed> {
  const Vector<Key> &keys = TRANSPORT::held().keys;
  Vector<Keyed> sections;
  for (Whole row = 0; row < keys.size(); ++row)
    sections.push_back(
      {keys[row], across(keys[row].at),
       row + 1 < keys.size() ? across(keys[row + 1].at) : reach});
  if (sections.empty()) sections.push_back({Key{}, ::ORIGIN, reach});
  return sections;
}

auto SOUND::VIEWS::ROLL::dressed(Whole pitch, const Key &key) -> String {
  const Whole degree = SCORE::degree(key, pitch);
  if (degree == ::FIRST) return String(::STEM) + ::TONIC;
  const String face = String(::STEM) + (::dark(pitch) ? ::BLACK : ::WHITE);
  if (degree != NONE) return face;
  return ::standing(key) ? face + ::OUT : face;
}

auto SOUND::VIEWS::ROLL::rekeyed(Vector<Key> &held) -> Flag {
  const Vector<Key> &keys = TRANSPORT::held().keys;
  Flag same = keys.size() == held.size();
  for (Whole row = 0; same && row < keys.size(); ++row)
    same = keys[row].at == held[row].at && keys[row].tonic == held[row].tonic &&
           keys[row].scale == held[row].scale;
  if (same) return false;
  held = keys;
  return true;
}

auto SOUND::VIEWS::ROLL::degreed(Whole pitch, Whole at) -> Whole {
  const Whole landing = SCORE::snapped(TRANSPORT::keyed(at), pitch);
  return landing < HIGHEST ? landing : HIGHEST;
}

auto SOUND::VIEWS::ROLL::named(Whole pitch) -> String {
  if (pitch % SEMITONES != 0) return String();
  return std::format("C{}", Integer(pitch / SEMITONES) - ::BELOW);
}

void SOUND::VIEWS::ROLL::SHED::keys() {
  const GUI::Handle page = document();
  if (VIEWS::reborn(::born)) {
    ::stood = 0;
    ::reach = ::ORIGIN;
    ::worn.clear();
    return;
  }
  for (Whole section = 0; section < ::stood; ++section) ::strike(page, section);
  ::stood = 0;
}

void SOUND::VIEWS::ROLL::keys() {
  const GUI::Handle page = document();
  if (VIEWS::reborn(::born)) {
    ::stood = 0;
    ::reach = ::ORIGIN;
    ::worn.clear();
  }
  const Flag moved = rekeyed(::worn);
  const Float grew = GUI::SAC::ROWS::reach(
    ::reach, GUI::NGA::GET::pan(page, board().c_str()).x, window().w, ::SPARE);
  const Flag grown = grew > ::reach;
  ::reach = grew;
  const Vector<Keyed> sections = keyed(::reach);
  if (::stood == 0 || moved) return ::relaid(page, sections);
  if (!grown) return;
  const Keyed &last = sections.back();
  for (Whole pitch = 0; pitch <= HIGHEST; ++pitch)
    GUI::set(
      page, sectioned(::STEM, sections.size() - 1, pitch).c_str(),
      GUI::Extent{last.to - last.from, STEP});
}

void SOUND::VIEWS::ROLL::keys(SHELL::Session &session) {
  const Key key = TRANSPORT::keyed(marked());
  session.print(std::format(
    "keys {} named {} step {:g} key {}", HIGHEST + 1, HIGHEST / SEMITONES + 1,
    STEP,
    ::standing(key) ? std::format("{} {}", SCORE::classed(key.tonic), key.scale)
                    : String("off")));
  const Vector<Keyed> sections = keyed(::reach);
  for (Whole section = 1; section < sections.size(); ++section)
    session.print(std::format(
      "section {} at {} key {} from {:g} to {:g}", section,
      sections[section].key.at,
      ::standing(sections[section].key)
        ? std::format(
            "{} {}", SCORE::classed(sections[section].key.tonic),
            sections[section].key.scale)
        : String("off"),
      sections[section].from, sections[section].to));
}
