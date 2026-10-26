// SPDX-License-Identifier: AGPL-3.0-or-later
#pragma once

#include "graph.internal.hpp"

namespace SOUND::VIEWS::WIRED::BROWSE {

constexpr STRING::Hot UNDER = "·";
constexpr STRING::Hot PANEL = "graph.browse";
constexpr STRING::Hot ROWS = "graph.browse.rows";
constexpr STRING::Hot HEAD = "graph.browse.head";
constexpr STRING::Hot UP = "graph.browse.up";
constexpr STRING::Hot FIND = "graph.browse.find";

constexpr STRING::Hot NOTHING = "-";
constexpr STRING::Hot INTO = "▸";
constexpr STRING::Hot WITHIN = "/";
constexpr STRING::Hot FILES = "files";

constexpr STRING::Hot STANDARDS[] = {PLUGIN::NATIVE, PLUGIN::VST3};

struct Filing {
  STRING::Hot type, path;
};
constexpr Filing FILED[] = {
  {"instrument", "instruments"}, {"emulator", "instruments/emulators"},
  {"effect", "effects"},         {"notes", "notes"},
  {"control", "control"},        {"device", "devices"},
  {"modular", "modular"},        {PLUGIN::PERCUSSION, "percussion"}};
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
  String path;
};

auto shelved(const Vector<Whole> &takes, const Vector<Whole> &gives) -> Whole;

auto rostered(const String &filing) -> Vector<Offer>;
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
void listing(GUI::Handle page, const Vector<Offer> &rows);

auto files(const String &filing) -> Vector<Offer>;

struct Folder {
  String path;
  Vector<String> folders, waves;
};

auto read(const String &filing) -> Folder;
auto housed(const String &filing) -> Flag;
void forget();

auto found(const String &run) -> Vector<Offer>;
auto shown(const String &filing) -> Vector<Offer>;
void sought(GUI::Handle page);
void seat(GUI::Handle page, Flag on);
void took(GUI::Handle page, const Vector<Offer> &rows);

auto land(const String &path) -> String;
auto words() -> Vector<SHELL::Command>;

}  // namespace SOUND::VIEWS::WIRED::BROWSE
