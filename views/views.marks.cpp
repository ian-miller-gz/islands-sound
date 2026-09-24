// SPDX-License-Identifier: AGPL-3.0-or-later
#include <algorithm>

#include "views.marks.hpp"

namespace {
using namespace SOUND;

constexpr Whole FIRST = 1;

void gathered(
  Vector<GUI::SAC::LADDER::Mark> &marks, const TRANSPORT::Section &one,
  const GUI::SAC::LADDER::Rung &rung, Flag head, Float pan, Float seen) {
  const Float opens = Float(one.at);
  const Float closes = opens + Float(one.span);
  const Float west = head ? pan : std::max(pan, opens);
  const Float east = one.span == 0 ? pan + seen : std::min(pan + seen, closes);
  if (east <= west) return;
  for (GUI::SAC::LADDER::Mark &mark :
       GUI::SAC::LADDER::dress(rung, one.bar, west - opens, east - west)) {
    mark.at += Integer(one.at);
    if (one.span != 0 && mark.at >= Integer(one.at + one.span)) break;
    mark.label = rung.stated != nullptr && mark.at >= 0
                   ? rung.stated(Whole(mark.at))
                   : String();
    marks.push_back(mark);
  }
}

}  // namespace

auto SOUND::VIEWS::dressed(
  const Vector<TRANSPORT::Section> &sections, Ladder ladder, Float scale,
  Float crowd, Float pan, Float seen) -> Vector<GUI::SAC::LADDER::Mark> {
  Vector<GUI::SAC::LADDER::Mark> marks;
  for (Whole seat = 0; seat < sections.size(); ++seat)
    ::gathered(
      marks, sections[seat],
      GUI::SAC::LADDER::climb(ladder(sections[seat].bar), scale, crowd),
      seat == 0, pan, seen);
  return marks;
}

auto SOUND::VIEWS::dressed(
  const Vector<TRANSPORT::Section> &sections,
  const GUI::SAC::LADDER::Rung &grain, Whole factor, Float scale, Float crowd,
  Float pan, Float seen) -> Vector<GUI::SAC::LADDER::Mark> {
  const GUI::SAC::LADDER::Rung rung{
    GUI::SAC::LADDER::climb(grain.span, factor, scale, crowd, seen), grain.name,
    grain.stated};
  Vector<GUI::SAC::LADDER::Mark> marks;
  for (Whole seat = 0; seat < sections.size(); ++seat)
    ::gathered(marks, sections[seat], rung, seat == 0, pan, seen);
  return marks;
}

auto SOUND::VIEWS::standing(
  const Vector<TRANSPORT::Section> &sections, Whole at) -> TRANSPORT::Section {
  if (sections.empty()) return {};
  Whole seat = 0;
  while (seat + 1 < sections.size() && sections[seat + 1].at <= at) ++seat;
  return sections[seat];
}

auto SOUND::VIEWS::numbered(
  const Vector<TRANSPORT::Section> &sections, Integer at) -> String {
  if (at < 0 || sections.empty()) return String();
  const TRANSPORT::Section one = standing(sections, Whole(at));
  const Whole into = Whole(at) - one.at;
  if (one.bar == 0 || into % one.bar != 0) return String();
  return std::to_string(one.bars + into / one.bar + ::FIRST);
}

auto SOUND::VIEWS::spanned(
  const Vector<GUI::SAC::LADDER::Mark> &marks, Whole at, Whole span) -> Whole {
  if (at + 1 < marks.size()) return Whole(marks[at + 1].at - marks[at].at);
  if (at > 0 && at < marks.size())
    return Whole(marks[at].at - marks[at - 1].at);
  return span;
}
