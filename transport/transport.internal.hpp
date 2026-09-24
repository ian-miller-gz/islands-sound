// SPDX-License-Identifier: AGPL-3.0-or-later
#pragma once
#include <threads.hpp>

#include "transport.hpp"

namespace SOUND::TRANSPORT {

struct Reading {
  Whole position = 0;
  Whole locates = 0;
};

struct Mirror {
  Reading pair[2];
  THREADS::Shared<Whole> face{0};
  void publish(const Reading &reading);
  auto read() -> Reading;
};

struct Deck {
  Statement pair[2];
  THREADS::Shared<Whole> face{0};
  void publish(const Statement &want);
  auto read() -> Statement;
};

struct Clockwork {
  Setting setting;
  Statement want;
  Deck deck;
  Mirror mirror;
  Statement taken;
  Whole position = 0;
  Whole started = 0;
  Whole locates = 0;
};

auto clockwork() -> Clockwork &;

void state();

}  // namespace SOUND::TRANSPORT
