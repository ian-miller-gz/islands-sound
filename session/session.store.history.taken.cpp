// SPDX-License-Identifier: AGPL-3.0-or-later
#include "session.store.internal.hpp"

using namespace SOUND;

namespace {

void borrowed(
  const String &facts, const Vector<Whole> &where, Graph &wiring,
  Vector<Husk> &husks) {
  const Whole from = husks.size();
  SESSION::taken(facts, wiring, husks);
  for (Whole one = from; one < husks.size(); ++one)
    if (husks[one].at < where.size()) husks[one].at = where[husks[one].at];
}

auto opened(std::istringstream &line) -> Edit {
  Edit edit;
  Whole verb = Edit::ADDED;
  line >> edit.arc >> verb;
  edit.verb = verb;
  edit.act = SESSION::rest(line);
  return edit;
}

}  // namespace

void SOUND::SESSION::taken(
  const String &text, History &history, Vector<Husk> &husks) {
  String facts;
  Vector<Whole> where;
  std::istringstream lines(text);
  Whole at = 0;
  for (String held; std::getline(lines, held); ++at) {
    std::istringstream line(held);
    String tag;
    line >> tag;
    if (tag == "x") {
      if (!history.edits.empty())
        ::borrowed(facts, where, history.edits.back().graph, husks);
      facts.clear();
      where.clear();
      history.edits.push_back(::opened(line));
    } else if (history.edits.empty()) {
      continue;
    } else if (!edited(tag, line, history.edits.back())) {
      facts += held + "\n";
      where.push_back(at);
    }
  }
  if (!history.edits.empty())
    ::borrowed(facts, where, history.edits.back().graph, husks);
}
