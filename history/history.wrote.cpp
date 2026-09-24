// SPDX-License-Identifier: AGPL-3.0-or-later
#include <algorithm>
#include <utility>

#include "history.internal.hpp"

namespace {
using namespace SOUND;

struct Held {
  Whole stock = NONE;
  Stock clip;
};

Vector<::Held> holdings;

auto holding(Whole stock) -> ::Held * {
  for (::Held &one : ::holdings)
    if (one.stock == stock) return &one;
  return nullptr;
}

void freed(Whole stock) {
  for (Whole one = 0; one < ::holdings.size(); ++one)
    if (::holdings[one].stock == stock)
      return (void)::holdings.erase(
        std::next(::holdings.begin(), Integer(one)));
}

template <typename Fact>
void refreshed(Vector<Fact> &into, Whole stock, const Vector<Fact> &rows) {
  into.erase(
    std::remove_if(
      into.begin(), into.end(),
      [stock](const Fact &one) { return one.stock == stock; }),
    into.end());
  into.insert(into.end(), rows.begin(), rows.end());
}

}  // namespace

void SOUND::HISTORY::take(Whole stock, const Stock &clip) {
  if (::holding(stock) != nullptr) return;
  ::holdings.push_back({stock, clip});
  ::holdings.back().clip.take = {};
}

void SOUND::HISTORY::released() { ::holdings.clear(); }

void SOUND::HISTORY::wrote(STRING::Hot act, Whole stock, const Stock &clip) {
  const ::Held *held = ::holding(stock);
  if (held == nullptr) return;
  Edit edit = {.act = String(act), .verb = Edit::CHANGED};
  differed(stock, held->clip, clip, edit.notes, edit.plots);
  if (standing() == NONE) ::freed(stock);
  Edit *tail = arced();
  if (tail == nullptr) {
    if (edit.notes.empty() && edit.plots.empty()) return;
    return record(std::move(edit));
  }
  ::refreshed(tail->notes, stock, edit.notes);
  ::refreshed(tail->plots, stock, edit.plots);
}
