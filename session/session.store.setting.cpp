// SPDX-License-Identifier: AGPL-3.0-or-later
#include <format>
#include <sstream>

#include "session.store.internal.hpp"

using namespace SOUND;

auto SOUND::SESSION::written(const Setting &setting) -> String {
  String text;
  for (const Tempo &row : setting.tempos)
    text += std::format("b {} {}\n", row.at, row.tempo);
  for (const Metre &row : setting.metres)
    text += std::format("m {} {} {}\n", row.at, row.numerator, row.denominator);
  for (const Key &row : setting.keys)
    text += std::format("k {} {} {}\n", row.at, row.tonic, row.scale);
  for (const Tag &row : setting.tags)
    text += std::format("t {} {}\n", row.at, row.text);
  for (const Span &row : setting.loops)
    text += std::format(
      "l {} {}{}\n", row.from, row.to,
      row.text.empty() ? String() : " " + row.text);
  if (setting.ends.to > setting.ends.from)
    text += std::format("e {} {}\n", setting.ends.from, setting.ends.to);
  return text;
}

void SOUND::SESSION::taken(
  const String &text, Setting &setting, Vector<Husk> &husks) {
  Vector<Tempo> tempos;
  Vector<Metre> metres;
  Vector<Key> keys;
  std::istringstream lines(text);
  setting.tags.clear();
  setting.loops.clear();
  setting.ends = Span{};
  Whole at = 0;
  for (String held; std::getline(lines, held); ++at) {
    std::istringstream line(held);
    String tag;
    line >> tag;
    if (tag == "b") {
      Tempo row;
      line >> row.at >> row.tempo;
      tempos.push_back(row);
    } else if (tag == "m") {
      Metre row;
      line >> row.at >> row.numerator >> row.denominator;
      metres.push_back(row);
    } else if (tag == "k") {
      Key row;
      line >> row.at >> row.tonic;
      row.scale = rest(line);
      keys.push_back(row);
    } else if (tag == "t") {
      Tag row;
      line >> row.at;
      row.text = rest(line);
      setting.tags.push_back(row);
    } else if (tag == "l") {
      Span row;
      line >> row.from >> row.to;
      row.text = rest(line);
      setting.loops.push_back(row);
    } else if (tag == "e") {
      line >> setting.ends.from >> setting.ends.to;
    } else {
      husked(held, at, husks);
    }
  }
  if (!tempos.empty()) setting.tempos = tempos;
  if (!metres.empty()) setting.metres = metres;
  if (!keys.empty()) setting.keys = keys;
}
