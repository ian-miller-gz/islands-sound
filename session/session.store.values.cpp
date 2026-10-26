// SPDX-License-Identifier: AGPL-3.0-or-later
#include <format>
#include <sstream>

#include "session.store.internal.hpp"

using namespace SOUND;

namespace {

constexpr STRING::Hot VALUE = "v";
constexpr STRING::Hot OVERRIDE = "o";

}  // namespace

auto SOUND::SESSION::written(const Vector<Value> &values) -> String {
  String text;
  for (const Value &value : values)
    text += std::format(
      "{} {} {} {}\n", value.overridden ? ::OVERRIDE : ::VALUE,
      value.address.parameter, value.value, value.address.node);
  return text;
}

void SOUND::SESSION::taken(
  const String &text, Vector<Value> &values, Vector<Husk> &husks) {
  std::istringstream lines(text);
  Whole at = 0;
  for (String held; std::getline(lines, held); ++at) {
    std::istringstream line(held);
    String tag;
    line >> tag;
    if (tag != ::VALUE && tag != ::OVERRIDE) {
      husked(held, at, husks);
      continue;
    }
    Value value = {.overridden = tag == ::OVERRIDE};
    line >> value.address.parameter >> value.value;
    value.address.node = rest(line);
    if (!value.address.node.empty()) values.push_back(value);
  }
}
