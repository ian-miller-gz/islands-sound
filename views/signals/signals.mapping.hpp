// SPDX-License-Identifier: AGPL-3.0-or-later
#pragma once
#include "../../inventory.hpp"
#include "signals.internal.hpp"

namespace SOUND::VIEWS::SIGNALS {

auto hung() -> Flag;

void hang(const Vector<Offer> &rows);

void aiming();
void aiming(SHELL::Session &session);

void aim(const Address &address);

}  // namespace SOUND::VIEWS::SIGNALS
