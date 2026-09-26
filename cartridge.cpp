// SPDX-License-Identifier: AGPL-3.0-or-later
#include "control.hpp"
#include "graph.hpp"
#include "history.hpp"
#include "inventory.hpp"
#include "render.hpp"
#include "session.hpp"
#include "timeline.hpp"
#include "views.hpp"
#include "commands.hpp"

namespace SOUND {
namespace {

auto initialize(STRING::Hot assets) -> Status {
  SOUND::COMMANDS::fresh();
  return SOUND::VIEWS::open(assets);
}

void frame() {
  SOUND::COMMANDS::tend();
  SOUND::VIEWS::frame();
}

void close() {
  SOUND::RENDER::VOICE::drop();
  SOUND::VIEWS::close();
  SOUND::CONTROL::close();
  SOUND::GRAPH::close();
  SOUND::TIMELINE::close();
  SOUND::INVENTORY::close();
  SOUND::HISTORY::close();
  SOUND::SESSION::close();
}

const CARTRIDGE::Interface interface = {
  .version = CARTRIDGE::VERSION,
  .manifest = "cartridges/sound/manifest.yaml",
  .initialize = initialize,
  .frame = frame,
  .close = close,
  .commands = COMMANDS::table};

}  // namespace
}  // namespace SOUND

extern "C" auto cartridge() -> const CARTRIDGE::Interface& {
  return SOUND::interface;
}
