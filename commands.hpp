// SPDX-License-Identifier: AGPL-3.0-or-later
#pragma once
#include <cartridge/interface.hpp>

#include "graph.hpp"

namespace SOUND::COMMANDS {

auto table() -> const Vector<SHELL::Command> &;

void document(SHELL::Session &session);
void save(SHELL::Session &session);
void open(SHELL::Session &session);
void saved(SHELL::Session &session);
void husks(SHELL::Session &session);

auto save(const String &name) -> String;
auto open(const String &name) -> String;
void fresh();
void fresh(SHELL::Session &session);

void stand(SHELL::Session &session, const Graph &plan);

void tend();
void land();

void timing(SHELL::Session &session);
void play(SHELL::Session &session);
void stop(SHELL::Session &session);
void locate(SHELL::Session &session);
void chase(SHELL::Session &session);

void settings(SHELL::Session &session);
void tempo(SHELL::Session &session);
void metre(SHELL::Session &session);
void cycle(SHELL::Session &session);

void scale(SHELL::Session &session);

void tag(SHELL::Session &session);
void untag(SHELL::Session &session);
void unloop(SHELL::Session &session);
void ends(SHELL::Session &session);
void ahead(SHELL::Session &session);
void behind(SHELL::Session &session);

void offers(SHELL::Session &session);
void seat(SHELL::Session &session);
void surface(SHELL::Session &session);
void io(SHELL::Session &session);
void main(SHELL::Session &session);
void pair(SHELL::Session &session);
void bus(SHELL::Session &session);
void root(SHELL::Session &session);
void wire(SHELL::Session &session);
void unwire(SHELL::Session &session);
void quiet(SHELL::Session &session);
void bypass(SHELL::Session &session);
void unseat(SHELL::Session &session);
void home(SHELL::Session &session);
void wiring(SHELL::Session &session);
void peak(SHELL::Session &session);

void note(SHELL::Session &session);
void turn(SHELL::Session &session);
void bounce(SHELL::Session &session);

auto root(Whole track, const String &name) -> String;

auto laned(Whole track, Whole kind) -> Whole;

auto admitted(Whole track, Whole lane, Flag on) -> Flag;
auto recast(Whole track, Whole lane, Whole kind) -> Flag;

auto unlaned(Whole track, Whole lane) -> Flag;

auto unseated(const String &node) -> Flag;
auto unseated(const String &node, Vector<String> &gone) -> Flag;

void tracks(SHELL::Session &session);
void track(SHELL::Session &session);
auto track(const String &name) -> Whole;
auto tracked(Whole track) -> String;
void name(SHELL::Session &session);
void drop(SHELL::Session &session);

auto dropped(Whole track) -> Flag;

void lane(SHELL::Session &session);
void unlane(SHELL::Session &session);
void kind(SHELL::Session &session);
void admit(SHELL::Session &session);
void place(SHELL::Session &session);
void span(SHELL::Session &session);
void lift(SHELL::Session &session);
void restore(SHELL::Session &session);

auto joined(Whole track, Whole lane, Vector<Whole> rows) -> Whole;
void join(SHELL::Session &session);

struct Export {
  String path;
  Whole frames = 0;
  String refusal;
};

auto exported(Whole track, const String &path) -> Export;
void exporting(SHELL::Session &session);

void map(SHELL::Session &session);

auto named(Whole track, Whole lane, const String &name) -> Flag;

auto bussed(Whole track, Flag on) -> Flag;

auto quieted(const String &node, Flag on) -> Flag;

auto homed(const String &node, const String &page, Float across, Float down)
  -> Flag;

auto paced(Whole at, Float tempo) -> Flag;
auto paced(Whole at) -> Flag;

auto metred(Whole at, Whole numerator, Whole denominator) -> Flag;
auto metred(Whole at) -> Flag;

auto keyed(Whole at, Whole tonic, const String &scale) -> Flag;
auto keyed(Whole at) -> Flag;

auto flagged(Whole at, const String &text) -> Flag;
auto flagged(Whole at) -> Flag;

auto looped(Whole from, Whole to, const String &text) -> Flag;
auto looped(Whole from) -> Flag;

auto ended(Whole from, Whole to) -> Flag;

void stocks(SHELL::Session &session);
void stock(SHELL::Session &session);
void take(SHELL::Session &session);

void author(SHELL::Session &session);

void move(SHELL::Session &session);
void stretch(SHELL::Session &session);
void erase(SHELL::Session &session);

void aim(SHELL::Session &session);
void plot(SHELL::Session &session);
void dial(SHELL::Session &session);

void sounding(SHELL::Session &session);

void history(SHELL::Session &session);
void undo(SHELL::Session &session);
void redo(SHELL::Session &session);
auto undo() -> String;
auto redo() -> String;
void trim(SHELL::Session &session);

void roll(SHELL::Session &session);

void beat(SHELL::Session &session);
void drive(SHELL::Session &session);
void strike(SHELL::Session &session);
void hear(SHELL::Session &session);
void record(SHELL::Session &session);

void input(SHELL::Session &session);

void views(SHELL::Session &session);
void view(SHELL::Session &session);
void panel(SHELL::Session &session);
void zoom(SHELL::Session &session);
void pan(SHELL::Session &session);

void look(SHELL::Session &session);

}  // namespace SOUND::COMMANDS
