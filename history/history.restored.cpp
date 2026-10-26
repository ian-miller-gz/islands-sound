// SPDX-License-Identifier: AGPL-3.0-or-later
#include <iterator>

#include "../kind.hpp"
#include "history.internal.hpp"

namespace {
using namespace SOUND;

auto wrote(const Noted &one) -> Flag { return one.was.at == NONE; }
auto wrote(const Plotted &one) -> Flag { return one.was.at == NONE; }

auto erased(const Noted &one) -> Flag { return one.note.at == NONE; }
auto erased(const Plotted &one) -> Flag { return one.point.at == NONE; }

auto changed(const Noted &one) -> Flag {
  return one.note.at != NONE && one.was.at != NONE;
}

auto changed(const Plotted &one) -> Flag {
  return one.point.at != NONE && one.was.at != NONE;
}

auto sided(const Noted &one, Flag back) -> Note {
  return back ? one.was : one.note;
}

auto sided(const Plotted &one, Flag back) -> Point {
  return back ? one.was : one.point;
}

template <typename Row, typename Fact, typename Make>
void patched(Vector<Row> &run, const Vector<Fact> &rows, Flag back, Make make) {
  for (const Fact &one : rows)
    if (::changed(one) && one.index < run.size()) run[one.index] = make(one);
  for (Whole at = rows.size(); at > 0; --at) {
    const Fact &one = rows[at - 1];
    if ((back ? ::wrote(one) : ::erased(one)) && one.index < run.size())
      run.erase(std::next(run.begin(), Integer(one.index)));
  }
  for (const Fact &one : rows)
    if ((back ? ::erased(one) : ::wrote(one)) && one.index <= run.size())
      run.insert(std::next(run.begin(), Integer(one.index)), make(one));
}

void plotted(Stock &clip, const Vector<Plotted> &rows, Flag back) {
  if (rows.empty()) return;
  if (clip.kind == KIND::CONTROL)
    return ::patched(clip.turns, rows, back, [back](const Plotted &one) {
      const Point node = ::sided(one, back);
      return Turn{
        .at = node.at,
        .number = one.number,
        .value = node.value,
        .shape = node.shape};
    });
  if (clip.kind == KIND::PROGRAM)
    return ::patched(clip.choices, rows, back, [back](const Plotted &one) {
      const Point node = ::sided(one, back);
      return Choice{.at = node.at, .program = Whole(node.value)};
    });
  ::patched(clip.curve.points, rows, back, [back](const Plotted &one) {
    return ::sided(one, back);
  });
}

}  // namespace

void SOUND::HISTORY::restored(
  const Edit &edit, Flag back, Whole stock, Stock &clip) {
  Vector<Noted> notes;
  for (const Noted &one : edit.notes)
    if (one.stock == stock) notes.push_back(one);
  Vector<Plotted> plots;
  for (const Plotted &one : edit.plots)
    if (one.stock == stock) plots.push_back(one);
  ::patched(clip.score.notes, notes, back, [back](const Noted &one) {
    return ::sided(one, back);
  });
  ::plotted(clip, plots, back);
}
