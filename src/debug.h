// Author:     Max Howell <max.howell@methylblue.com>, (C) 2003-5
// Copyright: See COPYING file that comes with this distribution
//

#ifndef CODEINE_DEBUG_H
#define CODEINE_DEBUG_H

#include <kdebug.h>
#include <q3cstring.h>
#include <QObject>
#include <QVariant>
//Added by qt3to4:
#include <Q3ValueList>
#include <sys/time.h>

#include <QApplication>
#include <iostream>

/**
 * @namespace Debug
 * @short kdebug with indentation functionality and convenience macros
 * @author Max Howell <max.howell@methylblue.com>
 *
 * Usage:
 *
 *      #define DEBUG_PREFIX "Blah"
 *      #include "debug.h"
 *
 *      void function()
 *      {
 *          Debug::Block myBlock( __PRETTY_FUNCTION__ );
 *
 *          debug() << "output1" << endl;
 *          debug() << "output2" << endl;
 *      }
 *
 * Will output:
 *
 * app: BEGIN: void function()
 * app:    [Blah] output1
 * app:    [Blah] output2
 * app: END: void function(): Took 0.1s
 *
 * @see Block
 * @see CrashHelper
 * @see ListStream
 */

namespace Debug
{
     // we can't use a statically instantiated QCString for the indent, because
     // static namespaces are unique to each dlopened library. So we piggy back
     // the QCString on the KApplication instance

     #define qOApp reinterpret_cast<QObject*>(qApp)
     class Indent : QObject
     {
          friend Q3CString &modifieableIndent();
          Indent() : QObject( qOApp ) { setObjectName( "DEBUG_indent" ); }
          Q3CString m_string;
     };

     inline Q3CString &modifieableIndent()
     {
          QObject* o = qOApp ? qOApp->findChild<QObject*>( "DEBUG_indent" ) : 0;
          Q3CString &ret = (o ? static_cast<Indent*>( o ) : new Indent)->m_string;
          return ret;
     }

     inline Q3CString indent()
     {
          return modifieableIndent();
     }
     #undef qOApp


     #ifdef NDEBUG
          static inline kndbgstream debug()    { return kDebugDevNull(); }
          static inline kndbgstream warning() { return kDebugDevNull(); }
          static inline kndbgstream error()    { return kDebugDevNull(); }
          static inline kndbgstream fatal()    { return kDebugDevNull(); }

          static inline void debug1( QVariant v ) {}
     #else
          #ifndef DEBUG_PREFIX
          #define AMK_PREFIX ""
          #else
          #define AMK_PREFIX "[" DEBUG_PREFIX "] "
          #endif

          //from kdebug.h
          enum DebugLevels {
                KDEBUG_INFO  = 0,
                KDEBUG_WARN  = 1,
                KDEBUG_ERROR = 2,
                KDEBUG_FATAL = 3
          };

          static inline QDebug debug()   { return kDebugStream( QtDebugMsg,    0 ) << indent().data() << AMK_PREFIX; }
          static inline QDebug warning() { return kDebugStream( QtWarningMsg,  0 ) << indent().data() << AMK_PREFIX << "[WARNING!] "; }
          static inline QDebug error()   { return kDebugStream( QtCriticalMsg, 0 ) << indent().data() << AMK_PREFIX << "[ERROR!] "; }
          static inline QDebug fatal()   { return kDebugStream( QtFatalMsg,    0 ) << indent().data() << AMK_PREFIX; }

          static inline void debug1( QVariant v ) { kDebugStream( QtDebugMsg, 0 ) << indent().data() << v << endl; }
          #undef AMK_PREFIX
     #endif
}

using Debug::debug;
using Debug::debug1;
using Debug::warning;
using Debug::error;
using Debug::fatal;

/// Standard function announcer
#define DEBUG_FUNC_INFO { kDebug() << Debug::indent() << k_funcinfo << endl; }

/// Announce a line
#define DEBUG_LINE_INFO { kDebug() << Debug::indent() << k_funcinfo << "Line: " << __LINE__ << endl; }

/// Convenience macro for making a standard Debug::Block
#define DEBUG_BLOCK Debug::Block uniquelyNamedStackAllocatedStandardBlock( __PRETTY_FUNCTION__ );

/// Use this to remind yourself to finish the implementation of a function
#define AMAROK_NOTIMPLEMENTED warning() << "NOT-IMPLEMENTED: " << __PRETTY_FUNCTION__ << endl;

/// Use this to alert other developers to stop using a function
#define AMAROK_DEPRECATED warning() << "DEPRECATED: " << __PRETTY_FUNCTION__ << endl;


namespace Debug
{
     /**
      * @class Debug::Block
      * @short Use this to label sections of your code
      *
      * Usage:
      *
      *      void function()
      *      {
      *            Debug::Block myBlock( "section" );
      *
      *            debug() << "output1" << endl;
      *            debug() << "output2" << endl;
      *      }
      *
      * Will output:
      *
      *      app: BEGIN: section
      *      app:  [prefix] output1
      *      app:  [prefix] output2
      *      app: END: section - Took 0.1s
      *
      */

     class Block
     {
          timeval      m_start;
          const char *m_label;
          

     public:
          Block( const char *label )
                     : m_label( label )
          {
                gettimeofday( &m_start, 0 ); //may not be thread safe

                kDebug() << "BEGIN: " << label << "\n";
                Debug::modifieableIndent() += "  ";
          }

          ~Block()
          {
                timeval end;
                gettimeofday( &end, 0 );

                end.tv_sec -= m_start.tv_sec;
                if( end.tv_usec < m_start.tv_usec) {
                     // Manually carry a one from the seconds field.
                     end.tv_usec += 1000000;
                     end.tv_sec--;
                }
                end.tv_usec -= m_start.tv_usec;

                double duration = double(end.tv_sec) + (double(end.tv_usec) / 1000000.0);

                Debug::modifieableIndent().truncate( Debug::indent().length() - 2 );
                kDebug() << "END__: " << m_label
                             << " - Took " << QString::number( duration, 'g', 2 ) << "s\n";
          }
     };


     /**
      * @name Debug::stamp()
      * @short To facilitate crash/freeze bugs, by making it easy to mark code that has been processed
      *
      * Usage:
      *
      *      {
      *            Debug::stamp();
      *            function1();
      *            Debug::stamp();
      *            function2();
      *            Debug::stamp();
      *      }
      *
      * Will output (assuming the crash occurs in function2()
      *
      *      app: Stamp: 1
      *      app: Stamp: 2
      *
      */

     inline void stamp()
     {
          static int n = 0;
          debug() << "| Stamp: " << ++n << endl;
     }
}


#include <QVariant>

namespace Debug
{
     /**
      * @class Debug::List
      * @short You can pass anything to this and it will output it as a list
      *
      *      debug() << (Debug::List() << anInt << aString << aQStringList << aDouble) << endl;
      */

     typedef Q3ValueList<QVariant> List;
}

#include <kmessagebox.h>

namespace Codeine
{
    //FIXME this function is inlined, so this may cause linkage problems for some people..
    extern class VideoWindow* const videoWindow();

    namespace MessageBox
    {
        static inline void error( const QString &message )
        {
            KMessageBox::error( (QWidget*)videoWindow(), message );
        }

        static inline void sorry( const QString &message )
        {
            KMessageBox::error( (QWidget*)videoWindow(), message );
        }

        static inline void information( const QString &message, const QString &title )
        {
            KMessageBox::information( (QWidget*)videoWindow(), message, title );
        }
    }
}

#endif
