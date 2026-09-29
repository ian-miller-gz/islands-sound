// SPDX-License-Identifier: AGPL-3.0-or-later
#pragma once

#include "graph.internal.hpp"

namespace SOUND::VIEWS::WIRED::BROWSE {

constexpr STRING::Hot UNDER = "·";
constexpr STRING::Hot PANEL = "graph.browse";
constexpr STRING::Hot ROWS = "graph.browse.rows";
constexpr STRING::Hot HEAD = "graph.browse.head";
constexpr STRING::Hot UP = "graph.browse.up";

constexpr STRING::Hot NOTHING = "-";
constexpr STRING::Hot INTO = "▸";
constexpr STRING::Hot WITHIN = "/";
constexpr STRING::Hot INPUTS = "inputs";
constexpr STRING::Hot LANES = "lanes";

constexpr STRING::Hot STANDARDS[] = {PLUGIN::NATIVE, PLUGIN::VST3};

struct Filing {
  STRING::Hot type, path;
};
constexpr Filing FILED[] = {
  {"instrument", "instruments"}, {"emulator", "instruments/emulators"},
  {"effect", "effects"},         {"notes", "notes"},
  {"control", "control"},        {"device", "devices"},
  {"modular", "modular"}};
constexpr STRING::Hot VOICED = "instrument";

enum Shelf : Whole { INSTRUMENTS, EFFECTS, SHAPERS, CONTROLLERS, PLUGINS };
constexpr STRING::Hot SHELVES[] = {
  "instruments", "effects", "note effects", "controllers", "plugins"};
static_assert(std::size(SHELVES) == PLUGINS + 1);

struct Offer {
  String name, from;
  Vector<Whole> takes, gives;
  Whole shelf = PLUGINS;
  String target;
  String among;
  String reading;
  Whole lane = NONE;
  String device;
  Flag directory = false;
  Whole holds = 0;
};

auto shelved(const Vector<Whole> &takes, const Vector<Whole> &gives) -> Whole;

auto rows(const String &filing) -> Vector<Offer>;
auto before(const Offer &one, const Offer &other) -> Flag;

auto path(const String &plugin) -> String;
auto catalogued() -> Vector<Offer>;
auto directories(const String &filing, const Vector<Offer> &plugins)
  -> Vector<Offer>;
auto under(const String &filing, const Vector<Offer> &plugins) -> Vector<Offer>;
auto up(const String &filing) -> String;
auto planted(const String &filing, const Vector<Offer> &plugins) -> Flag;
void head(GUI::Handle page, const String &filing);
auto filed(const Offer &offer) -> String;
auto stated(const Offer &offer) -> String;
auto surfacing(const String &directory) -> String;
auto devised(const String &directory) -> Flag;
auto spelled(const Vector<Whole> &kinds) -> String;
auto crossed(const Offer &offer) -> String;
auto carries(const Vector<Whole> &kinds, Whole kind) -> Flag;
void recorded(Vector<Offer> &rows);
void surfaced(Vector<Offer> &rows);
auto lacked(Whole track) -> Vector<Offer>;
void listing(GUI::Handle page, const Vector<Offer> &rows);

}  // namespace SOUND::VIEWS::WIRED::BROWSE
