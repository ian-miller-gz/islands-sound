// SPDX-License-Identifier: AGPL-3.0-or-later
#include <format>

#include "../lane/lane.face.hpp"
#include "../../kind.hpp"
#include "wave.placed.hpp"
#include "wave.rows.hpp"

namespace {
using namespace SOUND;
namespace WAVE = SOUND::VIEWS::WAVE;
namespace RIG = SOUND::VIEWS::RIG;

constexpr STRING::Hot LINE = "stretch {} {}";
constexpr STRING::Hot SHIFTED = "shift {} pitch={} cents={}";
constexpr Whole ROW = 0, PITCH = 1, CENTS = 2;
constexpr STRING::Hot HIDDEN[] = {"head", RIG::STEPPED,  RIG::BOUND,
                                  "mark", RIG::RESTORED, RIG::HINTED};
static_assert(SOUND::KIND::named(HIDDEN));

auto sources() -> Vector<RIG::Source> {
  Vector<RIG::Source> all = {WAVE::STRETCH::source(), WAVE::SHIFT::pitch(),
                             WAVE::SHIFT::cents(),    WAVE::ONSETS::flag(),
                             WAVE::ONSETS::sense(),   WAVE::NOTES::flag(),
                             WAVE::TUNE::source()};
  for (const RIG::Source &source : WAVE::FADES::sources())
    all.push_back(source);
  return all;
}

auto cell(Whole row) -> String { return std::format("{}.{}", WAVE::ROWS, row); }

auto text(GUI::Handle page, Whole row) -> String {
  const String dial = std::format("{}.{}", ::cell(row), RIG::DIALLED);
  return GUI::GET::text(page, dial.c_str());
}

void turned(
  GUI::Handle page, Whole row, const RIG::Source &source, Float column) {
  const String cell = ::cell(row);
  if (!GUI::GET::visibility(page, cell.c_str())) return;
  for (STRING::Hot part : ::HIDDEN)
    GUI::set(
      page, std::format("{}.{}", cell, part).c_str(), GUI::Visibility{false});
  RIG::turn(page, cell, source, column);
}

void turned(GUI::Handle page, const Vector<RIG::Source> &sources) {
  Vector<String> names;
  for (const RIG::Source &source : sources) names.push_back(source.name);
  const Float column = RIG::column(page, WAVE::ROWS, names);
  for (Whole row = 0; row < sources.size(); ++row)
    ::turned(page, row, sources[row], column);
}

}  // namespace

void SOUND::VIEWS::WAVE::rows() {
  const GUI::Handle page = document();
  const Flag shown = chosen() != NONE;
  const Vector<RIG::Source> sources = ::sources();
  const Whole count = Whole(shown) * sources.size();
  GUI::set(page, WAVE::ROWS, GUI::Rows{count});
  fit(page, count);
  if (shown) ::turned(page, sources);
  if (shown) FADES::doors(page, sources.size() - FADES::sources().size());
  STRETCH::commit();
  SHIFT::commit();
  TUNE::commit();
}

void SOUND::VIEWS::WAVE::rows(SHELL::Session &session) {
  const Whole row = chosen();
  if (row == NONE) return;
  const GUI::Handle page = document();
  session.print(std::format(::LINE, row, ::text(page, ::ROW)));
  session.print(
    std::format(::SHIFTED, row, ::text(page, ::PITCH), ::text(page, ::CENTS)));
}
