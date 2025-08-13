<!--
    SPDX-License-Identifier: CC0-1.0
    SPDX-FileCopyrightText: 2025 Harald Sitter <sitter@kde.org>
-->

To build and run

```bash
git clone https://invent.kde.org/multimedia/dragon.git
cd dragon
flatpak-builder build --user --install-deps-from=flathub --force-clean --ccache --install .flatpak-manifest.json
flatpak run org.kde.dragonplayer
```
