// SPDX-License-Identifier: AGPL-3.0-or-later
#include "../transport.hpp"
#include "views.changes.internal.hpp"
#include "views.internal.hpp"

namespace {
using namespace SOUND;
namespace CHANGES = VIEWS::CHANGES;

constexpr Float OVER = 2.0f;
constexpr Float HEADED = 5.0f;

template <typename Row>
auto east(const CHANGES::Tape &tape, const CHANGES::Seats &seats, const Row &row)
  -> Float {
  if (row.at == 0 && tape.head > 0.0f) return CHANGES::INSET + seats.head;
  return tape.placed(VIEWS::framed(row.at)) + CHANGES::INSET +
         CHANGES::beside(row);
}

template <typename Row>
void placed(
  GUI::Handle page, const CHANGES::Tape &tape, CHANGES::Seats &seats,
  const Vector<Row> &rows, Vector<CHANGES::Mark> &marks) {
  if (rows.size() != seats.stood)
    CHANGES::build(page, tape.strip, seats, rows.size());
  const String editing = GUI::GET::editing(page);
  for (Whole at = 0; at < rows.size(); ++at) {
    const String id = CHANGES::mark(tape.strip, seats, at);
    const Flag founding = at == 0;
    GUI::set(page, id.c_str(), GUI::Visibility{!founding || tape.head > 0.0f});
    GUI::set(
      page, id.c_str(), GUI::Position{::east(tape, seats, rows[at]), seats.down});
    GUI::set(page, id.c_str(), GUI::Depth{founding ? ::HEADED : ::OVER});
    if (!founding)
      marks.push_back(
        {id, rows[at].at, CHANGES::INSET + CHANGES::beside(rows[at]),
         seats.erase, seats.move});
    if (CHANGES::taken(page, id, rows[at]) || id == editing) continue;
    CHANGES::dressed(page, id, rows[at]);
    GUI::set(page, id.c_str(), GUI::Text{CHANGES::worded(rows[at])});
  }
}

}  // namespace

void SOUND::VIEWS::CHANGES::lay(const Tape &tape) {
  const GUI::Handle page = document();
  Laid &seats = seated(tape.strip);
  if (VIEWS::reborn(seats.born))
    seats.tempos.stood = seats.metres.stood = seats.keys.stood = 0;
  Vector<Mark> marks;
  ::placed(page, tape, seats.tempos, TRANSPORT::held().tempos, marks);
  ::placed(page, tape, seats.metres, TRANSPORT::held().metres, marks);
  ::placed(page, tape, seats.keys, TRANSPORT::held().keys, marks);
  settled();
  seated(page, tape.strip);
  asked(marks);
  handled(tape, marks);
}
