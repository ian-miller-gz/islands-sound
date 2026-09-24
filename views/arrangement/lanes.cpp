// SPDX-License-Identifier: AGPL-3.0-or-later
#include <algorithm>
#include <format>

#include <island/gui/nga.hpp>
#include <island/gui/rows.hpp>

#include "../../boards.hpp"
#include "../../kind.hpp"
#include "../../timeline.hpp"
#include "lanes.internal.hpp"

namespace {
using namespace SOUND;

constexpr STRING::Hot STEM = "band";
constexpr STRING::Hot CARRY = "carry";
constexpr STRING::Hot WASHED = "wash";
constexpr STRING::Hot CABLED = "cable";
constexpr STRING::Hot CURBED = "curbhead";
constexpr STRING::Hot SILLED = "curbfoot";

constexpr Float HEAD = 0.0f;

constexpr Float WEST = 0.0f;

Vector<VIEWS::ARRANGEMENT::Stand> stood;
Vector<GUI::SAC::ROWS::Rail> seating;
Whole born = 0;

Vector<Float> lifts;

auto band(Whole at) -> String {
  return std::format("{}.{}.{}", VIEWS::ARRANGEMENT::board(), ::STEM, at);
}

auto curb(const String &id) -> String { return id + "." + ::CURBED; }

auto sill(const String &id) -> String { return id + "." + ::SILLED; }

auto deep(const VIEWS::ARRANGEMENT::Stand &stand) -> Float {
  if (!stand.laden) return VIEWS::ARRANGEMENT::CABLE;
  return VIEWS::ARRANGEMENT::shut(stand.name, stand.standing)
           ? VIEWS::ARRANGEMENT::SHUT
           : VIEWS::ARRANGEMENT::OPEN;
}

auto carried(const VIEWS::ARRANGEMENT::Stand &stand) -> String {
  return String(::CARRY) + stand.kind;
}

auto dressed(Flag laden, STRING::Hot kind) -> String {
  return (laden ? String(::WASHED) : String(::CABLED)) + kind;
}

void opened(Vector<GUI::SAC::ROWS::Row> &rows) {
  const VIEWS::ARRANGEMENT::Draft drawing = VIEWS::ARRANGEMENT::drafting();
  if (::lifts.size() != rows.size()) ::lifts.assign(rows.size(), 0.0f);
  for (Whole row = 0; row < rows.size(); ++row) {
    const Flag under = drawing.track != NONE && drawing.band == row;
    const Float wanted =
      under ? std::max(0.0f, VIEWS::ARRANGEMENT::OPEN - rows[row].depth) : 0.0f;
    ::lifts[row] = VIEWS::eased(::lifts[row], wanted);
    rows[row].depth += ::lifts[row];
  }
}

auto stands(Vector<GUI::SAC::ROWS::Row> &rows)
  -> Vector<VIEWS::ARRANGEMENT::Stand> {
  Vector<VIEWS::ARRANGEMENT::Stand> wanted;
  const Vector<ARRANGEMENT::Track> &tracks = TIMELINE::held().tracks;
  for (Whole track = 0; track < tracks.size(); ++track) {
    const Vector<ARRANGEMENT::TRACK::Lane> &lanes = tracks[track].lanes;
    for (Whole lane = 0; lane < lanes.size(); ++lane) {
      const Flag laden = !lanes[lane].clips.empty();
      const STRING::Hot kind = KIND::spoken(lanes[lane].kind);
      wanted.push_back(
        {track, lane, ::band(wanted.size()), tracks[track].name,
         lanes[lane].name, kind, ::dressed(laden, kind), laden,
         lanes[lane].kind == KIND::AUDIO, !lanes[lane].outtakes.empty()});
      rows.push_back(
        {::deep(wanted.back()) +
           (wanted.back().shelved ? VIEWS::ARRANGEMENT::SHELF : 0.0f),
         lane == 0});
    }
  }
  return wanted;
}

void build(Whole count) {
  const GUI::Handle page = VIEWS::document();
  for (Whole row = count; row < ::stood.size(); ++row)
    GUI::NODES::remove(page, ::stood[row].id.c_str());
  for (Whole row = ::stood.size(); row < count; ++row) {
    const String id = ::band(row);
    BOARDS::place(
      page, VIEWS::ARRANGEMENT::board().c_str(), "panel", id, {}, {});
    BOARDS::place(page, id.c_str(), "panel", ::curb(id), {0.0f, ::HEAD}, {});
    BOARDS::place(page, id.c_str(), "panel", ::sill(id), {0.0f, ::HEAD}, {});
  }
}

void grounds() {
  const GUI::Handle page = VIEWS::document();
  const Float east = GUI::SAC::ROWS::reach(
    GUI::NGA::GET::pan(page, VIEWS::ARRANGEMENT::board().c_str()).x,
    VIEWS::ARRANGEMENT::window());
  for (Whole row = 0; row < ::stood.size(); ++row) {
    const String &id = ::stood[row].id;
    const String head = ::curb(id), foot = ::sill(id);
    GUI::set(page, id.c_str(), GUI::Position{::WEST, ::seating[row].top});
    GUI::set(page, id.c_str(), GUI::Extent{east, ::seating[row].depth});
    GUI::set(page, id.c_str(), GUI::Style{::stood[row].dress.c_str()});
    const Float curbed = ::stood[row].laden ? VIEWS::ARRANGEMENT::CURB : 0.0f;
    const String worn = ::carried(::stood[row]);
    GUI::set(page, head.c_str(), GUI::Extent{east, curbed});
    GUI::set(page, head.c_str(), GUI::Style{worn.c_str()});
    GUI::set(
      page, foot.c_str(), GUI::Position{0.0f, ::seating[row].depth - curbed});
    GUI::set(page, foot.c_str(), GUI::Extent{east, curbed});
    GUI::set(page, foot.c_str(), GUI::Style{worn.c_str()});
  }
}

void titled() {
  Float lead = 0.0f;
  for (Whole row = 0; row < ::seating.size(); ++row) {
    if (row == 0 || ::stood[row].track != ::stood[row - 1].track)
      lead += VIEWS::ARRANGEMENT::TITLE;
    ::seating[row].top += lead;
  }
}

}  // namespace

