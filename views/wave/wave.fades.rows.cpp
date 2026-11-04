// SPDX-License-Identifier: AGPL-3.0-or-later
#include <format>
#include <iterator>

#include "../../fade.hpp"
#include "wave.internal.hpp"
#include "wave.rows.hpp"

namespace {
using namespace SOUND;
namespace WAVE = SOUND::VIEWS::WAVE;
namespace RIG = SOUND::VIEWS::RIG;

constexpr Whole SHAPES = std::size(FADE::SHAPES);
constexpr Whole ENDS = std::size(FADE::ENDS);
constexpr Whole IN = 0, OUT = 1;
constexpr Float GAP = 4.0f;
constexpr STRING::Hot NAMES[] = {"In", "Out"};
static_assert(std::size(NAMES) == ::ENDS);

WAVE::Asked about;

auto published() -> AUDIO::PLUGIN::Control {
  return {
    .steps = ::SHAPES,
    .labels = WAVE::worded(),
    .least = WAVE::ORIGIN,
    .most = Float(::SHAPES)};
}

auto asked(Whole end) -> WAVE::Asked { return {WAVE::chosen(), end}; }

template <Whole END>
auto read() -> Float {
  return Float(WAVE::shapes(::asked(END)));
}

template <Whole END>
void write(Float value) {
  const Whole step = RIG::stepped(::published(), value);
  if (step != WAVE::shapes(::asked(END))) WAVE::shaped(::asked(END), step);
}

template <Whole END>
auto source() -> RIG::Source {
  return {
    .name = ::NAMES[END],
    .published = ::published,
    .read = ::read<END>,
    .write = ::write<END>};
}

void took(Whole end, const String &cell) {
  const Whole taken = VIEWS::MENU::taken(cell.c_str());
  if (taken == NONE || ::about.end != end) return;
  const WAVE::Asked asked = ::about;
  ::about = {};
  WAVE::shaped(asked, taken);
}

void raised(GUI::Handle page, Whole end, const String &cell) {
  if (!GUI::GET::visibility(page, cell.c_str())) return;
  const String door = cell + "." + RIG::STEPPED;
  GUI::set(page, door.c_str(), GUI::Visibility{true});
  if (!GUI::GET::clicked(page, door.c_str())) return;
  ::about = ::asked(end);
  const GUI::Position at = GUI::GET::origin(page, door.c_str());
  const GUI::Extent size = GUI::GET::measured(page, door.c_str());
  VIEWS::MENU::raise(
    WAVE::worded(), at.x, at.y + size.h + ::GAP, cell.c_str(),
    WAVE::shapes(::about));
}

}  // namespace

auto SOUND::VIEWS::WAVE::FADES::sources() -> Vector<RIG::Source> {
  return {::source<::IN>(), ::source<::OUT>()};
}

void SOUND::VIEWS::WAVE::FADES::doors(GUI::Handle page, Whole first) {
  if (!VIEWS::MENU::standing()) ::about = {};
  for (Whole end = 0; end < ::ENDS; ++end) {
    const String cell = std::format("{}.{}", ROWS, first + end);
    ::took(end, cell);
    ::raised(page, end, cell);
  }
}
