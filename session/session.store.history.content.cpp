// SPDX-License-Identifier: AGPL-3.0-or-later
#include <format>

#include "session.store.internal.hpp"

using namespace SOUND;

namespace {

auto noted(const Vector<Noted> &notes) -> String {
  String text;
  for (const Noted &one : notes)
    text += std::format(
      "sn {} {} {} {} {} {} {} {} {} {}\n", one.stock, one.index,
      one.note.pitch, one.note.at, one.note.length, one.note.velocity,
      one.was.pitch, one.was.at, one.was.length, one.was.velocity);
  return text;
}

auto plotted(const Vector<Plotted> &plots) -> String {
  String text;
  for (const Plotted &one : plots)
    text += std::format(
      "sp {} {} {} {} {} {} {} {} {}\n", one.stock, one.index, one.number,
      one.point.at, one.point.value, one.point.shape, one.was.at, one.was.value,
      one.was.shape);
  return text;
}

}  // namespace

auto SOUND::SESSION::content(const Edit &edit) -> String {
  return ::noted(edit.notes) + ::plotted(edit.plots);
}

auto SOUND::SESSION::content(
  const String &tag, std::istringstream &line, Edit &edit) -> Flag {
  if (tag == "sn") {
    Noted one;
    line >> one.stock >> one.index >> one.note.pitch >> one.note.at >>
      one.note.length >> one.note.velocity >> one.was.pitch >> one.was.at >>
      one.was.length >> one.was.velocity;
    edit.notes.push_back(one);
    return true;
  }
  if (tag != "sp") return false;
  Plotted one;
  line >> one.stock >> one.index >> one.number >> one.point.at >>
    one.point.value >> one.point.shape >> one.was.at >> one.was.value >>
    one.was.shape;
  edit.plots.push_back(one);
  return true;
}
