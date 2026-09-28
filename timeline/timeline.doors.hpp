// SPDX-License-Identifier: AGPL-3.0-or-later
#pragma once
#include "../arrangement.hpp"
#include "../inventory.hpp"
#include "../kind.hpp"

namespace SOUND::TIMELINE {

struct Sounding {
  Whole track = NONE;
  String root;
  Whole lane = 0;
  Whole at = 0, pitch = 0, length = 0;
  Float velocity = 0.0f;
};

struct Laid {
  Whole track = NONE;
  String root;
  Whole lane = 0;
  Whole at = 0;
  Vector<Vector<Float>> lanes;
};

struct Dialled {
  Whole track = NONE;
  Address address;
  Whole at = 0;
  Float value = 0.0f;
};

struct Sent {
  Whole track = NONE;
  String root;
  Whole lane = 0;
  Whole kind = KIND::CONTROL;
  Whole at = 0;
  Whole number = 0;
  Float value = 0.0f;
};

struct Governed {
  Flag stated = false;
  Float value = 0.0f;
};

auto held() -> const Arrangement &;

void adopt(const Arrangement &tracks);

auto track(const String &name) -> Whole;

auto bus(const String &name) -> Whole;

auto bus(Whole track, Flag on) -> Flag;

auto kinds(Whole track) -> Vector<Whole>;

auto name(Whole track, const String &called) -> Flag;

auto name(Whole track, Whole lane, const String &called) -> Flag;

auto kind(Whole track, Whole lane, Whole kind) -> Flag;

auto drop(Whole track) -> Flag;

auto track(Whole at, const ARRANGEMENT::Track &row) -> Flag;

auto lane(Whole track, Whole kind) -> Whole;

auto lane(Whole track, Whole at, const ARRANGEMENT::TRACK::Lane &row) -> Flag;

auto unlane(Whole track, Whole lane) -> Flag;

auto root(Whole track, const String &node) -> Flag;

auto map(Whole track, Whole lane, const String &plugin) -> Flag;

auto place(
  const Inventory &pool, Whole track, Whole lane,
  const ARRANGEMENT::TRACK::LANE::Clip &span) -> Whole;
auto place(
  const Inventory &pool, Whole track, Whole lane, Whole placement,
  const ARRANGEMENT::TRACK::LANE::Clip &span) -> Whole;

auto crossed(Whole track, Whole lane, Whole opens, Whole closes, Whole ignoring)
  -> Flag;

auto span(
  Whole track, Whole lane, Whole placement,
  const ARRANGEMENT::TRACK::LANE::Clip &shaped) -> Flag;

auto lift(Whole track, Whole lane, Whole placement) -> Flag;

auto ended() -> Whole;

auto spans(Whole track, Whole kind) -> Vector<ARRANGEMENT::TRACK::LANE::Clip>;

auto covered(Whole track, Whole kind, Whole at)
  -> ARRANGEMENT::TRACK::LANE::Clip;

auto bump(
  Whole track, Whole lane, Whole opens, Whole closes,
  Vector<ARRANGEMENT::TRACK::LANE::Clip> &moved,
  Vector<ARRANGEMENT::TRACK::LANE::Clip> &deleted) -> Flag;

auto restore(Whole track, Whole lane, Whole outtake, Whole at) -> Whole;

auto roll(
  const Inventory &pool, Whole from, Whole frames, const Vector<Tempo> &tempos,
  const Vector<String> &claims) -> Vector<Sounding>;

auto lay(
  const Inventory &pool, Whole from, Whole frames,
  const Vector<String> &claims) -> Vector<Laid>;

auto send(
  const Inventory &pool, Whole from, Whole frames, Whole grain,
  const Vector<String> &claims) -> Vector<Sent>;

auto dial(
  const Inventory &pool, Whole from, Whole frames, Whole grain,
  const Vector<String> &claims) -> Vector<Dialled>;

auto valued(const Vector<Turn> &turns, Whole number, Whole at) -> Float;

auto govern(const Inventory &pool, const Address &address, Whole at)
  -> Governed;

void close();

auto wrapped(Whole from, Whole elapsed, Whole carried) -> Whole;

auto opened(Whole at, Whole from, Whole carried) -> Whole;

}  // namespace SOUND::TIMELINE
