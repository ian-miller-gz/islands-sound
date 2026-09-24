// SPDX-License-Identifier: AGPL-3.0-or-later
#include <cstdlib>

#include "../commands.hpp"
#include "../transport.hpp"
#include "views.internal.hpp"
#include "views.marks.hpp"

namespace {
using namespace SOUND;

constexpr STRING::Hot TAGS = "tag here";
constexpr STRING::Hot LOOPS = "loop here";
constexpr STRING::Hot OPENS = "start here";
constexpr STRING::Hot CLOSES = "end here";
constexpr STRING::Hot KEYS = "key change here";
constexpr STRING::Hot WHOSE = "marks";
constexpr STRING::Hot FIGURES = "0123456789";
constexpr STRING::Hot BLANK = "";
constexpr Whole TAGGED = 0;
constexpr Whole LOOPED = 1;
constexpr Whole OPENED = 2;
constexpr Whole CLOSED = 3;

Span bar;

void opened(const Span &at) {
  const Span ends = TRANSPORT::held().ends;
  COMMANDS::ended(at.from, ends.to > at.from ? ends.to : at.to);
}

void closed(const Span &at) {
  const Span ends = TRANSPORT::held().ends;
  COMMANDS::ended(ends.from < at.to ? ends.from : at.from, at.to);
}

void took(Whole row) {
  const Span at = ::bar;
  ::bar = {};
  if (row == ::TAGGED) return void(COMMANDS::flagged(at.from, String(::BLANK)));
  if (row == ::LOOPED) return void(COMMANDS::looped(at.from, at.to, String()));
  if (row == ::OPENED) return ::opened(at);
  if (row == ::CLOSED) return ::closed(at);
  const Key standing = TRANSPORT::keyed(at.from);
  COMMANDS::keyed(at.from, standing.tonic, standing.scale);
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

}  // namespace

void SOUND::VIEWS::asked(
  const String &strip, const Vector<GUI::SAC::LADDER::Mark> &dressed,
  Whole span, Pulsed pulsed) {
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
  MENU::raise(
    {String(::TAGS), String(::LOOPS), String(::OPENS), String(::CLOSES),
     String(::KEYS)},
    ask.x, ask.y, ::WHOSE);
}
