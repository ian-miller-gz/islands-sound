// SPDX-License-Identifier: AGPL-3.0-or-later
#include <format>
#include <sstream>

#include "session.store.internal.hpp"

using namespace SOUND;

namespace {

void trailing(std::istringstream &line, Whole &shape) {
  Whole token = 0;
  if (line >> token) shape = token;
}

}  // namespace

void SOUND::SESSION::hung(
  const String &tag, std::istringstream &line, ARRANGEMENT::Track &track) {
  if (tag == "b") {
    track.bus = true;
  } else if (tag == "k") {
    track.root = rest(line);
  } else if (tag == "l") {
    ARRANGEMENT::TRACK::Lane lane;
    line >> lane.kind;
    lane.name = rest(line);
    track.lanes.push_back(lane);
  } else if (tag == "m") {
    if (!track.lanes.empty()) track.lanes.back().map = rest(line);
  } else if (!track.lanes.empty()) {
    ARRANGEMENT::TRACK::LANE::Clip span;
    line >> span.stock >> span.at >> span.from >> span.frames;
    (tag == "o" ? track.lanes.back().outtakes : track.lanes.back().clips)
      .push_back(span);
  }
}

auto SOUND::SESSION::spoken(const String &tag) -> Flag {
  return tag == "w" || tag == "n" || tag == "a" || tag == "v" || tag == "u" ||
         tag == "g";
}

void SOUND::SESSION::hung(
  const String &tag, std::istringstream &line, Stock &stock) {
  if (tag == "w") {
    stock.take.path = SESSION::rest(line);
  } else if (tag == "n") {
    Note note;
    line >> note.pitch >> note.at >> note.length >> note.velocity;
    stock.score.notes.push_back(note);
  } else if (tag == "a") {
    line >> stock.curve.address.parameter;
    stock.curve.address.node = SESSION::rest(line);
  } else if (tag == "v") {
    Point point;
    line >> point.at >> point.value;
    ::trailing(line, point.shape);
    stock.curve.points.push_back(point);
  } else if (tag == "u") {
    Turn move;
    line >> move.at >> move.number >> move.value;
    ::trailing(line, move.shape);
    stock.turns.push_back(move);
  } else {
    Choice choice;
    line >> choice.at >> choice.program;
    stock.choices.push_back(choice);
  }
}

auto SOUND::SESSION::rest(std::istringstream &line) -> String {
  String name;
  std::getline(line >> std::ws, name);
  return name;
}

auto SOUND::SESSION::written(const Arrangement &tracks) -> String {
  String text;
  for (const ARRANGEMENT::Track &track : tracks.tracks) {
    text += std::format("t {}\n", track.name);
    if (track.bus) text += "b\n";
    if (!track.root.empty()) text += std::format("k {}\n", track.root);
    for (const ARRANGEMENT::TRACK::Lane &lane : track.lanes) {
      text += std::format("l {} {}\n", lane.kind, lane.name);
      if (!lane.map.empty()) text += std::format("m {}\n", lane.map);
      for (const ARRANGEMENT::TRACK::LANE::Clip &span : lane.clips)
        text += std::format(
          "p {} {} {} {}\n", span.stock, span.at, span.from, span.frames);
      for (const ARRANGEMENT::TRACK::LANE::Clip &span : lane.outtakes)
        text += std::format(
          "o {} {} {} {}\n", span.stock, span.at, span.from, span.frames);
    }
  }
  return text;
}

auto SOUND::SESSION::written(const Inventory &pool) -> String {
  String text;
  for (const Stock &stock : pool.stocks) {
    text += std::format("c {} {}\n", stock.kind, stock.name);
    if (!stock.take.path.empty())
      text += std::format("w {}\n", stock.take.path);
    for (const Note &note : stock.score.notes)
      text += std::format(
        "n {} {} {} {}\n", note.pitch, note.at, note.length, note.velocity);
    if (stock.kind == KIND::LOGIC)
      text += std::format(
        "a {} {}\n", stock.curve.address.parameter, stock.curve.address.node);
    for (const Point &point : stock.curve.points)
      text += std::format("v {} {} {}\n", point.at, point.value, point.shape);
    for (const Turn &move : stock.turns)
      text += std::format(
        "u {} {} {} {}\n", move.at, move.number, move.value, move.shape);
    for (const Choice &choice : stock.choices)
      text += std::format("g {} {}\n", choice.at, choice.program);
  }
  return text;
}

auto SOUND::SESSION::written(const Vector<String> &recents) -> String {
  String text;
  for (const String &plugin : recents) text += std::format("r {}\n", plugin);
  return text;
}

void SOUND::SESSION::taken(
  const String &text, Vector<String> &recents, Vector<Husk> &husks) {
  std::istringstream lines(text);
  Whole at = 0;
  for (String held; std::getline(lines, held); ++at) {
    std::istringstream line(held);
    String tag;
    line >> tag;
    if (tag == "r")
      recents.push_back(rest(line));
    else
      husked(held, at, husks);
  }
}

void SOUND::SESSION::taken(
  const String &text, Arrangement &tracks, Vector<Husk> &husks) {
  std::istringstream lines(text);
  Whole at = 0;
  for (String held; std::getline(lines, held); ++at) {
    std::istringstream line(held);
    String tag;
    line >> tag;
    if (tag == "t") {
      ARRANGEMENT::Track track;
      track.name = rest(line);
      tracks.tracks.push_back(track);
    } else if (
      tag == "b" || tag == "k" || tag == "l" || tag == "m" || tag == "p" ||
      tag == "o") {
      if (!tracks.tracks.empty()) hung(tag, line, tracks.tracks.back());
    } else {
      husked(held, at, husks);
    }
  }
}

void SOUND::SESSION::taken(
  const String &text, Inventory &pool, Vector<Husk> &husks) {
  std::istringstream lines(text);
  Whole at = 0;
  for (String held; std::getline(lines, held); ++at) {
    std::istringstream line(held);
    String tag;
    line >> tag;
    if (tag == "c") {
      Stock stock;
      line >> stock.kind;
      stock.name = rest(line);
      pool.stocks.push_back(stock);
    } else if (spoken(tag)) {
      if (!pool.stocks.empty()) hung(tag, line, pool.stocks.back());
    } else {
      husked(held, at, husks);
    }
  }
}
