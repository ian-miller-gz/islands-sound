// SPDX-License-Identifier: AGPL-3.0-or-later
#include <algorithm>

#include <island/gui/nga.hpp>
#include <island/gui/window.hpp>

#include "../../score.hpp"
#include "wave.internal.hpp"

namespace {
using namespace SOUND;
namespace WAVE = SOUND::VIEWS::WAVE;

constexpr Whole REACH = Whole(10) * 3600 * RATE;
constexpr Float DASH = 0.5f;

auto across(Whole frames) -> Float { return Float(frames); }

}  // namespace

auto SOUND::VIEWS::WAVE::scaled() -> Float {
  return GUI::NGA::GET::zoom(document(), board().c_str()).value;
}

auto SOUND::VIEWS::WAVE::window() -> Float {
  const GUI::Handle page = document();
  const String id = board();
  return GUI::SAC::WINDOW::seen(
           GUI::GET::measured(page, id.c_str()),
           GUI::NGA::GET::zoom(page, id.c_str()))
    .w;
}

auto SOUND::VIEWS::WAVE::deep() -> Float {
  return GUI::GET::measured(document(), board().c_str()).h;
}

auto SOUND::VIEWS::WAVE::least() -> Float {
  const GUI::Extent seen = GUI::GET::measured(document(), board().c_str());
  return GUI::SAC::WINDOW::least(seen.w, Float(::REACH));
}

auto SOUND::VIEWS::WAVE::overview() -> Overview {
  Overview read;
  read.placed = ::across;
  for (const Clip &clip : placed()) {
    read.reach = std::max(read.reach, clip.at + clip.frames);
    read.dashes.push_back({Float(clip.at), Float(clip.frames), ::DASH});
  }
  return read;
}
