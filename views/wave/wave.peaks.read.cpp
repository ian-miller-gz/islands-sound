// SPDX-License-Identifier: AGPL-3.0-or-later
#include <algorithm>
#include <cmath>

#include "../../timeline.hpp"
#include "wave.internal.hpp"

namespace {
using namespace SOUND;
namespace WAVE = SOUND::VIEWS::WAVE;

auto offset(const WAVE::Clip &clip) -> Whole {
  if (clip.ratio == WAVE::Clip{}.ratio) return clip.from;
  return Whole(
    std::lround(std::max(Float(clip.from) * clip.ratio, WAVE::ORIGIN)));
}

auto joined(const PEAKS::Peak &one, const PEAKS::Peak &two) -> PEAKS::Peak {
  return {std::min(one.least, two.least), std::max(one.most, two.most)};
}

}  // namespace

auto SOUND::VIEWS::WAVE::peak(const Clip &clip, Whole opens, Whole frames)
  -> PEAKS::Peak {
  const TIMELINE::Derived source = TIMELINE::derived(clip.stock, clip);
  if (source.take == nullptr || source.take->lanes.empty()) return {};
  const Take &take = *source.take;
  const Whole carried = take.lanes[0].size();
  if (carried == 0) return {};
  if (frames >= carried) return PEAKS::peak(take, *source.level, 0, carried);
  const Whole reads = TIMELINE::wrapped(::offset(clip), opens, carried);
  const Whole head = std::min(frames, carried - reads);
  const PEAKS::Peak one = PEAKS::peak(take, *source.level, reads, head);
  if (head == frames) return one;
  return ::joined(one, PEAKS::peak(take, *source.level, 0, frames - head));
}
