// SPDX-License-Identifier: GPL-2.0-only OR GPL-3.0-only OR LicenseRef-KDE-Accepted-GPL
// SPDX-FileCopyrightText: 2025 Harald Sitter <sitter@kde.org>

#include "sandbox.h"

extern "C" {
#include <libavcodec/avcodec.h>
}

#include <KSandbox>

using namespace Qt::StringLiterals;

bool Sandbox::isInside() const
{
    return KSandbox::isInside();
}

bool Sandbox::hasFfmpegFull() const
{
    if (!KSandbox::isFlatpak()) {
        const auto codec = avcodec_find_decoder_by_name("h264");
        return codec != nullptr;
    }

    return QDir(u"/app/lib/ffmpeg/"_s).entryList({u"libav*.so*"_s}, QDir::Files | QDir::NoDotAndDotDot).size() > 0;
}
