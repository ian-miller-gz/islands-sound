// SPDX-License-Identifier: AGPL-3.0-or-later
#pragma once
#include <sstream>

#include <store.hpp>

#include "../inventory.hpp"
#include "session.hpp"

namespace SOUND::SESSION {

auto store() -> STORE::Handle;

constexpr Whole SCHEMA = 25;
constexpr Whole SHAPED = 25;
constexpr Whole ADMITTED = 24;
constexpr Whole MASTERED = 19;

constexpr Whole KINDED = 13;
constexpr Whole OPTED = 15;

constexpr Whole NAMED = 7;

constexpr Whole STOCKED = 11;

namespace FIELD {

constexpr STRING::Hot SCHEMED = "schema";

constexpr STRING::Hot SETTING = "setting";

constexpr STRING::Hot FROM = "from";
constexpr STRING::Hot TO = "to";
constexpr STRING::Hot LOOPING = "looping";

constexpr STRING::Hot TEMPO = "tempo";

constexpr STRING::Hot TRACKS = "tracks";
constexpr STRING::Hot STOCKS = "stocks";
constexpr STRING::Hot CLIPS = "clips";
constexpr STRING::Hot RECENTS = "recents";
constexpr STRING::Hot VALUES = "values";
constexpr STRING::Hot HISTORY = "history";
constexpr STRING::Hot UNDONE = "undone";
constexpr STRING::Hot WIRING = "wiring";

}  // namespace FIELD

auto written(const Arrangement &tracks) -> String;
auto written(const Inventory &pool) -> String;
auto written(const Graph &wiring) -> String;
auto written(const Vector<String> &recents) -> String;
auto written(const Setting &setting) -> String;
auto written(const Vector<Value> &values) -> String;
auto written(const History &history, Whole keep) -> String;

auto schemed(const FIELDS::Map &row) -> Whole;
void taken(
  const FIELDS::Map &row, Whole schema, Session &document, Vector<Husk> &husks);
auto written(const Session &document) -> FIELDS::Map;
auto unspoken(const FIELDS::Map &row) -> FIELDS::Map;

void taken(const String &text, Arrangement &tracks, Vector<Husk> &husks);
void taken(const String &text, Inventory &pool, Vector<Husk> &husks);
void taken(const String &text, Graph &wiring, Vector<Husk> &husks);
void taken(const String &text, Vector<String> &recents, Vector<Husk> &husks);
void taken(const String &text, Setting &setting, Vector<Husk> &husks);
void taken(const String &text, Vector<Value> &values, Vector<Husk> &husks);
void taken(const String &text, History &history, Vector<Husk> &husks);

void hung(
  const String &tag, std::istringstream &line, ARRANGEMENT::Track &track);
void hung(const String &tag, std::istringstream &line, Stock &stock);
auto spoken(const String &tag) -> Flag;

auto named(const Graph &wiring, Whole row) -> String;
void older(const String &text, Graph &wiring, Vector<Husk> &husks);
void older(
  const String &text, Arrangement &tracks, const Graph &wiring,
  Vector<Husk> &husks);
void older(
  const String &text, Inventory &pool, const Graph &wiring,
  Vector<Husk> &husks);

void relaid(Whole schema, const Arrangement &tracks, Graph &wiring);

void admitted(Whole schema, const Arrangement &tracks, Graph &wiring);

void culled(Arrangement &tracks, Graph &wiring);

void mastered(Arrangement &tracks, Graph &wiring);

void adopted(const Vector<Husk> &lines, const FIELDS::Map &fields);
auto unread() -> const FIELDS::Map &;
auto spliced(const String &text, STRING::Hot half) -> String;

void husked(const String &line, Whole at, Vector<Husk> &husks);

auto rest(std::istringstream &line) -> String;

auto rowed(const Edit &edit) -> String;
auto rowed(const String &tag, std::istringstream &line, Edit &edit) -> Flag;

auto content(const Edit &edit) -> String;
auto content(const String &tag, std::istringstream &line, Edit &edit) -> Flag;

auto edited(const String &tag, std::istringstream &line, Edit &edit) -> Flag;

}  // namespace SOUND::SESSION
