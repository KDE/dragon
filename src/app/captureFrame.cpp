// (C) 2005 Max Howell (max.howell@methylblue.com)
// See COPYING file for licensing information

#include "debug.h"
#include <kfiledialog.h>
#include <kpreviewwidgetbase.h>
#include <kpushbutton.h>
#include <kstatusbar.h>
#include <kstdguiitem.h>
#include "mainWindow.h"
#include "mxcl.library.h"
#include <qdialog.h>
#include <qhbox.h>
#include <qlabel.h>
#include <qimage.h>
#include <qlayout.h>
#include <qpainter.h>
#include <qstringlist.h>
#include "theStream.h"
#include "xineEngine.h"
#include <xine.h>


namespace Codeine {

class FrameCapturePreview : public KPreviewWidgetBase
{
   QImage m_frame;

   virtual void showPreview( const KURL& ) {}
   virtual void clearPreview() {}

   virtual void paintEvent( QPaintEvent* )
   {
      QPainter painter( this );

      const uint h = int( double(m_frame.height()) / m_frame.width() * (width()-5) );
      const uint y = (height() - h) / 2;
      painter.drawImage( QRect( 5, y, width(), h ), m_frame );

      const QString text = QString("%1x%2").arg( m_frame.width() ).arg( m_frame.height() );
      const uint x = (width() - fontMetrics().width( text ))/2;
      painter.drawText( x, y + h + fontMetrics().height() + 5, text );
   }

public:
   FrameCapturePreview( const QImage& frame, QWidget *parent )
         : KPreviewWidgetBase( parent )
         , m_frame( frame )
   {
      setMinimumWidth( 200 );
   }
};


class FrameCaptureDialog : public QDialog
{
   const QImage m_frame;
   const QString m_time;
   const QString m_title;

   void message( const QString &text ) { ((MainWindow*)parentWidget())->statusBar()->message( text, 4000 ); }

public:
   FrameCaptureDialog( const QImage &frame, const QString &time, MainWindow *parent )
         : QDialog( parent, 0, false /*modal*/, Qt::WDestructiveClose )
         , m_frame( frame )
         , m_time( time )
         , m_title( TheStream::prettyTitle() )
   {
      (new QVBoxLayout( this ))->setAutoAdd( true );
      (new QLabel( this ))->setPixmap( frame );

      QHBox *box = new QHBox( this );
      KPushButton *o = new KPushButton( KStdGuiItem::save(), box );
      connect( o, SIGNAL(clicked()), SLOT(accept()) );

      o = new KPushButton( KStdGuiItem::cancel(), box );
      o->setText( i18n("Discard") );
      connect( o, SIGNAL(clicked()), SLOT(reject()) );

      setCaption( i18n("Capture - %1").arg( time ) );
      setFixedSize( sizeHint() );

      show();

      //TODO don't activate
      //TODO move to the parent's side - not centrally aligned
   }

   ~FrameCaptureDialog()
   {
      delete [] m_frame.bits();
   }

