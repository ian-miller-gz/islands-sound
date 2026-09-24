// SPDX-License-Identifier: AGPL-3.0-or-later
#include "transpose.hpp"

namespace {
using namespace SOUND;

auto placed(const TRANSPOSE::Shift &shift, Whole pitch) -> Whole {
  const Integer moved =
    static_cast<Integer>(pitch) + static_cast<Integer>(shift.steps);
  if (moved < 0 || moved > static_cast<Integer>(TRANSPOSE::HIGHEST))
    return TRANSPOSE::SILENT;
  return static_cast<Whole>(moved);
}

auto struck(
  TRANSPOSE::Shift &shift, AUDIO::PLUGIN::Event event,
  AUDIO::PLUGIN::Event *out, Whole room) -> Whole {
  Whole written = 0;
  const Whole to = ::placed(shift, event.index);
  Whole &held = shift.sounding[event.index];
  if (held != TRANSPOSE::SILENT && held != to && written < room)
    out[written++] = {AUDIO::PLUGIN::Event::NOTE_OFF, event.offset, held, 0};
  held = to;
  if (to == TRANSPOSE::SILENT || written == room) return written;
  event.index = to;
  out[written++] = event;
  return written;
}

auto lifted(
  TRANSPOSE::Shift &shift, AUDIO::PLUGIN::Event event,
  AUDIO::PLUGIN::Event *out, Whole room) -> Whole {
  Whole &held = shift.sounding[event.index];
  if (held == TRANSPOSE::SILENT || room == 0) return 0;
  event.index = held;
  held = TRANSPOSE::SILENT;
  out[0] = event;
  return 1;
}

auto moved(
  TRANSPOSE::Shift &shift, const AUDIO::PLUGIN::Event &event,
  AUDIO::PLUGIN::Event *out, Whole room) -> Whole {
  if (event.kind == AUDIO::PLUGIN::Event::CONTROLLER) {
    if (event.index == TRANSPOSE::STEPS)
      shift.steps = TRANSPOSE::clamped(event.value);
    return 0;
  }
  const Flag noted = event.kind == AUDIO::PLUGIN::Event::NOTE_ON ||
                     event.kind == AUDIO::PLUGIN::Event::NOTE_OFF;
  if (!noted) {
    if (room == 0) return 0;
    out[0] = event;
    return 1;
  }
  if (event.index > TRANSPOSE::HIGHEST) return 0;
  const Flag striking =
    event.kind == AUDIO::PLUGIN::Event::NOTE_ON && event.value > 0;
  return striking ? ::struck(shift, event, out, room)
                  : ::lifted(shift, event, out, room);
}

auto answer(
  void *instance, const AUDIO::PLUGIN::Event *events, Whole count,
  AUDIO::PLUGIN::Event *out, Whole room) -> Whole {
  auto &shift = *static_cast<TRANSPOSE::Shift *>(instance);
  Whole written = 0;
  for (Whole at = 0; at < count; ++at)
    written += ::moved(shift, events[at], out + written, room - written);
  return written;
}

auto create(Whole rate, Whole channels) -> void * {
  if (rate == 0 || channels == 0) return nullptr;
  auto *shift = new TRANSPOSE::Shift{};
  shift->sounding.assign(TRANSPOSE::HIGHEST + 1, TRANSPOSE::SILENT);
  return shift;
}

void destroy(void *instance) {
  delete static_cast<TRANSPOSE::Shift *>(instance);
}

const AUDIO::PLUGIN::Plug surface = {
  .create = create,
  .render = nullptr,
  .meter = nullptr,
  .destroy = destroy,
  .parameters = TRANSPOSE::SURFACE::parameters,
  .name = TRANSPOSE::SURFACE::name,
  .reading = TRANSPOSE::SURFACE::reading,
  .held = TRANSPOSE::SURFACE::held,
  .control = TRANSPOSE::SURFACE::control,
  .ins = {{AUDIO::PLUGIN::Port::NOTES}, {AUDIO::PLUGIN::Port::CONTROL}},
  .outs = {{AUDIO::PLUGIN::Port::NOTES}}};

[[maybe_unused]] const Flag offered =
  PLUG::offer({.name = "transpose", .surface = &surface, .answer = answer});

}  // namespace
