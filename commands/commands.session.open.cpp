// SPDX-License-Identifier: AGPL-3.0-or-later
#include <format>

#include "../control.hpp"
#include "../graph.hpp"
#include "../render.hpp"
#include "../history.hpp"
#include "../inventory.hpp"
#include "../session.hpp"
#include "../timeline.hpp"
#include "../transport.hpp"
#include "../views.hpp"
#include "../commands.hpp"
#include "commands.internal.hpp"

using namespace SOUND;

namespace {

void reread() {
  for (Whole index = 0; index < INVENTORY::held().stocks.size(); ++index) {
    const Stock &stock = INVENTORY::held().stocks[index];
    if (stock.kind != KIND::AUDIO || stock.take.path.empty()) continue;
    INVENTORY::take(index, String(stock.take.path));
  }
}

void reasserted() {
  for (const Value &value : SESSION::held().values)
    GRAPH::hand(value.address.node, value.address.parameter, value.value);
}

auto said(const COMMANDS::Gathered &told) -> String {
  String lines;
  for (const String &line : told.lines)
    lines += (lines.empty() ? "" : "\n") + line;
  return lines;
}

}  // namespace

auto SOUND::COMMANDS::open(const String &name) -> String {
  const STRING::Hot refused = SESSION::open(name);
  if (refused[0] != '\0') return std::format("open refused: {}", refused);
  RENDER::VOICE::drop();
  Gathered told;
  TRANSPORT::adopt(SESSION::held().setting);
  TIMELINE::adopt(SESSION::held().arrangement);
  INVENTORY::adopt(SESSION::held().inventory);
  HISTORY::adopt(SESSION::held().history);
  ::reread();
  GRAPH::close();
  stand(told, SESSION::held().graph);
  CONTROL::adopt(SESSION::held().values);
  ::reasserted();
  if (SESSION::rewritten() != 0)
    told.print(std::format("rewritten from schema {}", SESSION::rewritten()));
  const Vector<Husk> husks = SESSION::husks();
  if (!husks.empty()) told.print(std::format("held {} husks", husks.size()));
  FIELDS::Map face;
  if (SESSION::kept(SESSION::held().name, face)) VIEWS::keep(face);
  told.print(std::format("opened {}", SESSION::held().name));
  return ::said(told);
}
