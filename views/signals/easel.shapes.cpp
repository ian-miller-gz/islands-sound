// SPDX-License-Identifier: AGPL-3.0-or-later
#include "../../shape.hpp"
#include "signals.internal.hpp"

namespace {
using namespace SOUND;

constexpr STRING::Hot LINED = "signals.linear";
constexpr STRING::Hot SWELLED = "signals.exponential";
constexpr STRING::Hot FLATTENED = "signals.logarithmic";
constexpr STRING::Hot STEPPED = "signals.step";

const Vector<VIEWS::SIGNALS::Shape> family = {
  {::LINED, SHAPE::LINE},
  {::SWELLED, SHAPE::RISE},
  {::FLATTENED, SHAPE::FALL},
  {::STEPPED, SHAPE::HOLD}};

}  // namespace

auto SOUND::VIEWS::SIGNALS::shapes() -> const Vector<Shape>& {
  return ::family;
}
