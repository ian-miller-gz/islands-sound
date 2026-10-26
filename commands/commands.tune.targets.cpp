// SPDX-License-Identifier: AGPL-3.0-or-later
#include <cmath>
#include <limits>

#include "../contour.hpp"
#include "../transport.hpp"
#include "commands.tune.hpp"

namespace {
using namespace SOUND;
using ARRANGEMENT::TRACK::LANE::Clip;
using ARRANGEMENT::TRACK::LANE::Tune;

constexpr Float CENT = Float(TIMELINE::SHIFT::SEMITONE);

auto nearest(const Key &key, Float sung) -> Float {
  const Integer middle = Integer(std::lround(sung));
  Float best = Float(middle), gap = std::numeric_limits<Float>::max();
  for (Integer pitch = middle - Integer(CLASSES);
       pitch <= middle + Integer(CLASSES); ++pitch)
    if (
      pitch >= 0 && SCORE::degree(key, Whole(pitch)) != NONE &&
      std::abs(Float(pitch) - sung) < gap) {
      best = Float(pitch);
      gap = std::abs(best - sung);
    }
  return best;
}

auto sourced(Whole frames, const Clip &clip) -> Whole {
  return Whole(std::lround(Float(frames) / clip.ratio));
}

}  // namespace

auto SOUND::COMMANDS::TUNE::tunes(const Clip &plain, Float strength)
  -> Vector<Tune> {
  const TIMELINE::Derived source = TIMELINE::derived(plain.stock, plain);
  if (source.take == nullptr) return {};
  Vector<Tune> tunes;
  for (const CONTOUR::Found &found :
       CONTOUR::noted(CONTOUR::traced(*source.take))) {
    const Vector<Whole> places = TIMELINE::placed(plain, {found.at});
    if (places.empty()) continue;
    const Key key =
      TRANSPORT::keyed(SCORE::pulsed(places.front(), TRANSPORT::held().tempos));
    const Float sung = Float(found.pitch) + Float(found.cents) / ::CENT;
    const Integer cents =
      Integer(std::lround((::nearest(key, sung) - sung) * ::CENT * strength));
    if (cents != 0)
      tunes.push_back(
        {::sourced(found.at, plain), ::sourced(found.frames, plain), cents});
  }
  return tunes;
}
