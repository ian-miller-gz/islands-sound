// SPDX-License-Identifier: AGPL-3.0-or-later
#include <cstdlib>
#include <format>

#include "../inventory.hpp"
#include "../kind.hpp"
#include "../score.hpp"
#include "../commands.hpp"

namespace {
using namespace SOUND;

auto counted(const String &token) -> Whole {
  return std::strtoul(token.c_str(), nullptr, 10);
}

auto measured(const Stock &stock) -> String {
  if (stock.kind == KIND::AUDIO)
    return std::format(
      "frames {}", stock.take.lanes.empty() ? 0 : stock.take.lanes[0].size());
  if (stock.kind == KIND::LOGIC)
    return std::format(
      "points {} node {} parameter {}", stock.curve.points.size(),
      stock.curve.address.node.empty() ? String("none")
                                       : stock.curve.address.node,
      stock.curve.address.parameter);
  if (stock.kind == KIND::CONTROL)
    return std::format("turns {}", stock.turns.size());
  if (stock.kind == KIND::PROGRAM)
    return std::format("choices {}", stock.choices.size());
  return std::format("notes {}", stock.score.notes.size());
}

auto stocked(Whole index, const Stock &stock) -> String {
  return std::format(
    "stock {} {} {} {}", index, KIND::spoken(stock.kind), stock.name,
    ::measured(stock));
}

}  // namespace

void SOUND::COMMANDS::stocks(SHELL::Session &session) {
  const Inventory &pool = INVENTORY::held();
  session.print(std::format("stocks {}", pool.stocks.size()));
  for (Whole index = 0; index < pool.stocks.size(); ++index)
    session.print(::stocked(index, pool.stocks[index]));
}

void SOUND::COMMANDS::stock(SHELL::Session &session) {
  if (session.arguments.size() < 2)
    return session.print(
      "stock <name> [audio|notes|control|program|logic|data]");
  const Whole kind = session.arguments.size() > 2
                       ? KIND::meant(session.arguments[2])
                       : KIND::NOTES;
  if (kind == NONE) return session.print("stock refused: no such kind");
  const STRING::Hot refused = INVENTORY::refuses(kind);
  if (refused[0] != '\0')
    return session.print(std::format("stock refused: {}", refused));
  const Whole index = INVENTORY::add(session.arguments[1], kind);
  if (index == NONE) return session.print("stock refused: no name");
  session.print(::stocked(index, INVENTORY::held().stocks[index]));
}

void SOUND::COMMANDS::take(SHELL::Session &session) {
  if (session.arguments.size() < 3) return session.print("take <stock> <path>");
  const Whole index = ::counted(session.arguments[1]);
  const STRING::Hot refused = INVENTORY::take(index, session.arguments[2]);
  if (refused[0] != '\0')
    return session.print(std::format("take refused: {}", refused));
  session.print(::stocked(index, INVENTORY::held().stocks[index]));
}
