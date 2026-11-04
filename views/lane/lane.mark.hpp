// SPDX-License-Identifier: AGPL-3.0-or-later
#pragma once
#include "../../score.hpp"
#include "../views.attention.hpp"

namespace SOUND::ARRANGEMENT::TRACK::LANE {

struct Clip;

}  // namespace SOUND::ARRANGEMENT::TRACK::LANE

namespace SOUND::VIEWS::LANE {

using Clip = SOUND::ARRANGEMENT::TRACK::LANE::Clip;

void paged(Whole kind);

auto editable(Whole kind) -> Flag;

using Mark = SOUND::VIEWS::ATTENTION::Mark;

void mark(Whole track, Whole lane);

auto marked() -> Mark;

auto edited(Whole kind) -> Mark;

auto scope(Whole clip) -> Flag;

namespace GET {

auto seen() -> Flag;

auto scoped() -> const Clip *;

auto admitted(Whole at) -> Flag;

}  // namespace GET

}  // namespace SOUND::VIEWS::LANE
