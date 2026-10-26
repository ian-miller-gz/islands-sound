// SPDX-License-Identifier: AGPL-3.0-or-later
#include <algorithm>
#include <array>
#include <format>
#include <string_view>

#include "../../control.hpp"
#include "../../graph.hpp"
#include "../views.kept.hpp"
#include "params.kept.hpp"

namespace {
using namespace SOUND;
using CONTROL::MAP::Binding;
namespace KEPT = VIEWS::KEPT;

constexpr STRING::Hot MAP = "params.map";
constexpr STRING::Hot COUNT = "params.map.count";
constexpr STRING::Hot NODE = "node";
constexpr STRING::Hot PARAMETER = "parameter";
constexpr STRING::Hot NUMBER = "number";
constexpr STRING::Hot CHANNEL = "channel";

constexpr auto STANDING = std::to_array<std::string_view>(
  {GRAPH::VETTED::CLEAN, GRAPH::VETTED::PLUGIN});

Vector<Binding> skipped;

auto key(Whole n, STRING::Hot leaf) -> String {
  return KEPT::join(::MAP, n, leaf);
}

auto read(const FIELDS::Map &face, Whole n, Binding &binding) -> Flag {
  return KEPT::text(face, ::key(n, ::NODE), binding.address.node) &&
         KEPT::number(face, ::key(n, ::PARAMETER), binding.address.parameter) &&
         KEPT::number(face, ::key(n, ::NUMBER), binding.number) &&
         KEPT::number(face, ::key(n, ::CHANNEL), binding.channel);
}

auto standing(const Binding &binding) -> Flag {
  const std::string_view why =
    GRAPH::vetted(binding.address.node, binding.address.parameter);
  return std::ranges::find(::STANDING, why) != ::STANDING.end();
}

}  // namespace

void SOUND::VIEWS::RACK::keep(FIELDS::Map &face) {
  const Vector<Binding> &bindings = CONTROL::MAP::held();
  face[::COUNT] = Whole(bindings.size());
  for (Whole n = 0; n < bindings.size(); ++n) {
    face[::key(n, ::NODE)] = bindings[n].address.node;
    face[::key(n, ::PARAMETER)] = bindings[n].address.parameter;
    face[::key(n, ::NUMBER)] = bindings[n].number;
    face[::key(n, ::CHANNEL)] = bindings[n].channel;
  }
}

void SOUND::VIEWS::RACK::restore(const FIELDS::Map &face) {
  Whole count = 0;
  KEPT::number(face, ::COUNT, count);
  Vector<Binding> bindings;
  ::skipped.clear();
  for (Whole n = 0; n < count; ++n) {
    Binding binding;
    if (!::read(face, n, binding)) continue;
    (::standing(binding) ? bindings : ::skipped).push_back(binding);
  }
  CONTROL::MAP::adopt(bindings);
}

void SOUND::VIEWS::RACK::skipped(SHELL::Session &session) {
  for (const Binding &binding : ::skipped)
    session.print(std::format(
      "skipped {} {} cc {}", binding.address.node, binding.address.parameter,
      binding.number));
}
