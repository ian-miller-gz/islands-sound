// SPDX-License-Identifier: AGPL-3.0-or-later
#include <cstdlib>
#include <format>
#include <sstream>

#include "../transport.hpp"
#include "commands.internal.hpp"

namespace {
using namespace SOUND;

auto placed(const Change &change) -> Whole {
  return Whole(std::strtoul(change.at.c_str(), nullptr, 10));
}

}  // namespace

auto SOUND::COMMANDS::CHANGE::paced(const Change &change) -> String {
  const Whole at = ::placed(change);
  for (const Tempo &row : TRANSPORT::held().tempos)
    if (row.at == at) return std::format("{:g}", row.tempo);
  return String();
}

auto SOUND::COMMANDS::CHANGE::paced(const Change &change, const String &value)
  -> Flag {
  const Whole at = ::placed(change);
  if (value.empty()) return TRANSPORT::unpace(at);
  TRANSPORT::pace(at, std::strtof(value.c_str(), nullptr));
  return true;
}

auto SOUND::COMMANDS::CHANGE::metred(const Change &change) -> String {
  const Whole at = ::placed(change);
  for (const Metre &row : TRANSPORT::held().metres)
    if (row.at == at)
      return std::format("{}/{}", row.numerator, row.denominator);
  return String();
}

auto SOUND::COMMANDS::CHANGE::metred(const Change &change, const String &value)
  -> Flag {
  const Whole at = ::placed(change);
  if (value.empty()) return TRANSPORT::unmetre(at);
  const auto slash = value.find('/');
  if (slash == String::npos) return false;
  TRANSPORT::metre(
    at, Whole(std::strtoul(value.c_str(), nullptr, 10)),
    Whole(std::strtoul(value.c_str() + slash + 1, nullptr, 10)));
  return true;
}

auto SOUND::COMMANDS::CHANGE::keyed(const Change &change) -> String {
  const Whole at = ::placed(change);
  for (const Key &row : TRANSPORT::held().keys)
    if (row.at == at) return std::format("{} {}", row.tonic, row.scale);
  return String();
}

auto SOUND::COMMANDS::CHANGE::keyed(const Change &change, const String &value)
  -> Flag {
  const Whole at = ::placed(change);
  if (value.empty()) return TRANSPORT::unkey(at);
  std::istringstream spoken(value);
  Whole tonic = 0;
  String scale;
  spoken >> tonic >> scale;
  TRANSPORT::key(at, tonic, scale);
  return true;
}
