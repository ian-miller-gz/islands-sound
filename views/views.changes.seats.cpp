// SPDX-License-Identifier: AGPL-3.0-or-later
#include <format>

#include "../transport.hpp"
#include "views.changes.internal.hpp"
#include "views.internal.hpp"

namespace {
using namespace SOUND;
namespace CHANGES = VIEWS::CHANGES;

constexpr STRING::Hot INK = "icon";
constexpr STRING::Hot LINE = "change {} {} at {} {}";
constexpr Float LETTER = 11.0f;
constexpr Float OVER = 2.0f;

VIEWS::Sided<Vector<CHANGES::Laid>> tapes;

auto paced(Whole at) -> Flag { return COMMANDS::paced(at); }
auto metred(Whole at) -> Flag { return COMMANDS::metred(at); }
auto keyed(Whole at) -> Flag { return COMMANDS::keyed(at); }

auto laid(const String &strip) -> CHANGES::Laid {
  return {
    strip,
    {"tempo", "", CHANGES::DIALLED, CHANGES::UPPER, 0.0f, ::paced,
     CHANGES::paced},
    {"metre", "", CHANGES::TYPED, CHANGES::LOWER, 0.0f, ::metred,
     CHANGES::metred},
    {"key", "", CHANGES::KEYED, CHANGES::LOWER,
     CHANGES::TYPED + CHANGES::INSET, ::keyed, CHANGES::keyed}};
}

template <typename Row>
void said(
  SHELL::Session &session, const String &strip, const CHANGES::Seats &seats,
  const Vector<Row> &rows) {
  const GUI::Handle page = VIEWS::document();
  for (Whole at = 1; at < rows.size() && at < seats.stood; ++at)
    session.print(std::format(
      ::LINE, seats.stem, at, rows[at].at,
      GUI::GET::text(page, CHANGES::mark(strip, seats, at).c_str())));
}

}  // namespace

auto SOUND::VIEWS::CHANGES::seated(const String &strip) -> Laid & {
  for (Laid &one : ::tapes())
    if (one.strip == strip) return one;
  ::tapes().push_back(::laid(strip));
  return ::tapes().back();
}

auto SOUND::VIEWS::CHANGES::mark(
  const String &strip, const Seats &seats, Whole at) -> String {
  return std::format("{}.{}.{}", strip, seats.stem, at);
}

void SOUND::VIEWS::CHANGES::build(
  GUI::Handle page, const String &strip, Seats &seats, Whole count) {
  const GUI::Extent size{seats.wide, VIEWS::BAND + INSET};
  for (Whole at = count; at < seats.stood; ++at)
    GUI::NODES::remove(page, mark(strip, seats, at).c_str());
  for (Whole at = seats.stood; at < count; ++at) {
    const String id = mark(strip, seats, at);
    GUI::NODES::create(page, strip.c_str(), seats.widget, id.c_str());
    GUI::set(page, id.c_str(), GUI::Style{::INK});
    GUI::set(page, id.c_str(), GUI::Size{::LETTER});
    GUI::set(page, id.c_str(), GUI::Depth{::OVER});
    GUI::set(page, id.c_str(), size);
  }
  seats.stood = count;
}

void SOUND::VIEWS::CHANGES::shed(const Tape &tape) {
  const GUI::Handle page = document();
  Laid &seats = seated(tape.strip);
  if (VIEWS::reborn(seats.born))
    seats.tempos.stood = seats.metres.stood = seats.keys.stood = 0;
  for (Seats *each : {&seats.tempos, &seats.metres, &seats.keys})
    build(page, tape.strip, *each, 0);
  handled(tape, {});
}

void SOUND::VIEWS::CHANGES::state(SHELL::Session &session, const Tape &tape) {
  const Laid &seats = seated(tape.strip);
  const Setting &setting = TRANSPORT::held();
  ::said(session, tape.strip, seats.tempos, setting.tempos);
  ::said(session, tape.strip, seats.metres, setting.metres);
  ::said(session, tape.strip, seats.keys, setting.keys);
}
