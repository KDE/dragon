<!--
    SPDX-License-Identifier: CC0-1.0
    SPDX-FileCopyrightText: 2025 Harald Sitter <sitter@kde.org>
-->
# <img src="misc/48-apps-dragonplayer.png" width="48"/> Dragon Player
Dragon Player is a simple video and audio player focusing on basic functionality with high reliability and a clean and uncluttered user interface.

![Screenshot of Dragon Player](https://cdn.kde.org/screenshots/dragonplayer/dragonplayer.png)
## Links
* Issues: https://bugs.kde.org/enter_bug.cgi?product=dragonplayer
* Project page: https://invent.kde.org/multimedia/dragon

## Installing:

```bash
git clone https://invent.kde.org/multimedia/dragon.git
cd dragon
flatpak-builder build --user --install-deps-from=flathub --force-clean --ccache --install .flatpak-manifest.json
flatpak run org.kde.dragonplayer
```
