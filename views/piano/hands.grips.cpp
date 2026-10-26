// SPDX-License-Identifier: AGPL-3.0-or-later
#include <algorithm>

#include <island/gui/seat.hpp>

#include "../../boards.hpp"
#include "piano.hands.hpp"
#include "piano.notation.hpp"

namespace {
using namespace SOUND;

constexpr STRING::Hot STEM = "grip";
constexpr STRING::Hot MARKED = "selected";
constexpr Float WIDE = 10.0f;
constexpr Float SHARE = 3.0f;
constexpr Float RAISED = 1.0f;
constexpr Float ORIGIN = 0.0f;

struct Worn {
  Flag tonal = false;
  Float at = 0.0f, wide = 0.0f;
  auto operator==(const Worn &) const -> Bool = default;
};

Whole stood = 0;
Whole born = 0;
Vector<Worn> worn;

auto plate(Whole row) -> String {
  return GUI::SAC::SEAT::named(VIEWS::ROLL::board().c_str(), row);
}

auto gripped(Whole length) -> Float {
  return std::min(::WIDE, VIEWS::ROLL::wide(length) / ::SHARE);
}

void stand(GUI::Handle page, Whole row) {
  const String id = VIEWS::ROLL::HANDS::handle(row);
  BOARDS::place(page, ::plate(row).c_str(), "node", id, {}, {});
  GUI::set(page, id.c_str(), GUI::Style{::MARKED});
  GUI::set(page, id.c_str(), GUI::Depth{::RAISED});
  ::worn.push_back({});
}

void wear(GUI::Handle page, Whole row, const Worn &now) {
  const String id = VIEWS::ROLL::HANDS::handle(row);
  GUI::set(page, id.c_str(), GUI::Visibility{now.tonal});
  GUI::set(page, id.c_str(), GUI::Position{now.at, ::ORIGIN});
  GUI::set(page, id.c_str(), GUI::Extent{now.wide, VIEWS::ROLL::TALL});
}

}  // namespace

auto SOUND::VIEWS::ROLL::HANDS::handle(Whole row) -> String {
  return ::plate(row) + "." + ::STEM;
}

auto SOUND::VIEWS::ROLL::HANDS::resting(Whole length) -> Float {
  return wide(length) - ::gripped(length);
}

void SOUND::VIEWS::ROLL::HANDS::grip(
  GUI::Handle page, const Vector<Pip> &drawn) {
  if (VIEWS::reborn(::born)) ::stood = 0;
  ::stood = std::min<Whole>(::stood, drawn.size());
  ::worn.resize(::stood);
  for (; ::stood < drawn.size(); ++::stood) ::stand(page, ::stood);
  const Flag tonal = GET::notation() == TONAL;
  for (Whole row = 0; row < drawn.size(); ++row) {
    const Worn now = {
      tonal, resting(drawn[row].length), ::gripped(drawn[row].length)};
    if (now == ::worn[row]) continue;
    ::wear(page, row, now);
    ::worn[row] = now;
  }
}

void SOUND::VIEWS::ROLL::SHED::hands() {
  const GUI::Handle page = document();
  if (VIEWS::reborn(::born)) ::stood = 0;
  for (Whole row = 0; row < ::stood; ++row)
    BOARDS::drop(page, HANDS::handle(row));
  ::stood = 0;
}