auto SOUND::VIEWS::ARRANGEMENT::deep() -> Float {
  return GUI::SAC::ROWS::deep(::seating, MARGIN);
}

auto SOUND::VIEWS::ARRANGEMENT::row(Float down) -> Row {
  const Whole band = GUI::SAC::ROWS::at(::seating, down);
  if (band == GUI::SAC::ROWS::NONE || band >= ::stood.size()) return {};
  return {::stood[band].track, ::stood[band].lane, band};
}

auto SOUND::VIEWS::ARRANGEMENT::shut(Whole band) -> Flag {
  return band < ::stood.size() &&
         shut(::stood[band].name, ::stood[band].standing);
}

auto SOUND::VIEWS::ARRANGEMENT::rail(Whole band) -> Float {
  return band < ::seating.size() ? ::seating[band].top : MARGIN;
}

auto SOUND::VIEWS::ARRANGEMENT::depth(Whole band) -> Float {
  return band < ::seating.size() ? ::seating[band].depth : CABLE;
}

auto SOUND::VIEWS::ARRANGEMENT::sketched(Whole band) -> Flag {
  return depth(band) >= OPEN;
}

auto SOUND::VIEWS::ARRANGEMENT::banded(Whole track, Whole lane) -> Whole {
  for (Whole row = 0; row < ::stood.size(); ++row)
    if (::stood[row].track == track && ::stood[row].lane == lane) return row;
  return NONE;
}

auto SOUND::VIEWS::ARRANGEMENT::inlay(Whole band) -> Float {
  return rail(band) + CURB + INLAY;
}

auto SOUND::VIEWS::ARRANGEMENT::inlaid(Whole band) -> Float {
  const Float run = depth(band) - shelf(band) - (CURB + INLAY) * 2.0f;
  return run > 0.0f ? run : depth(band);
}

auto SOUND::VIEWS::ARRANGEMENT::shelf(Whole band) -> Float {
  return band < ::stood.size() && ::stood[band].shelved ? SHELF : 0.0f;
}

auto SOUND::VIEWS::ARRANGEMENT::shelved(Float down) -> Flag {
  const Whole band = GUI::SAC::ROWS::at(::seating, down);
  if (band == GUI::SAC::ROWS::NONE || band >= ::stood.size()) return false;
  return down >= ::seating[band].top + ::seating[band].depth - shelf(band);
}

void SOUND::VIEWS::ARRANGEMENT::bands() {
  doors();
  Vector<GUI::SAC::ROWS::Row> rows;
  Vector<Stand> wanted = ::stands(rows);
  if (VIEWS::reborn(::born)) ::stood.clear();
  if (wanted.size() != ::stood.size()) ::build(wanted.size());
  ::stood = wanted;
  ::opened(rows);
  ::seating = GUI::SAC::ROWS::stack(rows, MARGIN, GAP);
  ::titled();
  ::grounds();
  cells(::stood);
  doors(::stood);
}

auto SOUND::VIEWS::ARRANGEMENT::ribbons() -> Vector<Ribbon> {
  Vector<Ribbon> runs;
  for (Whole row = 0; row < ::stood.size();) {
    Whole last = row;
    while (last + 1 < ::stood.size() &&
           ::stood[last + 1].track == ::stood[row].track)
      ++last;
    runs.push_back(
      {::seating[row].top - TITLE, ::seating[last].top + ::seating[last].depth,
       ::stood[row].name});
    row = last + 1;
  }
  return runs;
}

void SOUND::VIEWS::ARRANGEMENT::bands(SHELL::Session &session) {
  session.print(std::format("bands {} deep {}", ::stood.size(), deep()));
  for (Whole row = 0; row < ::stood.size(); ++row)
    session.print(std::format(
      "band {} track {} {} lane {} {} {} {} at {} deep {} wears {} curb {}",
      row, ::stood[row].track, ::stood[row].name, ::stood[row].lane,
      ::stood[row].standing, String(::stood[row].kind),
      !::stood[row].laden                              ? "cable"
      : shut(::stood[row].name, ::stood[row].standing) ? "shut"
                                                       : "open",
      ::seating[row].top, ::seating[row].depth, ::stood[row].dress,
      ::carried(::stood[row])));
}
