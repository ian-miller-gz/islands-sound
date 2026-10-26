// SPDX-License-Identifier: AGPL-3.0-or-later
#pragma once
#include <common.hpp>
#include <island/gui/gui.hpp>
#include <island/input.hpp>

#include "piano.internal.hpp"

namespace SOUND::VIEWS::ROLL {

struct Act {
  INPUT::KEYS::Action action = INPUT::KEYS::TEXT;
  Whole letter = 0;
  Flag control = false;
  Flag shift = false;
  void (*act)(GUI::Handle page) = nullptr;
};

auto acted(GUI::Handle page, const INPUT::KEYS::Event &key) -> Flag;

void drawn(Flag pencil);
auto drawn() -> Flag;

void all(GUI::Handle page);

void dropped(GUI::Handle page);

struct Step {
  Integer grains = 0, bars = 0, rows = 0, octaves = 0;
};

struct Landing {
  Pip pip;
  Whole at = 0, pitch = 0;
};

auto seated(GUI::Handle page) -> Vector<Pip>;

auto clipped(const Pip &pip, Integer by) -> Whole;

void landed(const Vector<Landing> &landings, STRING::Hot act);

void moved(GUI::Handle page, const Step &step);
void shifted(Whole slot, Whole at, Whole pitch);
void doubled(GUI::Handle page);
void quantized(GUI::Handle page);

}  // namespace SOUND::VIEWS::ROLL
