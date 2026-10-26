// SPDX-License-Identifier: AGPL-3.0-or-later
#include <island/gui/nga.hpp>

#include "../../score.hpp"
#include "../../transport.hpp"
#include "../views.marks.hpp"
#include "wave.internal.hpp"

namespace {
using namespace SOUND;
namespace WAVE = SOUND::VIEWS::WAVE;

constexpr Float CROWD = 44.0f;
constexpr Whole COARSER = 2;

auto sections() -> Vector<TRANSPORT::Section> {
  return TRANSPORT::framed(TRANSPORT::sections());
}

auto beat() -> Whole { return SCORE::framed(PULSES, TRANSPORT::held().tempos); }

auto barred(Whole frames) -> String {
  return VIEWS::numbered(::sections(), Integer(frames));
}

}  // namespace

auto SOUND::VIEWS::WAVE::rung() -> Whole {
  return GUI::SAC::LADDER::climb(
    ::beat(), ::COARSER, scaled(), ::CROWD, window());
}

auto SOUND::VIEWS::WAVE::marks() -> Vector<GUI::SAC::LADDER::Mark> {
  const Float west = GUI::NGA::GET::pan(document(), board().c_str()).x;
  const String stem = strip();
  return VIEWS::dressed(
    ::sections(), {::beat(), stem.c_str(), ::barred}, ::COARSER, scaled(),
    ::CROWD, west, window());
}
