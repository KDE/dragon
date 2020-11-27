/*
    SPDX-FileCopyrightText: 2008 Ian Monroe <ian@monroe.nu>

    SPDX-License-Identifier: GPL-2.0-only OR GPL-3.0-only OR LicenseRef-KDE-Accepted-GPL
*/

#include "discSelectionDialog.h"

#include "codeine.h"
#include "videoWindow.h"
#include "mainWindow.h"

#include <QLabel>
#include <QListWidgetItem>
#include <QVBoxLayout>
#include <QIcon>
#include <QDialogButtonBox>
#include <QListWidget>

#include <KLocalizedString>
#include <Solid/Device>
#include <Solid/OpticalDisc>


class SolidListItem : public QListWidgetItem
{
public:
    SolidListItem( QListWidget* parent, const Solid::Device& device )
        : QListWidgetItem( parent )
        , m_device( device )
    {
        const Solid::OpticalDisc* disc = device.as<const Solid::OpticalDisc>();
        if( disc ) {
            const QString label = disc->label();
            if( label.isEmpty() ) {
                setText( contentTypesToString( disc->availableContent() ) );
            } else {
                setText( i18nc( "%1 is the disc type, %2 is the name of the disc that the user can choose. Ex. 'DVD: OfficeSpace'"
                                , "%1: %2"
                                , contentTypesToString( disc->availableContent() )
                                , label ) );
            }
            if( disc->availableContent() & Solid::OpticalDisc::Audio )
                setIcon( QIcon::fromTheme( QLatin1String( "audio-x-generic" ) ) );
            else
                setIcon( QIcon::fromTheme( QLatin1String( "video-x-generic" ) ) );
        }
    }

    Solid::Device device() const
    {
        return m_device;
    }

    static QString contentTypesToString( Solid::OpticalDisc::ContentTypes solidType )
    {
        if( solidType & Solid::OpticalDisc::VideoDvd )
            return i18nc( "Digital Versatile Disc, but keep it short", "DVD" );
        else if( solidType & ( Solid::OpticalDisc::VideoCd | Solid::OpticalDisc::SuperVideoCd ) )
            return i18n( "Video CD" );
        else if( solidType & Solid::OpticalDisc::Audio )
            return i18n( "Audio CD" );
        else
            return i18n( "Data CD" );
    }

private:
    const Solid::Device m_device;
};

DiscSelectionDialog::DiscSelectionDialog( QWidget* parent, const QList< Solid::Device >& deviceList )
    : QDialog( parent )
    , m_listWidget( new QListWidget() )
{
    setWindowTitle( i18nc("@title:window", "Select a Disc") );
    
    QLabel* questionLabel = new QLabel( i18n( "Select a disc to play." ) );
    for (const Solid::Device& device : deviceList) {
        new SolidListItem( m_listWidget, device );
    }

    QDialogButtonBox * bbox = new QDialogButtonBox(QDialogButtonBox::Ok | QDialogButtonBox::Cancel, this);

    QVBoxLayout* layout = new QVBoxLayout();
    layout->addWidget( questionLabel );
    layout->addWidget( m_listWidget );
    layout->addWidget(bbox);
    setLayout( layout );

    connect(m_listWidget, &QListWidget::itemDoubleClicked,
            this, &DiscSelectionDialog::discItemSelected);
    connect( bbox, &QDialogButtonBox::accepted, this, &DiscSelectionDialog::okClicked);
    connect( bbox, &QDialogButtonBox::rejected, this, &QDialog::deleteLater );
    //connect( bbox, &QDialogButtonBox::rejected, const_cast<QObject *>(Dragon::mainWindow()), &Dragon::MainWindow::playDisc ); // kf5 FIXME? this could have never worked
    show();
}

void DiscSelectionDialog::discItemSelected( QListWidgetItem *item )
{
    openItem( item );
    deleteLater();
}

void DiscSelectionDialog::okClicked()
{
    openItem( m_listWidget->currentItem() );
    deleteLater();
}

void DiscSelectionDialog::openItem( QListWidgetItem *item )
{
    if( item ) {
        const SolidListItem* solidItem = static_cast<SolidListItem*>( item );
        Dragon::engine()->playDisc( solidItem->device() );
    }
}
