#!/bin/sh
set -eu

home="$(cd "$(dirname "$0")" && pwd)"
plugins="$home/plugins"

[ -d "$plugins" ] && exit 0
git clone --quiet --branch stable \
  https://github.com/ian-miller-gz/islands-sound-plugins "$plugins"
