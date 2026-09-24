// SPDX-License-Identifier: AGPL-3.0-or-later
#pragma once
#include "../score.hpp"

namespace SOUND {

struct Setting {
  Vector<Tempo> tempos{Tempo{}};
  Vector<Metre> metres{Metre{}};
  Vector<Key> keys{Key{}};
  Vector<Tag> tags;
  Vector<Span> loops;
  Span ends;
  Flag looping = false;
};

}  // namespace SOUND

namespace SOUND::TRANSPORT {

constexpr Float SLOWEST = 20.0f;
constexpr Float FASTEST = 300.0f;

struct Marker {
  Whole position = 0;
  Flag playing = false;
  Whole locates = 0;
};

struct Statement {
  Flag playing = false;
  Whole mark = 0, locates = 0;
  Whole from = 0, to = 0;
  Flag looping = false;
};

void play();
void stop();
void locate(Whole frame);

void pace(Float tempo);
void pace(Whole at, Float tempo);
void metre(Whole at, Whole numerator, Whole denominator);
void key(Whole tonic, const String &scale);
void key(Whole at, Whole tonic, const String &scale);

auto unpace(Whole at) -> Flag;
auto unmetre(Whole at) -> Flag;
auto unkey(Whole at) -> Flag;

auto tag(Whole at, const String &text) -> Whole;

auto untag(Whole index) -> Flag;

auto loop(Whole from, Whole to) -> Whole;

auto unloop(Whole index) -> Flag;

auto loop(Whole index, const String &text) -> Flag;

void loop(Flag looping);

void ends(Whole from, Whole to);

auto ahead() -> Whole;

auto behind() -> Whole;

auto cycled() -> Span;

auto cycled(Whole at) -> Span;

void tend();

auto held() -> const Setting &;

auto paced() -> Float;
auto paced(Whole at) -> Float;
auto metred() -> Metre;
auto metred(Whole at) -> Metre;
auto keyed() -> Key;
auto keyed(Whole at) -> Key;
auto bar() -> Whole;
auto bar(Whole at) -> Whole;

struct Section {
  Whole at = 0, span = 0, bar = 0, bars = 0;
  Whole numerator = METRE, denominator = METRE;
};

auto sections() -> Vector<Section>;
auto framed(const Vector<Section> &sections) -> Vector<Section>;

auto mark() -> Whole;

auto marker() -> Marker;

auto seconds(Whole frames) -> Float;

void adopt(const Setting &setting);

void detach();

void advance(Whole frames);

auto block() -> const Statement &;
auto started() -> Whole;

}  // namespace SOUND::TRANSPORT
