// SPDX-License-Identifier: AGPL-3.0-or-later
#pragma once

#include "../commands.hpp"
#include "../control.hpp"
#include "../history.hpp"

namespace SOUND::COMMANDS {

namespace CHANGE {

auto named(const Change &change) -> String;
auto named(const Change &change, const String &value) -> Flag;

auto bussed(const Change &change) -> String;
auto bussed(const Change &change, const String &value) -> Flag;

auto homed(const Change &change) -> String;
auto homed(const Change &change, const String &value) -> Flag;

auto quieted(const Change &change) -> String;
auto quieted(const Change &change, const String &value) -> Flag;

auto paced(const Change &change) -> String;
auto paced(const Change &change, const String &value) -> Flag;

auto metred(const Change &change) -> String;
auto metred(const Change &change, const String &value) -> Flag;

auto keyed(const Change &change) -> String;
auto keyed(const Change &change, const String &value) -> Flag;

auto flagged(const Change &change) -> String;
auto flagged(const Change &change, const String &value) -> Flag;

auto looped(const Change &change) -> String;
auto looped(const Change &change, const String &value) -> Flag;

auto ended(const Change &change) -> String;
auto ended(const Change &change, const String &value) -> Flag;

}  // namespace CHANGE

auto stated(const Change &change) -> String;

auto state(const Change &change, const String &value) -> Flag;

auto kept(Change change, const String &said = String()) -> Flag;

auto landing(Whole track, Whole kind) -> Whole;
void covered(Whole track, Whole lane, Whole opens, Whole closes);

void landed(const CONTROL::Take &take);

void loops(SHELL::Session &session);

auto lined(Whole index, const Edit &edit) -> String;

void rowed(const Edit &edit, Flag standing);

void rewrote(const Edit &edit, Flag back);

auto walked(SHELL::Session &session, Flag back) -> String;

struct Asked {
  Whole words = 0, at = 0;
  Flag placed = false, off = false;
};

auto asked(const SHELL::Session &session) -> Asked;

struct Gathered : SHELL::Session {
  Vector<String> lines;
  void print(const String &line) override { lines.push_back(line); }
  void clear() override {}
  void close() override {}
};

}  // namespace SOUND::COMMANDS
