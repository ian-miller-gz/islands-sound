// SPDX-License-Identifier: AGPL-3.0-or-later
#pragma once
#include <iterator>

#include "piano.pips.hpp"
#include "piano.staff.hpp"

namespace SOUND::VIEWS::ROLL::STAFF {

constexpr Float WAYS[] = {-1.0f, 1.0f};
constexpr Whole DEPTH = std::rbegin(VALUES)->beams;

struct Slope {
  Whole at = 0;
  Integer quarters = 0;
};

struct Ends {
  GUI::Position near, far;
};

struct Rest {
  Whole at = 0, beams = 0;
  auto operator==(const Rest &) const -> Bool = default;
};

namespace GET {
auto slopes() -> const Vector<Slope> &;
}  // namespace GET

auto centred(Integer place) -> Float;
auto tipped(Integer place, Flag foot) -> Float;
auto anchored(const Stem &stem) -> Float;
auto ended(const Beam &beam, const Vector<Stem> &stems) -> Ends;
void cut(const String &id, const Box &box);
void cut(const String &id, const Ends &ends);

struct Scale {
  Float across = 0.0f, down = 0.0f;
};

auto scaled() -> Scale;
void flagged(const Vector<Beam> &beams);
void hang(const String &id, const Stem &stem, Scale scale);
void bow(const String &id, const Stem &stem, Whole rank, Scale scale);

void valued(Vector<Stem> &stems);

auto rested(const Vector<Stem> &stems) -> Vector<Rest>;
void rested(Flag moved);

namespace SHED {
void flagged();
void rested();
}  // namespace SHED

}  // namespace SOUND::VIEWS::ROLL::STAFF