   virtual void accept()
   {
      KFileDialog dialog( ":frame_capture", i18n("*.png|PNG Format\n*.jpeg|JPEG Format"), this, 0, false );
      dialog.setOperationMode( KFileDialog::Saving );
      dialog.setCaption( i18n("Save Frame") );
      dialog.setSelection( m_title + " - " + m_time + ".png" );
      dialog.setPreviewWidget( new FrameCapturePreview( m_frame, &dialog ) );

      if( dialog.exec() == Accepted ) {
         const QString fileName = dialog.selectedFile();
         if( fileName.isEmpty() )
            return;

         const QString type = dialog.currentFilter().remove( 0, 2 ).upper();
         if( m_frame.save( fileName, type ) )
            message( i18n("%1 saved successfully").arg( fileName ) );
         else
            message( i18n("Sorry, could not save %1").arg( fileName ) );
      }

      deleteLater();
   }
};


void
MainWindow::captureFrame()
{
   new FrameCaptureDialog( videoWindow()->captureFrame(), m_timeLabel->text(), this );
}


/************************************************************
 *   Helpers to convert yuy and yv12 frames to rgb          *
 *   code from gxine modified for 32bit output              *
 *   Copyright (C) 2000-2003 the xine project               *
 ************************************************************/

static void
yuy2Toyv12( uint8_t *y, uint8_t *u, uint8_t *v, uint8_t *input, int w, int h )
{
   const int w2 = w / 2;
   for( int j, i = 0; i < h; i += 2 ) {
       for( j = 0; j < w2; j++ )
       {
         // packed YUV 422 is: Y[i] U[i] Y[i+1] V[i]
         *(y++) = *(input++);
         *(u++) = *(input++);
         *(y++) = *(input++);
         *(v++) = *(input++);
       }

      // down sampling
      for( j = 0; j < w2; j++ ) {
         // skip every second line for U and V
         *(y++) = *(input++);
         input++;
         *(y++) = *(input++);
         input++;
      }
   }
}

static uchar*
yv12ToRgb( uint8_t *src_y, uint8_t *src_u, uint8_t *src_v, const int w, const int h )
{
   /// Create rgb data from yv12

   #define clip_8_bit(val)       \
         {                       \
            if( val < 0 )        \
               val = 0;          \
            else if( val > 255 ) \
               val = 255;        \
         }

   int y, u, v;
   int r, g, b;

   int sub_i_uv;
   int sub_j_uv;

   const int uv_width  = w / 2;
   const int uv_height = h / 2;

   uchar * const rgb = new uchar[(w * h * 4)]; //qt needs a 32bit align
   if( !rgb )
      return 0;

   for( int i = 0; i < h; ++i ) {
      // calculate u & v rows
      sub_i_uv = ((i * uv_height) / h);

      for( int j = 0; j < w; ++j ) {
         // calculate u & v columns
         sub_j_uv = (j * uv_width) / w;

         /***************************************************
         *
         *  Colour conversion from
         *    http://www.inforamp.net/~poynton/notes/colour_and_gamma/ColorFAQ.html#RTFToC30
         *
         *  Thanks to Billy Biggs <vektor@dumbterm.net>
         *  for the pointer and the following conversion.
         *
         *   R' = [ 1.1644         0    1.5960 ]   ([ Y' ]   [  16 ])
         *   G' = [ 1.1644   -0.3918   -0.8130 ] * ([ Cb ] - [ 128 ])
         *   B' = [ 1.1644    2.0172         0 ]   ([ Cr ]   [ 128 ])
         *
         *  Where in xine the above values are represented as
         *
         *   Y' == image->y
         *   Cb == image->u
         *   Cr == image->v
         *
         ***************************************************/

         y = src_y[(i * w) + j] - 16;
         u = src_u[(sub_i_uv * uv_width) + sub_j_uv] - 128;
         v = src_v[(sub_i_uv * uv_width) + sub_j_uv] - 128;

         r = (int)((1.1644 * (double)y) + (1.5960 * (double)v));
         g = (int)((1.1644 * (double)y) - (0.3918 * (double)u) - (0.8130 * (double)v));
         b = (int)((1.1644 * (double)y) + (2.0172 * (double)u));

         clip_8_bit( r );
         clip_8_bit( g );
         clip_8_bit( b );

         rgb[(i * w + j) * 4 + 0] = b;
         rgb[(i * w + j) * 4 + 1] = g;
         rgb[(i * w + j) * 4 + 2] = r;
         rgb[(i * w + j) * 4 + 3] = 0;
      }
   }

   return rgb;
}

/************************************************************/


QImage
VideoWindow::captureFrame() const
{
   DEBUG_BLOCK

   int ratio, format, w, h;
   if( !xine_get_current_frame( *engine(), &w, &h, &ratio, &format, NULL ) )
      return QImage();

   uint8_t *yuv = new uint8_t[((w+8) * (h+1) * 2)];
   if( yuv == 0 ) {
      Debug::error() << "Not enough memory to make screenframe!\n";
      return QImage(); }

   xine_get_current_frame( *engine(), &w, &h, &ratio, &format, yuv );

   // convert to yv12 if necessary
   uint8_t *y = 0, *u = 0, *v = 0;
   switch( format )
   {
   case XINE_IMGFMT_YUY2: {
      uint8_t *yuy2 = yuv;

      yuv = new uint8_t[(w * h * 2)];
      if( yuv == 0 ) {
         Debug::error() << "Not enough memory to make screenframe!\n";
         delete [] yuy2;
         return QImage(); }

      y = yuv;
      u = yuv + w * h;
      v = yuv + w * h * 5 / 4;

      yuy2Toyv12( y, u, v, yuy2, w, h );

      delete [] yuy2;
   }  break;

   case XINE_IMGFMT_YV12:
      y = yuv;
      u = yuv + w * h;
      v = yuv + w * h * 5 / 4;
      break;

   default:
      Debug::warning() << "Format " << format << " not supported!\n";
      delete [] yuv;
      return QImage();
   }

   // convert to rgb
   uchar *rgb = yv12ToRgb( y, u, v, w, h );
   QImage frame( rgb, w, h, 32, 0, 0, QImage::IgnoreEndian );
   delete [] yuv;

   return frame;
}

}
