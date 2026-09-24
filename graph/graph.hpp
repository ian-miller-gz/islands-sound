// SPDX-License-Identifier: AGPL-3.0-or-later
#pragma once
#include "../kind.hpp"
#include "../plug.hpp"
#include "../score.hpp"

namespace SOUND {

struct Port {
  Whole kind = KIND::AUDIO;
  String name;
  String type;
  Whole lane = NONE;
  Flag bypassed = false;
};

struct Berth {
  String page;
  Float across = NONE, down = NONE;
};

struct Node {
  enum Seat : Whole { PLUG, ROOT, RECORD };
  Whole seat = PLUG;
  String name;
  String plug;
  Vector<Port> ins, outs;
  Float across = NONE, down = NONE;
  String claim;
  Flag quiet = false;
  String device;
  Whole lane = 0;
  Flag clock = false;
  Vector<Berth> berths;
};

struct Wire {
  String from;
  Whole out = 0;
  String to;
  Whole in = 0;
};

enum class Side { IN, OUT };

struct Graph {
  Vector<Node> nodes;
  Vector<Wire> wires;
  Vector<Wire> slack;
};

}  // namespace SOUND

namespace SOUND::GRAPH {

auto held() -> const Graph &;

auto counted() -> Whole;

auto at(const String &name) -> Whole;

auto clock() -> String;

auto clock(const String &device, const String &name) -> String;

auto offers() -> Vector<String>;

auto takes(const String &name) -> Vector<Whole>;
auto gives(const String &name) -> Vector<Whole>;

auto from(const String &name) -> STRING::Hot;

auto parameters(const String &plug) -> Whole;
auto named(const String &plug, Whole parameter) -> String;

auto described(const String &plug, Whole parameter, AUDIO::PLUGIN::Control &out)
  -> Flag;

auto fed(const String &root, Whole out) -> String;

auto seat(const String &plug) -> String;
auto seat(const String &plug, const String &name) -> String;

auto root(const Vector<Whole> &kinds) -> String;
auto root(const Vector<Whole> &kinds, const String &name) -> String;

auto grow(const String &root, Whole kind) -> Whole;

auto grow(const String &root, const Port &port, Whole out) -> Whole;

auto shed(const String &root, Whole out) -> Flag;

auto admit(const String &root, Whole out, Flag on) -> Flag;

auto admitted(const String &root, Whole out) -> Whole;

auto bypass(const String &root, Whole out, Flag on) -> STRING::Hot;

auto bypassed(const String &root, Whole out) -> Flag;

auto recast(const String &root, Whole out, Whole kind) -> Flag;

auto rooted(const String &node) -> Flag;

constexpr STRING::Hot RECORDED = "record";

auto record(const String &root) -> String;
auto record(const String &root, const String &name) -> String;

auto recorder(const String &root) -> String;

auto blocked(const String &root, Whole lane) -> Flag;

auto claim(const String &node, const String &root) -> Flag;

auto claimed(const String &node) -> String;

auto quiet(const String &node, Flag on) -> Flag;

auto quieted(const String &node) -> Flag;

auto unseat(const String &node) -> Flag;

auto unseat(const String &node, Vector<String> &gone) -> Flag;

namespace TRACE {

struct Step {
  String node;
  Whole branch = 0, depth = 0;
};

auto trace(const String &root) -> Vector<Step>;

}  // namespace TRACE

auto refusal(const String &from, Whole out, const String &to, Whole in)
  -> STRING::Hot;

auto hookup(const String &from, Whole out, const String &to, Whole in) -> Flag;

auto lane(const String &node, Whole lane) -> Flag;

void main(const String &device);

constexpr STRING::Hot DEFAULT = "default";

auto surfaces() -> Vector<String>;
auto bound(const String &plug) -> Flag;

auto surface(const String &plug, const String &device) -> String;
auto surface(const String &plug, const String &device, const String &name)
  -> String;

auto unwire(const String &from, Whole out, const String &to, Whole in) -> Flag;

auto joined(const String &node, Side side, Whole port) -> Whole;

auto slackened(const String &node, Side side, Whole port) -> Whole;

auto dormant() -> const Vector<Flag> &;

auto dormant(const Wire &wire) -> Flag;

auto slacken(const String &node, Side side, Whole port) -> Whole;

auto tighten(const String &node, Side side, Whole port) -> Whole;

auto home(const String &node, Float across, Float down) -> Flag;
auto home(const String &node, const String &page, Float across, Float down)
  -> Flag;

auto berth(const String &node, const String &page) -> Berth;

auto unberth(const String &node, const String &page) -> Flag;

auto develop(
  const String &node, Whole out, Whole at, Whole pitch, Float velocity,
  Whole length) -> Flag;

auto develop(const String &node, Whole out, const AUDIO::PLUGIN::Event &edge)
  -> Flag;

auto develop(const String &node, const AUDIO::PLUGIN::Event &edge) -> Flag;

auto develop(
  const String &node, Whole out, Whole at,
  const Vector<Vector<Float>> &lanes) -> Flag;

auto dial(const String &node, Whole at, Whole parameter, Float value)
  -> STRING::Hot;

void sort();

void sort(const Vector<String> &nodes);

void owe(const String &node);

void owe();

auto owed() -> Vector<String>;

void threaded(const String &node);

auto developed(const String &node, Whole out)
  -> const Vector<AUDIO::PLUGIN::Event> &;
auto laid(const String &node, Whole out) -> const Vector<Vector<Float>> &;

auto dialled(const String &node) -> const Vector<AUDIO::PLUGIN::Event> &;

auto spare() -> Flag;

void publish();

auto adopt() -> Flag;

constexpr Whole PITCHES = 128;

constexpr Whole SLOTS = PITCHES + 32;

struct Handed {
  Whole out = NONE;
  AUDIO::PLUGIN::Event edge;
};

struct Intake {
  Whole size = 0;
  Handed handed[SLOTS];
};

auto hand(const String &node, Whole parameter, Float value) -> STRING::Hot;

auto hand(const String &node, const AUDIO::PLUGIN::Event &edge) -> Flag;

auto hand(const String &node, Whole out, const AUDIO::PLUGIN::Event &edge)
  -> Flag;

void hush(const String &node);

auto handed(const String &node) -> const Intake &;

void take();

auto taken(const String &node) -> const Intake &;

auto remembered(const String &node, Whole parameter) -> Float;

void clear(const String &node);

void listen();

auto offered(const String &node) -> const PLUG::Offer *;
auto instance(const String &node) -> void *;

void rest();

void close();

}  // namespace SOUND::GRAPH
