// SPDX-License-Identifier: AGPL-3.0-or-later
#pragma once
#include "../../graph.hpp"
#include "trace.face.hpp"

namespace SOUND::VIEWS::TRACE {

constexpr STRING::Hot NAME = "trace";
constexpr STRING::Hot LIST = "tracks";
constexpr STRING::Hot NODED = "trace.face.nodes";
constexpr STRING::Hot LANED = "trace.face.lanes";
constexpr STRING::Hot CLIPPED = "trace.face.clips";
constexpr STRING::Hot EMPTY = "trace.empty";
constexpr STRING::Hot KINDED = "trace.kind";
constexpr STRING::Hot GROW = "trace.grow";

auto attended() -> Whole;

auto walked() -> Vector<GRAPH::TRACE::Step>;

void aimed(GUI::Handle page);
void aimed(SHELL::Session &session);

void turned(GUI::Handle page);

void read();

void nodes();

struct Clipped {
  Whole clip = NONE;
  String name, place;
  Float bars = 0.0f;
  Flag scoped = false;
};

auto clipped() -> Vector<Clipped>;

void clips();
void clips(SHELL::Session &session);

auto adding() -> Whole;

void lanes();
void lanes(SHELL::Session &session);

void marked(GUI::Handle page);

void asked(GUI::Handle page);

auto kinded(GUI::Handle page, STRING::Hot door, STRING::Hot whose, Whole &kind)
  -> Flag;

void edit(Whole track, Whole lane);

void editing();
void editing(SHELL::Session &session);

}  // namespace SOUND::VIEWS::TRACE
