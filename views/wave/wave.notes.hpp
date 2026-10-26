// SPDX-License-Identifier: AGPL-3.0-or-later
#pragma once
#include "../../contour.hpp"
#include "wave.internal.hpp"

namespace SOUND::VIEWS::WAVE {

constexpr STRING::Hot NOTE = "wavenote";

struct Bar {
  Whole at = 0, run = 0, pitch = 0;
  Float value = 0.0f, was = 0.0f;
};

struct Sung {
  CONTOUR::Found found;
  Float was = 0.0f, now = 0.0f;
};

struct Band {
  Float low = 0.0f, high = 0.0f;
};

auto bars(Float west, Float east) -> Vector<Bar>;

auto sung(const Clip &clip) -> Vector<Sung>;
auto banded(const Vector<Sung> &sung) -> Band;
void tie(GUI::Handle page, const String &bar);
void tie(GUI::Handle page, const String &bar, Float run, Float rise);

auto notes(const Clip &clip) -> Vector<CONTOUR::Found>;
auto spelled(Whole pitch) -> String;
void notes();
void notes(SHELL::Session &session);

namespace SHED {

void notes();

}  // namespace SHED

}  // namespace SOUND::VIEWS::WAVE
