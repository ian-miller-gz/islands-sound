// SPDX-License-Identifier: AGPL-3.0-or-later
#include <algorithm>
#include <utility>

#include "history.internal.hpp"

namespace {

SOUND::History ledger;
Whole arcs = 0, standing = SOUND::NONE, depth = 0;
String phrase;

auto cursor() -> Whole { return ::ledger.edits.size() - ::ledger.undone; }

}  // namespace

auto SOUND::HISTORY::held() -> const History & { return ::ledger; }

auto SOUND::HISTORY::begin() -> Whole {
  if (::depth++ == 0) {
    ::standing = ::arcs++;
    ::phrase.clear();
  }
  return ::standing;
}

void SOUND::HISTORY::end() {
  if (::depth == 0 || --::depth != 0) return;
  ::standing = NONE;
  ::phrase.clear();
  released();
}

auto SOUND::HISTORY::standing() -> Whole { return ::standing; }

auto SOUND::HISTORY::arced() -> Edit * {
  Vector<Edit> &edits = ::ledger.edits;
  if (::standing == NONE || ::cursor() != edits.size() || edits.empty())
    return nullptr;
  return edits.back().arc == ::standing ? &edits.back() : nullptr;
}

void SOUND::HISTORY::adopt(const History &history) {
  ::ledger = history;
  ::ledger.undone = std::min(::ledger.undone, Whole(::ledger.edits.size()));
  for (const Edit &edit : ::ledger.edits)
    if (edit.arc != NONE && edit.arc >= ::arcs) ::arcs = edit.arc + 1;
  stir();
}

void SOUND::HISTORY::said(const String &phrase) {
  ::phrase = phrase;
  if (Edit *held = arced(); held != nullptr) held->said = phrase;
}

void SOUND::HISTORY::record(Edit edit) {
  edit.arc = ::standing;
  if (!::phrase.empty()) edit.said = ::phrase;
  if (::standing == NONE) ::phrase.clear();
  Vector<Edit> &edits = ::ledger.edits;
  edits.resize(::cursor());
  ::ledger.undone = 0;
  stir(edit);
  if (edit.arc != NONE && !edits.empty() && edits.back().arc == edit.arc)
    return folded(edits.back(), edit);
  edits.push_back(std::move(edit));
  trim(HORIZON);
}

auto SOUND::HISTORY::undoable() -> const Edit * {
  return ::cursor() == 0 ? nullptr : &::ledger.edits[::cursor() - 1];
}

auto SOUND::HISTORY::redoable() -> const Edit * {
  return ::ledger.undone == 0 ? nullptr : &::ledger.edits[::cursor()];
}

void SOUND::HISTORY::undone() {
  if (::cursor() == 0) return;
  stir(*undoable());
  walked(*undoable(), true);
  ::ledger.undone += 1;
}

void SOUND::HISTORY::redone() {
  if (::ledger.undone == 0) return;
  stir(*redoable());
  walked(*redoable(), false);
  ::ledger.undone -= 1;
}

void SOUND::HISTORY::trim(Whole keep) {
  Vector<Edit> &edits = ::ledger.edits;
  if (keep >= edits.size()) return;
  edits.erase(edits.begin(), edits.end() - keep);
  if (::ledger.undone > keep) ::ledger.undone = keep;
}

void SOUND::HISTORY::close() {
  ::ledger = {};
  ::standing = NONE;
  ::depth = 0;
  ::phrase.clear();
  released();
}
