// SPDX-License-Identifier: GPL-2.0-only OR GPL-3.0-only OR LicenseRef-KDE-Accepted-GPL
// SPDX-FileCopyrightText: 2023 Harald Sitter <sitter@kde.org>

#pragma once

#include <QString>

#include "version.h"

namespace Dragon
{
constexpr auto desktopFileName = QLatin1String("org.kde.dragonplayer");
constexpr auto version = QLatin1String(DRAGON_VERSION_STRING);
} // namespace Dragon
