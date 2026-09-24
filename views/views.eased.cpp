// SPDX-License-Identifier: AGPL-3.0-or-later
#include "../transport.hpp"
#include "views.hpp"

namespace {
using namespace SOUND;

Float drawn = 0.0f;
Float pace = 0.0f;
Float last = 0.0f;
Whole placed = 0;
Flag standing = false;

}  // namespace

auto SOUND::VIEWS::eased(Float held, Float wanted) -> Float {
  const Float gap = wanted - held;
  return gap < SETTLE && gap > -SETTLE ? wanted : held + gap * EASE;
}

void SOUND::VIEWS::glide() {
  const TRANSPORT::Marker mark = TRANSPORT::marker();
  const Float reading = Float(mark.position);
  const Flag fell = reading < ::last;
  const Flag put = mark.locates != ::placed;
  ::last = reading;
  ::placed = mark.locates;
  if (!mark.playing || !::standing || fell || put) {
    ::standing = true;
    ::drawn = reading;
    return;
  }
  ::drawn += ::pace;
  const Float residual = reading - ::drawn;
  ::drawn += residual * EASE;
  ::pace += residual * TRIM;
}

auto SOUND::VIEWS::clock() -> Whole {
  return ::drawn <= 0.0f ? 0 : Whole(::drawn + 0.5f);
}
