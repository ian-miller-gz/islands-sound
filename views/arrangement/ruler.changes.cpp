// SPDX-License-Identifier: AGPL-3.0-or-later
#include <format>

#include <island/gui/nga.hpp>

#include "../../transport.hpp"
#include "../views.internal.hpp"
#include "arrangement.internal.hpp"
#include "ruler.changes.hpp"

namespace {
using namespace SOUND;
namespace CHANGES = VIEWS::ARRANGEMENT::CHANGES;

constexpr STRING::Hot INK = "icon";
constexpr Float LETTER = 11.0f;
constexpr Float NORTH = 1.0f;
constexpr Float OVER = 2.0f;

struct Seats {
  STRING::Hot stem, widget;
  Float wide;
  auto (*erase)(Whole at) -> Flag;
  Whole stood = 0;
};

Seats tempos{"tempo", "dial", CHANGES::DIALLED, [](Whole at) {
               return COMMANDS::paced(at);
             }};
Seats metres{"metre", "field", CHANGES::TYPED, [](Whole at) {
               return COMMANDS::metred(at);
             }};
Whole born = 0;

auto mark(const Seats &seats, Whole at) -> String {
  return std::format("{}.{}.{}", VIEWS::ARRANGEMENT::strip(), seats.stem, at);
}

void build(GUI::Handle page, Seats &seats, Whole count) {
  const String strip = VIEWS::ARRANGEMENT::strip();
  const GUI::Extent size{seats.wide, VIEWS::BAND + CHANGES::INSET};
  for (Whole at = count; at < seats.stood; ++at)
    GUI::NODES::remove(page, ::mark(seats, at).c_str());
  for (Whole at = seats.stood; at < count; ++at) {
    const String id = ::mark(seats, at);
    GUI::NODES::create(page, strip.c_str(), seats.widget, id.c_str());
    GUI::set(page, id.c_str(), GUI::Style{::INK});
    GUI::set(page, id.c_str(), GUI::Size{::LETTER});
    GUI::set(page, id.c_str(), GUI::Depth{::OVER});
    GUI::set(page, id.c_str(), size);
  }
  seats.stood = count;
}

auto place(GUI::Handle page, Whole pulses) -> Float {
  const String board = VIEWS::ARRANGEMENT::board();
  const Float scale = GUI::NGA::GET::zoom(page, board.c_str()).value;
  const Float west = GUI::NGA::GET::pan(page, board.c_str()).x;
  const Float across = VIEWS::ARRANGEMENT::across(VIEWS::framed(pulses));
  return (across - west) * scale + CHANGES::INSET;
}

template <typename Row>
void laid(
  GUI::Handle page, Seats &seats, const Vector<Row> rows,
  Vector<CHANGES::Mark> &marks) {
  if (rows.size() != seats.stood) ::build(page, seats, rows.size());
  const String editing = GUI::GET::editing(page);
  for (Whole at = 0; at < rows.size(); ++at) {
    const String id = ::mark(seats, at);
    const Float east = ::place(page, rows[at].at) + CHANGES::beside(rows[at]);
    GUI::set(page, id.c_str(), GUI::Position{east, ::NORTH});
    if (at > 0) marks.push_back({id, rows[at].at, seats.erase});
    if (CHANGES::taken(page, id, rows[at]) || id == editing) continue;
    CHANGES::dressed(page, id, rows[at]);
    GUI::set(page, id.c_str(), GUI::Text{CHANGES::worded(rows[at])});
  }
}

}  // namespace

void SOUND::VIEWS::ARRANGEMENT::changes() {
  const GUI::Handle page = document();
  if (VIEWS::reborn(::born)) ::tempos.stood = ::metres.stood = 0;
  Vector<CHANGES::Mark> marks;
  ::laid(page, ::tempos, TRANSPORT::held().tempos, marks);
  ::laid(page, ::metres, TRANSPORT::held().metres, marks);
  CHANGES::settled();
  CHANGES::seated(page, std::format("{}.{}.", strip(), ::metres.stem));
  CHANGES::asked(marks);
}
