// SPDX-License-Identifier: AGPL-3.0-or-later
#include <algorithm>

#include "transport.internal.hpp"

namespace {
using namespace SOUND;

auto setting() -> Setting & { return TRANSPORT::clockwork().setting; }

auto within(Float tempo) -> Float {
  return std::clamp(tempo, TRANSPORT::SLOWEST, TRANSPORT::FASTEST);
}

}  // namespace

void SOUND::TRANSPORT::pace(Float tempo) { pace(0, tempo); }

void SOUND::TRANSPORT::pace(Whole at, Float tempo) {
  SCORE::placed(::setting().tempos, Tempo{at, ::within(tempo)});
}

void SOUND::TRANSPORT::metre(Whole at, Whole numerator, Whole denominator) {
  SCORE::placed(::setting().metres, Metre{at, numerator, denominator});
}

void SOUND::TRANSPORT::key(Whole tonic, const String &scale) {
  key(0, tonic, scale);
}

void SOUND::TRANSPORT::key(Whole at, Whole tonic, const String &scale) {
  SCORE::placed(::setting().keys, Key{at, tonic % CLASSES, scale});
}

auto SOUND::TRANSPORT::unpace(Whole at) -> Flag {
  return SCORE::removed(::setting().tempos, at);
}

auto SOUND::TRANSPORT::unmetre(Whole at) -> Flag {
  return SCORE::removed(::setting().metres, at);
}

auto SOUND::TRANSPORT::unkey(Whole at) -> Flag {
  return SCORE::removed(::setting().keys, at);
}

auto SOUND::TRANSPORT::paced() -> Float { return paced(0); }

auto SOUND::TRANSPORT::paced(Whole at) -> Float {
  return SCORE::paced(::setting().tempos, at);
}

auto SOUND::TRANSPORT::metred() -> Metre { return metred(0); }

auto SOUND::TRANSPORT::metred(Whole at) -> Metre {
  return SCORE::metred(::setting().metres, at);
}

auto SOUND::TRANSPORT::keyed() -> Key { return keyed(0); }

auto SOUND::TRANSPORT::keyed(Whole at) -> Key {
  return SCORE::keyed(::setting().keys, at);
}

auto SOUND::TRANSPORT::bar() -> Whole { return bar(0); }

auto SOUND::TRANSPORT::bar(Whole at) -> Whole {
  return SCORE::framed(PULSES, paced(at)) * metred(at).numerator;
}
