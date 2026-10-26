// SPDX-License-Identifier: AGPL-3.0-or-later
#include <cstdlib>
#include <iterator>

#include "../commands.hpp"
#include "../transport.hpp"
#include "views.internal.hpp"
#include "views.marks.hpp"

namespace {
using namespace SOUND;

constexpr STRING::Hot WHOSE = "marks";
constexpr STRING::Hot FIGURES = "0123456789";
constexpr STRING::Hot BLANK = "";

Span bar;
Whole offered = 0;

void tagged(const Span &at) { COMMANDS::flagged(at.from, String(::BLANK)); }
void looped(const Span &at) { COMMANDS::looped(at.from, at.to, String()); }

void opened(const Span &at) {
  const Span ends = TRANSPORT::held().ends;
  COMMANDS::ended(at.from, ends.to > at.from ? ends.to : at.to);
}

void closed(const Span &at) {
  const Span ends = TRANSPORT::held().ends;
  COMMANDS::ended(ends.from < at.to ? ends.from : at.from, at.to);
}

void keyed(const Span &at) {
  const Key standing = TRANSPORT::keyed(at.from);
  COMMANDS::keyed(at.from, standing.tonic, standing.scale);
}

void paced(const Span &at) {
  COMMANDS::paced(at.from, TRANSPORT::paced(at.from));
}

void metred(const Span &at) {
  const Metre standing = TRANSPORT::metred(at.from);
  COMMANDS::metred(at.from, standing.numerator, standing.denominator);
}

struct Row {
  STRING::Hot label;
  void (*act)(const Span &at);
};

constexpr Row ROWS[] = {{"tag here", ::tagged},   {"loop here", ::looped},
                        {"start here", ::opened}, {"end here", ::closed},
                        {"key here", ::keyed},    {"tempo here", ::paced},
                        {"metre here", ::metred}};

void took(Whole row) {
  if (row < ::offered) ::ROWS[row].act(::bar);
  ::bar = {};
}

auto marked(const String &strip, const GUI::NGA::Ask &ask) -> Whole {
  if (!ask.asked || !ask.board.empty()) return NONE;
  const String stem = strip + ".";
  if (ask.target.compare(0, stem.size(), stem) != 0) return NONE;
  const String tail = ask.target.substr(stem.size());
  const String word = tail.substr(0, tail.find('.'));
  if (word.empty() || word.find_first_not_of(::FIGURES) != String::npos)
    return NONE;
  return std::strtoul(word.c_str(), nullptr, 10);
}

auto worded(Whole rows) -> Vector<String> {
  Vector<String> labels;
  for (Whole row = 0; row < rows && row < std::size(::ROWS); ++row)
    labels.emplace_back(::ROWS[row].label);
  return labels;
}

}  // namespace

void SOUND::VIEWS::asked(
  const String &strip, const Vector<GUI::SAC::LADDER::Mark> &dressed,
  Whole span, Pulsed pulsed, Whole rows) {
  if (::bar.to > ::bar.from) {
    const Whole row = MENU::taken(::WHOSE);
    if (row != NONE) return ::took(row);
    if (!MENU::standing()) ::bar = {};
  }
  const GUI::NGA::Ask ask = GUI::NGA::GET::asked(document());
  const Whole mark = ::marked(strip, ask);
  if (mark == NONE || mark >= dressed.size()) return;
  const Integer at = dressed[mark].at;
  const Integer end = at + Integer(spanned(dressed, mark, span));
  if (end <= 0) return;
  ::bar = {pulsed(at < 0 ? 0 : Whole(at)), pulsed(Whole(end))};
  if (::bar.to <= ::bar.from) return void(::bar = {});
  ::offered = rows;
  MENU::raise(::worded(rows), ask.x, ask.y, ::WHOSE);
}
