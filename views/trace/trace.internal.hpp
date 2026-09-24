// SPDX-License-Identifier: AGPL-3.0-or-later
#pragma once
#include "trace.face.hpp"

namespace SOUND::VIEWS::TRACE {

constexpr STRING::Hot NAME = "trace";
constexpr STRING::Hot LIST = "tracks";
constexpr STRING::Hot NODED = "trace.face.nodes";
constexpr STRING::Hot LANED = "trace.face.lanes";
constexpr STRING::Hot EMPTY = "trace.empty";
constexpr STRING::Hot KINDED = "trace.kind";
constexpr STRING::Hot GROW = "trace.grow";

auto attended() -> Whole;

void laning(Flag on);

void lanes();
void lanes(SHELL::Session &session);

auto kinded(GUI::Handle page, STRING::Hot door, STRING::Hot whose, Whole &kind)
  -> Flag;

void edit(Whole track, Whole lane);

void editing();
void editing(SHELL::Session &session);

}  // namespace SOUND::VIEWS::TRACE
