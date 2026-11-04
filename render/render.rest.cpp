// SPDX-License-Identifier: AGPL-3.0-or-later
#include <algorithm>

#include "../control.hpp"
#include "../transport.hpp"
#include "render.internal.hpp"
#include "render.rest.hpp"

namespace {
using namespace SOUND;
using RENDER::REST::Spanned;

auto addressed(const Vector<Spanned> &spans) -> Vector<Address> {
  Vector<Address> addresses;
  for (const Spanned &one : spans)
    if (std::none_of(
          addresses.begin(), addresses.end(), [&](const Address &held) {
            return CONTROL::same(held, one.address);
          }))
      addresses.push_back(one.address);
  return addresses;
}

auto ends(const Vector<Spanned> &spans, const Address &address)
  -> Vector<Whole> {
  Vector<Whole> ends;
  if (!RENDER::REST::covered(spans, address, 0)) ends.push_back(0);
  for (const Spanned &one : spans)
    if (
      CONTROL::same(one.address, address) &&
      !RENDER::REST::covered(spans, address, one.closes) &&
      std::find(ends.begin(), ends.end(), one.closes) == ends.end())
      ends.push_back(one.closes);
  return ends;
}

auto resting(const Address &address) -> Float {
  return CONTROL::resting(
    address, GRAPH::remembered(address.node, address.parameter));
}

auto instant() -> Whole {
  const TRANSPORT::Marker marker = TRANSPORT::marker();
  return marker.playing ? marker.position : TRANSPORT::mark();
}

}  // namespace

void SOUND::RENDER::rested(const Vector<String> &claims) {
  const Vector<Spanned> spans = REST::spanned();
  for (const Address &address : ::addressed(spans)) {
    if (std::find(claims.begin(), claims.end(), address.node) == claims.end())
      continue;
    if (CONTROL::overridden(address)) continue;
    const Float rest = ::resting(address);
    for (const Whole at : ::ends(spans, address))
      GRAPH::dial(address.node, at, address.parameter, rest);
  }
}

void SOUND::RENDER::rest() {
  const Whole at = ::instant();
  for (const Address &address : ::addressed(REST::spanned())) {
    if (CONTROL::overridden(address)) continue;
    const TIMELINE::Governed governed = RENDER::governed(address, at);
    GRAPH::hand(
      address.node, address.parameter,
      governed.stated ? governed.value : ::resting(address));
  }
}
