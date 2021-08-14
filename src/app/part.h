/*
    SPDX-FileCopyrightText: 2005 Max Howell <max.howell@methylblue.com>
    SPDX-FileCopyrightText: 2007 Ian Monroe <ian@monroe.nu>

    SPDX-License-Identifier: GPL-2.0-only OR GPL-3.0-only OR LicenseRef-KDE-Accepted-GPL
*/

#ifndef DRAGONPLAYER_PART_H
#define DRAGONPLAYER_PART_H

#include "codeine.h"

#include <QList>

#include <KParts/StatusBarExtension>
#include <KParts/ReadOnlyPart>
#include <QUrl>
#include <Phonon/MediaSource>


namespace Dragon
{
class PlayAction;

class Part : public KParts::ReadOnlyPart
{
    Q_OBJECT
public:
    Part(QWidget* parentWidget, QObject* parent, const KPluginMetaData& metaData, const QVariantList& /*args*/);

    bool closeUrl() override;

public Q_SLOTS:
    bool openUrl( const QUrl& ) override;

private Q_SLOTS:
    void engineStateChanged( Phonon::State state );
    void videoContextMenu( const QPoint & pos );

private:
    QUrl m_url;
    KParts::StatusBarExtension *m_statusBarExtension;
    Dragon::PlayAction* m_playPause;

    QStatusBar *statusBar() { return m_statusBarExtension->statusBar(); }
};
}

#endif
