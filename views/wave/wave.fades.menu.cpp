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
constexpr Float TENTH = 0.1f;

WAVE::Asked about;

auto aimed(const String &target) -> WAVE::Asked {
  const Whole count = WAVE::placed().size();
  for (Whole row = 0; row < count; ++row)
    for (Whole end = 0; end < ::ENDS; ++end)
      if (WAVE::grip(row, end) == target) return {row, end};
  return {};
}

void laid(WAVE::Clip &clip, const FADE::End &end, Whole taken) {
  FADE::Fade &fade = clip.*end.fade;
  if (taken >= ::SHAPES) {
    fade = {};
    return;
  }
  if (fade.frames == 0) {
    const Integer tenth = Integer(Float(clip.frames) * ::TENTH);
    fade.frames = FADE::held(tenth, FADE::room(clip, end));
  }
  fade.shape = taken;
}

}  // namespace

auto SOUND::VIEWS::WAVE::worded() -> Vector<String> {
  Vector<String> said;
  for (const FADE::Shape &shape : FADE::SHAPES) said.push_back(shape.word);
  said.push_back(FADE::OFF);
  return said;
}

auto SOUND::VIEWS::WAVE::shapes(const Asked &asked) -> Whole {
  const Vector<Clip> seated = placed();
  if (asked.row >= seated.size() || asked.end >= ::ENDS) return ::SHAPES;
  const FADE::Fade &fade = seated[asked.row].*FADE::ENDS[asked.end].fade;
  return fade.frames == 0 ? ::SHAPES : fade.shape;
}

void SOUND::VIEWS::WAVE::shaped(const Asked &asked, Whole taken) {
  const Vector<Clip> seated = placed();
  if (asked.row >= seated.size() || asked.end >= ::ENDS) return;
  Clip now = seated[asked.row];
  ::laid(now, FADE::ENDS[asked.end], taken);
  retouch(asked.row, seated[asked.row], now, FADE::WORD);
}

void SOUND::VIEWS::WAVE::menu() {
  const GUI::Handle page = document();
  if (::about.row != NONE) {
    const Asked asked = ::about;
    const Whole taken = VIEWS::MENU::taken(GRIP);
    if (taken != NONE) {
      ::about = {};
      return shaped(asked, taken);
    }
    if (!VIEWS::MENU::standing()) ::about = {};
  }
  const GUI::NGA::Ask ask = GUI::NGA::GET::asked(page);
  if (ask.board.empty() || ask.board != board()) return;
  const Asked asked = ::aimed(String(ask.target));
  if (asked.row == NONE) return;
  ::about = asked;
  VIEWS::MENU::raise(worded(), ask.x, ask.y, GRIP, shapes(asked));
}
