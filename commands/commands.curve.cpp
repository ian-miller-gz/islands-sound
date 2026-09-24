// SPDX-License-Identifier: AGPL-3.0-or-later
#include <cstdlib>
#include <format>

#include "../control.hpp"
#include "../graph.hpp"
#include "../inventory.hpp"
#include "commands.internal.hpp"

namespace {
using namespace SOUND;

auto counted(const String &token) -> Whole {
  return std::strtoul(token.c_str(), nullptr, 10);
}

auto measured(const String &token) -> Float {
  return std::strtof(token.c_str(), nullptr);
}

auto refused(Whole stock) -> STRING::Hot {
  const Inventory &pool = INVENTORY::held();
  if (stock >= pool.stocks.size()) return "no such stock";
  return pool.stocks[stock].kind == KIND::LOGIC ? ""
                                                : "the stock carries no curve";
}

}  // namespace

void SOUND::COMMANDS::aim(SHELL::Session &session) {
  if (session.arguments.size() < 4)
    return session.print("aim <stock> <node> <parameter>");
  const Whole stock = ::counted(session.arguments[1]);
  const STRING::Hot why = ::refused(stock);
  if (why[0] != '\0') return session.print(std::format("aim refused: {}", why));
  const Address address = {
    .node = session.arguments[2], .parameter = ::counted(session.arguments[3])};
  INVENTORY::aim(stock, address);
  session.print(std::format(
    "aimed stock {} at node {} parameter {}", stock, address.node,
    address.parameter));
}

void SOUND::COMMANDS::plot(SHELL::Session &session) {
  if (session.arguments.size() < 4)
    return session.print("plot <stock> <at> <value>");
  const Whole stock = ::counted(session.arguments[1]);
  const STRING::Hot why = ::refused(stock);
  if (why[0] != '\0')
    return session.print(std::format("plot refused: {}", why));
  const Point point = {
    .at = ::counted(session.arguments[2]),
    .value = ::measured(session.arguments[3])};
  HISTORY::take(stock, INVENTORY::held().stocks[stock]);
  INVENTORY::plot(stock, point);
  HISTORY::wrote("plot", stock, INVENTORY::held().stocks[stock]);
  session.print(std::format(
    "plotted stock {} at {} value {:.3f} points {}", stock, point.at,
    point.value, INVENTORY::held().stocks[stock].curve.points.size()));
}

void SOUND::COMMANDS::dial(SHELL::Session &session) {
  if (session.arguments.size() < 4)
    return session.print("dial <node> <parameter> <value>");
  const Address address = {
    .node = session.arguments[1], .parameter = ::counted(session.arguments[2])};
  const Float value = ::measured(session.arguments[3]);
  tend();
  const STRING::Hot why = GRAPH::hand(address.node, address.parameter, value);
  if (why[0] != '\0')
    return session.print(std::format("dial refused: {}", why));
  CONTROL::turn(address, value);
  session.print(std::format(
    "dialled node {} parameter {} to {:.3f} run {}", address.node,
    address.parameter, value, GRAPH::handed(address.node).size));
}
