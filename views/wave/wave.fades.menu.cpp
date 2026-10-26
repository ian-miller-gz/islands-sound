// SPDX-License-Identifier: AGPL-3.0-or-later
#include <iterator>

#include <island/gui/nga.hpp>

#include "../../fade.hpp"
#include "wave.internal.hpp"

namespace {
using namespace SOUND;
namespace WAVE = SOUND::VIEWS::WAVE;

constexpr Whole ENDS = std::size(FADE::ENDS);
constexpr Whole SHAPES = std::size(FADE::SHAPES);

struct Asked {
  Whole row = NONE, end = NONE;
};

Asked about;

auto worded() -> Vector<String> {
  Vector<String> said;
  for (const FADE::Shape &shape : FADE::SHAPES) said.push_back(shape.word);
  said.push_back(FADE::OFF);
  return said;
}

auto aimed(const String &target) -> ::Asked {
  const Whole count = WAVE::placed().size();
  for (Whole row = 0; row < count; ++row)
    for (Whole end = 0; end < ::ENDS; ++end)
      if (WAVE::grip(row, end) == target) return {row, end};
  return {};
}

auto ticked(const ::Asked &asked) -> Whole {
  const FADE::Fade &fade =
    WAVE::placed()[asked.row].*FADE::ENDS[asked.end].fade;
  return fade.frames == 0 ? ::SHAPES : fade.shape;
}

void took(const ::Asked &asked, Whole taken) {
  const Vector<WAVE::Clip> seated = WAVE::placed();
  if (asked.row >= seated.size()) return;
  WAVE::Clip now = seated[asked.row];
  FADE::Fade &fade = now.*FADE::ENDS[asked.end].fade;
  if (taken < ::SHAPES)
    fade.shape = taken;
  else
    fade = {};
  WAVE::retouch(asked.row, seated[asked.row], now, FADE::WORD);
}

}  // namespace

void SOUND::VIEWS::WAVE::menu() {
  const GUI::Handle page = document();
  if (::about.row != NONE) {
    const ::Asked asked = ::about;
    const Whole taken = VIEWS::MENU::taken(GRIP);
    if (taken != NONE) {
      ::about = {};
      return ::took(asked, taken);
    }
    if (!VIEWS::MENU::standing()) ::about = {};
  }
  const GUI::NGA::Ask ask = GUI::NGA::GET::asked(page);
  if (ask.board.empty() || ask.board != board()) return;
  const ::Asked asked = ::aimed(String(ask.target));
  if (asked.row == NONE) return;
  ::about = asked;
  VIEWS::MENU::raise(::worded(), ask.x, ask.y, GRIP, ::ticked(asked));
}
