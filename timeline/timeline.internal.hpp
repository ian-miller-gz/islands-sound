// SPDX-License-Identifier: AGPL-3.0-or-later
#pragma once
#include "timeline.doors.hpp"

namespace SOUND::TIMELINE {

auto standing() -> Arrangement &;

auto valued(const Curve &curve, Whole at) -> Float;

auto claimed(const Vector<String> &claims, const String &node) -> Flag;

struct Sending {
  Whole track = NONE;
  String root;
  Whole lane = 0;
  Whole from = 0, until = 0;
};

void stamp(
  Vector<Sent> &run, const Sending &where,
  const ARRANGEMENT::TRACK::LANE::Clip &span, Whole carried, const Sent &edge);

void turned(
  Vector<Sent> &run, const Stock &stock, const Sending &where,
  const ARRANGEMENT::TRACK::LANE::Clip &span, Whole grain);

}  // namespace SOUND::TIMELINE
