// SPDX-License-Identifier: AGPL-3.0-or-later
#include <algorithm>

#include <island/input.hpp>

#include "keys.hpp"

namespace {
using namespace SOUND;

void turned(KEYS::Board &board, Whole place, Flag down) {
  Whole pitch = 0;
  if (!KEYS::pitched(board, place, pitch)) return;
  board.ring.push(
    {.kind =
       down ? AUDIO::PLUGIN::Event::NOTE_ON : AUDIO::PLUGIN::Event::NOTE_OFF,
     .offset = 0,
     .index = pitch,
     .value = down ? KEYS::FORCE : 0.0f});
}

void listen(void *instance) {
  auto &board = *static_cast<KEYS::Board *>(instance);
  for (Whole place = 0; place < KEYS::SPAN; ++place) {
    const Flag down = INPUT::GET::held(KEYS::keyed(place));
    if (down == board.down[place]) continue;
    board.down[place] = down;
    ::turned(board, place, down);
  }
}

void turned(KEYS::Board &board, const AUDIO::PLUGIN::Event &event) {
  if (event.kind != AUDIO::PLUGIN::Event::CONTROLLER) return;
  if (event.index != KEYS::OCTAVE) return;
  board.octave = std::clamp(event.value, KEYS::LEAST, KEYS::MOST);
}

auto answer(
  void *instance, const AUDIO::PLUGIN::Event *events, Whole count,
  AUDIO::PLUGIN::Event *out, Whole room) -> Whole {
  auto &board = *static_cast<KEYS::Board *>(instance);
  for (Whole at = 0; at < count; ++at) ::turned(board, events[at]);
  Whole written = 0;
  while (written < room && board.ring.pop(out[written])) ++written;
  return written;
}

auto create(Whole rate, Whole channels) -> void * {
  if (rate == 0 || channels == 0) return nullptr;
  return new KEYS::Board{};
}

void destroy(void *instance) { delete static_cast<KEYS::Board *>(instance); }

const AUDIO::PLUGIN::Plug surface = {
  .create = create,
  .render = nullptr,
  .meter = nullptr,
  .destroy = destroy,
  .parameters = KEYS::SURFACE::parameters,
  .name = KEYS::SURFACE::name,
  .reading = KEYS::SURFACE::reading,
  .held = KEYS::SURFACE::held,
  .control = KEYS::SURFACE::control,
  .ins = {{AUDIO::PLUGIN::Port::CONTROL}},
  .outs = {{AUDIO::PLUGIN::Port::NOTES}}};

[[maybe_unused]] const Flag offered = PLUGIN::offer(
  {.name = "keys", .surface = &surface, .answer = answer, .listen = listen});

}  // namespace

auto SOUND::KEYS::keyed(Whole place) -> Whole {
  const STRING::Hot row = place < SEMITONES ? LOWER : UPPER;
  return static_cast<Whole>(row[place % SEMITONES]);
}

auto SOUND::KEYS::pitched(const Board &board, Whole place, Whole &pitch)
  -> Flag {
  const auto octave = static_cast<Whole>(board.octave);
  pitch = octave * SEMITONES + place;
  return pitch <= HIGHEST;
}
