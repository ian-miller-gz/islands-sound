// SPDX-License-Identifier: AGPL-3.0-or-later
#pragma once
#include "../arrangement.hpp"
#include "../graph.hpp"
#include "../score.hpp"
#include "../stock.hpp"

namespace SOUND {

struct Placement {
  Whole track = NONE, lane = NONE, placement = NONE;
  ARRANGEMENT::TRACK::LANE::Clip span, was;
};

struct Mapping {
  Whole track = NONE, lane = NONE;
  String map, stood;
};

struct Row {
  Whole track = NONE;
  ARRANGEMENT::Track row;
};

struct Laning {
  Whole track = NONE, lane = NONE;
  ARRANGEMENT::TRACK::Lane row;
  Port port;
  Flag admits = false;
};

struct Change {
  String word, at, now, stood;
};

struct Noted {
  Whole stock = NONE, index = NONE;
  Note note{.at = NONE}, was{.at = NONE};
};

struct Plotted {
  Whole stock = NONE, index = NONE, number = 0;
  Point point{.at = NONE}, was{.at = NONE};
};

struct Edit {
  enum Verb : Whole { REMOVED, ADDED, CHANGED };
  String act, said;
  Whole verb = ADDED;
  Whole arc = NONE;
  Vector<String> swept;
  Graph graph;
  Vector<Placement> placements;
  Vector<Placement> gone;
  Vector<Mapping> maps;
  Vector<Change> changes;
  Vector<Row> rows;
  Vector<Laning> lanes;
  Vector<Noted> notes;
  Vector<Plotted> plots;
};

struct History {
  Vector<Edit> edits;
  Whole undone = 0;
};

struct Walk {
  Whole mark = 0;
  Flag placing = false, writing = false;
  Vector<Placement> placements;
  Vector<Noted> notes;
};

}  // namespace SOUND

namespace SOUND::HISTORY {

constexpr Whole HORIZON = 1000;

namespace WORD {

constexpr STRING::Hot NAME = "name";
constexpr STRING::Hot BUS = "bus";
constexpr STRING::Hot HOME = "home";
constexpr STRING::Hot QUIET = "quiet";

constexpr STRING::Hot TEMPO = "tempo";
constexpr STRING::Hot METRE = "metre";
constexpr STRING::Hot SCALE = "scale";
constexpr STRING::Hot TAG = "tag";
constexpr STRING::Hot LOOP = "loop";
constexpr STRING::Hot ENDS = "ends";

constexpr STRING::Hot ON = "on";
constexpr STRING::Hot OFF = "off";

}  // namespace WORD

auto held() -> const History &;

auto begin() -> Whole;

void end();

void record(Edit edit);

void said(const String &phrase);

void take(Whole stock, const Stock &clip);

void wrote(STRING::Hot act, Whole stock, const Stock &clip);

void restored(const Edit &edit, Flag back, Whole stock, Stock &clip);

auto undoable() -> const Edit *;
auto redoable() -> const Edit *;
void undone();
void redone();

auto walked() -> const Walk &;

void adopt(const History &history);

void trim(Whole keep);

auto counted() -> Whole;

auto touched() -> const Touched &;

void settled();

void close();

}  // namespace SOUND::HISTORY
