// SPDX-License-Identifier: AGPL-3.0-or-later
#include <algorithm>
#include <cstdlib>
#include <filesystem>
#include <format>

#include "../graph.hpp"
#include "../kind.hpp"
#include "../master.hpp"
#include "../render.hpp"
#include "../score.hpp"
#include "../session.hpp"
#include "../timeline.hpp"
#include "../transport.hpp"
#include "commands.internal.hpp"

namespace {
using namespace SOUND;

constexpr STRING::Hot HOME = "exports";
constexpr STRING::Hot UNNAMED = "unnamed";
constexpr STRING::Hot SUFFIX = ".wav";

auto spanned() -> Span {
  const Setting &setting = TRANSPORT::held();
  if (setting.ends.to > setting.ends.from)
    return {
      SCORE::framed(setting.ends.from, setting.tempos),
      SCORE::framed(setting.ends.to, setting.tempos)};
  return {0, TIMELINE::ended()};
}

auto claimed(const Node &node, const String &root) -> Flag {
  return node.name == root || node.claim == root;
}

auto outgoing(const String &root) -> Vector<String> {
  const Graph &graph = GRAPH::held();
  Vector<String> sources;
  for (const Wire &wire : graph.wires) {
    const Whole from = GRAPH::at(wire.from), to = GRAPH::at(wire.to);
    if (from == NONE || to == NONE) continue;
    const Node &source = graph.nodes[from];
    if (!::claimed(source, root) || ::claimed(graph.nodes[to], root)) continue;
    if (
      wire.out >= source.outs.size() ||
      source.outs[wire.out].kind != KIND::AUDIO)
      continue;
    if (std::find(sources.begin(), sources.end(), source.name) == sources.end())
      sources.push_back(source.name);
  }
  if (sources.empty()) sources.push_back(root);
  return sources;
}

auto named(const ARRANGEMENT::Track &track) -> String {
  const String session = SESSION::held().name;
  return std::format(
    "{}/{}.{}{}", ::HOME, session.empty() ? String(::UNNAMED) : session,
    track.name, ::SUFFIX);
}

auto counted(const String &word) -> Whole {
  return std::strtoul(word.c_str(), nullptr, 10);
}

}  // namespace

auto SOUND::COMMANDS::exported(Whole track, const String &path) -> Export {
  const Vector<ARRANGEMENT::Track> &tracks = TIMELINE::held().tracks;
  if (track >= tracks.size()) return {.refusal = "no such track"};
  const Span span = ::spanned();
  if (span.to <= span.from) return {.refusal = "nothing placed to render"};
  Vector<Vector<Float>> lanes;
  RENDER::spool();
  if (!MASTER::bounce(
        ::outgoing(tracks[track].root), span.from, span.to - span.from, lanes))
    return {.refusal = "the graph would not render"};
  const String where = path.empty() ? ::named(tracks[track]) : path;
  std::error_code error;
  std::filesystem::create_directories(
    std::filesystem::path(where).parent_path(), error);
  if (!MASTER::WAVE::write(where, RATE, lanes))
    return {.refusal = std::format("{} would not take the file", where)};
  return {.path = where, .frames = span.to - span.from};
}

void SOUND::COMMANDS::exporting(SHELL::Session &session) {
  if (session.arguments.size() < 2)
    return session.print("export <track> [path]");
  const Whole track = ::counted(session.arguments[1]);
  const Export made = COMMANDS::exported(
    track, session.arguments.size() > 2 ? session.arguments[2] : String());
  if (!made.refusal.empty())
    return session.print(std::format("export refused: {}", made.refusal));
  session.print(std::format(
    "exported {} {} {} frames {}", track, TIMELINE::held().tracks[track].name,
    made.frames, made.path));
}
