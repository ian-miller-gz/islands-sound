// SPDX-License-Identifier: AGPL-3.0-or-later
#pragma once
#include <island/gui/ladder.hpp>
#include <island/gui/stroke.hpp>

#include "../../arrangement.hpp"
#include "../../peaks.hpp"
#include "../lane/lane.face.hpp"
#include "wave.face.hpp"

namespace SOUND::VIEWS::WAVE {

constexpr STRING::Hot NAME = "wave";
constexpr STRING::Hot RULER = "ruler";
constexpr STRING::Hot PLATE = "waveclip";
constexpr STRING::Hot PEAK = "wave";
constexpr STRING::Hot ZERO = "wavezero";
constexpr STRING::Hot SLIP = "slip";
constexpr STRING::Hot PANEL = "panel";
constexpr STRING::Hot LABEL = "label";
constexpr STRING::Hot GRIP = "wavefade";
constexpr STRING::Hot LINED = "waveline";
constexpr STRING::Hot HANDLE = "wavestretch";
constexpr STRING::Hot STRETCHED = "stretched";
constexpr STRING::Hot ONSET = "waveonset";

constexpr Float ORIGIN = 0.0f;
constexpr Float HALF = 0.5f;
constexpr Float HAIR = 1.0f;
constexpr Float COLUMN = 2.0f;

using Clip = ARRANGEMENT::TRACK::LANE::Clip;

auto board() -> String;
auto strip() -> String;

auto laned() -> LANE::Mark;
auto placed() -> Vector<Clip>;

auto scaled() -> Float;
auto window() -> Float;
auto deep() -> Float;
auto least() -> Float;

void clips();
void clips(SHELL::Session &session);
void peaks();
auto peak(const Clip &clip, Whole opens, Whole frames) -> PEAKS::Peak;
void ruler();
auto rung() -> Whole;
auto marks() -> Vector<GUI::SAC::LADDER::Mark>;
void hands();
void retouch(Whole row, const Clip &was, const Clip &now, STRING::Hot act);

auto grip(Whole row, Whole end) -> String;
auto gripped() -> GUI::Extent;
auto west(const Clip &clip, Whole end) -> Float;
auto shown(const Clip &clip) -> Flag;
void grips();
auto fade(const GUI::SAC::STROKE::Gesture &gesture) -> Flag;
void menu();
void lines();
auto onsets(const Clip &clip, Float sense) -> Vector<Whole>;
void onsets();
void onsets(SHELL::Session &session);

auto handle(Whole row) -> String;
auto handled() -> GUI::Extent;
auto held() -> Whole;
auto landed(Whole row, const Clip &clip) -> Clip;
auto stretch(const GUI::SAC::STROKE::Gesture &gesture) -> Flag;
void handles();
void say();
void shifts();
auto chosen() -> Whole;
void choose(const GUI::SAC::STROKE::Gesture &gesture);
void keyed();
void rows();
void rows(SHELL::Session &session);
auto worded(const Clip &clip) -> String;

auto overview() -> Overview;

namespace SHED {

void clips();
void peaks();
void ruler();
void grips();
void lines();
void handles();
void reading();
void shifts();
void onsets();
void band();

}  // namespace SHED

}  // namespace SOUND::VIEWS::WAVE
