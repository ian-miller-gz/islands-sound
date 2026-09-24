// SPDX-License-Identifier: AGPL-3.0-or-later
#include "transport.internal.hpp"

namespace {
using namespace SOUND;

auto counted(Whole span, Whole bar) -> Whole {
  return bar == 0 ? 0 : (span + bar - 1) / bar;
}

auto reached(const Vector<Metre> &metres, Whole seat) -> Whole {
  if (seat + 1 >= metres.size()) return 0;
  const Whole next = metres[seat + 1].at, here = metres[seat].at;
  return next > here ? next - here : 0;
}

}  // namespace

auto SOUND::TRANSPORT::sections() -> Vector<Section> {
  const Vector<Metre> &metres = held().metres;
  if (metres.empty()) return {Section{}};
  Vector<Section> cuts;
  Whole bars = 0;
  for (Whole seat = 0; seat < metres.size(); ++seat) {
    const Metre &row = metres[seat];
    const Whole span = ::reached(metres, seat);
    const Whole bar = PULSES * row.numerator;
    cuts.push_back({row.at, span, bar, bars, row.numerator, row.denominator});
    bars += ::counted(span, bar);
  }
  return cuts;
}

auto SOUND::TRANSPORT::framed(const Vector<Section> &sections)
  -> Vector<Section> {
  const Vector<Tempo> &tempos = held().tempos;
  Vector<Section> cuts;
  for (const Section &one : sections) {
    const Whole opens = SCORE::framed(one.at, tempos);
    const Whole closes =
      one.span == 0 ? opens : SCORE::framed(one.at + one.span, tempos);
    cuts.push_back(
      {opens, closes - opens, bar(one.at), one.bars, one.numerator,
       one.denominator});
  }
  return cuts;
}
