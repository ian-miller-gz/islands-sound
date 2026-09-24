// SPDX-License-Identifier: AGPL-3.0-or-later
#include "../control.hpp"
#include "../graph.hpp"
#include "../history.hpp"
#include "../inventory.hpp"
#include "../render.hpp"
#include "../session.hpp"
#include "../timeline.hpp"
#include "../transport.hpp"
#include "../commands.hpp"

namespace {
using namespace SOUND;

constexpr STRING::Hot MASTER = "master";
constexpr STRING::Hot LEAD = "lead";
constexpr STRING::Hot OUTPUT = "output";
constexpr Whole FIRST = 0;
constexpr Float WEST = 10.0f;
constexpr Float NORTH = 60.0f;
constexpr Float STEP = 210.0f;
constexpr Float SOUTH = 200.0f;

void founded() {
  const Whole master = TIMELINE::bus(MASTER);
  const String root = COMMANDS::root(master, MASTER);
  TIMELINE::root(master, root);
  const String taking = GRAPH::record(root);
  const String clock = GRAPH::clock("", OUTPUT);
  GRAPH::claim(clock, root);
  GRAPH::hookup(root, ::FIRST, clock, ::FIRST);
  const Whole lead = TIMELINE::track(LEAD);
  const String led = COMMANDS::root(lead, "root");
  TIMELINE::root(lead, led);
  const String taken = GRAPH::record(led);
  GRAPH::hookup(led, ::FIRST, root, ::FIRST);
  GRAPH::home(led, ::WEST, ::NORTH);
  GRAPH::home(root, ::WEST + ::STEP, ::NORTH);
  GRAPH::home(root, led, ::WEST + ::STEP, ::NORTH);
  GRAPH::home(clock, ::WEST + ::STEP * 2.0f, ::NORTH);
  GRAPH::home(taken, ::WEST, ::NORTH + ::SOUTH);
  GRAPH::home(taking, ::WEST + ::STEP, ::NORTH + ::SOUTH);
}

}  // namespace

void SOUND::COMMANDS::fresh() {
  SESSION::adopt({});
  TRANSPORT::adopt(SESSION::held().setting);
  TIMELINE::adopt(SESSION::held().arrangement);
  INVENTORY::adopt(SESSION::held().inventory);
  HISTORY::adopt(SESSION::held().history);
  GRAPH::close();
  ::founded();
  CONTROL::adopt(SESSION::held().values);
  RENDER::VOICE::drop();
  RENDER::VOICE::claim();
}

void SOUND::COMMANDS::fresh(SHELL::Session &session) {
  fresh();
  document(session);
}
