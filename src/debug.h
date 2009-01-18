/***********************************************************************
 * Copyright 2003-5  Max Howell <max.howell@methylblue.com>
 *           2007    Mark Kretschmann <kretschmann@kde.org>
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License as
 * published by the Free Software Foundation; either version 2 of
 * the License or (at your option) version 3 or any later version
 * accepted by the membership of KDE e.V. (or its successor approved
 * by the membership of KDE e.V.), which shall act as a proxy 
 * defined in Section 14 of version 3 of the license.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 ***********************************************************************/

// Author:   
// Author:   

#ifndef DRAGONPLAYER_DEBUG_H
#define DRAGONPLAYER_DEBUG_H

// We always want debug output available at runtime
#undef QT_NO_DEBUG_OUTPUT
#undef KDE_NO_DEBUG_OUTPUT

#include <KGlobal>
#include <KConfig>
#include <KConfigGroup>
#include <KDebug>
#include <KCmdLineArgs>

#include <QApplication>
#include <QObject>

#include <iostream>
#include <sys/time.h>

#ifdef _WIN32
#define __PRETTY_FUNCTION__ __FUNCTION__
#endif

/**
 * @namespace Debug
 * @short kdebug with indentation functionality and convenience macros
 * @author Max Howell <max.howell@methylblue.com>
 *
 * Usage:
 *
 *     #define DEBUG_PREFIX "Blah"
 *     #include "debug.h"
 *
 *     void function()
 *     {
 *        Debug::Block myBlock( __PRETTY_FUNCTION__ );
 *
 *        debug() << "output1" << endl;
 *        debug() << "output2" << endl;
 *     }
 *
 * Will output:
 *
 * app: BEGIN: void function()
 * app:   [Blah] output1
 * app:   [Blah] output2
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
        friend QString &modifieableIndent();
        Indent() : QObject( qOApp ) { setObjectName( "DEBUG_indent" ); }
        QString m_string;
    };

    inline QString &modifieableIndent()
    {
        QObject* o = qOApp ? qOApp->findChild<QObject*>( "DEBUG_indent" ) : 0;
        QString &ret = (o ? static_cast<Indent*>( o ) : new Indent)->m_string;
        return ret;
    }

    inline QString indent()
    {
        return modifieableIndent();
    }

    inline bool debugEnabled()
    {
		KCmdLineArgs *args = KCmdLineArgs::parsedArgs();
        if(args->appName() == "dragon" && args->isSet("debug"))
        {
			return true;
        }

        KConfigGroup config = KGlobal::config()->group( "General" );
        const bool debug = config.readEntry( "Debug Enabled", false );
        return debug;
    }

    inline kdbgstream dbgstream()
    {
        return debugEnabled() ? kdbgstream( QtDebugMsg ) : kDebugDevNull();
    }
 
    #undef qOApp

    #ifndef DEBUG_PREFIX
    #define AMK_PREFIX ""
    #else
    #define AMK_PREFIX "[" DEBUG_PREFIX "]"
    #endif

    //from kdebug.h
    enum DebugLevels {
        KDEBUG_INFO  = 0,
        KDEBUG_WARN  = 1,
        KDEBUG_ERROR = 2,
        KDEBUG_FATAL = 3
    };

        
    static inline kdbgstream debug()   { QString ind = indent(); return dbgstream() << qPrintable( "dragonplayer: " + ind + AMK_PREFIX ); }
    static inline kdbgstream warning() { QString ind = indent(); return dbgstream() << qPrintable( "dragonplayer: " + ind + AMK_PREFIX + " [WARNING!]" ); }
    static inline kdbgstream error()   { QString ind = indent(); return dbgstream() << qPrintable( "dragonplayer: " + ind + AMK_PREFIX + " [ERROR!]" ); }
    static inline kdbgstream fatal()   { QString ind = indent(); return dbgstream() << qPrintable( "dragonplayer: " + ind + AMK_PREFIX ); }
}

using Debug::debug;
using Debug::warning;
using Debug::error;
using Debug::fatal;

/// Standard function announcer
#define DEBUG_FUNC_INFO { kDebug() << Debug::indent(); }

/// Announce a line
#define DEBUG_LINE_INFO { kDebug() << Debug::indent() << "Line: " << __LINE__; }

/// Convenience macro for making a standard Debug::Block
#define DEBUG_BLOCK Debug::Block uniquelyNamedStackAllocatedStandardBlock( __PRETTY_FUNCTION__ );

/// Use this to remind yourself to finish the implementation of a function
#define DRAGONPLAYER_NOTIMPLEMENTED warning() << "NOT-IMPLEMENTED: " << __PRETTY_FUNCTION__ << endl;

/// Use this to alert other developers to stop using a function
#define DRAGONPLAYER_DEPRECATED warning() << "DEPRECATED: " << __PRETTY_FUNCTION__ << endl;


namespace Debug
{
    /**
     * @class Debug::Block
     * @short Use this to label sections of your code
     *
     * Usage:
     *
     *     void function()
     *     {
     *         Debug::Block myBlock( "section" );
     *
     *         debug() << "output1" << endl;
     *         debug() << "output2" << endl;
     *     }
     *
     * Will output:
     *
     *     app: BEGIN: section
     *     app:  [prefix] output1
     *     app:  [prefix] output2
     *     app: END: section - Took 0.1s
     *
     */

    class Block
    {
        timeval     m_start;
        const char *m_label;

    public:
        Block( const char *label )
                : m_label( label )
        {
            if( !debugEnabled() ) return;

            gettimeofday( &m_start, 0 );

            dbgstream() << "dragonplayer: BEGIN:" << label;
            Debug::modifieableIndent() += "  ";
        }

        ~Block()
        {
            if( !debugEnabled() ) return;

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
            dbgstream() << "dragonplayer: END__:" << m_label
                        << "- Took" << qPrintable( QString::number( duration, 'g', 2 ) + "s" );
        }
    };


    /**
     * @name Debug::stamp()
     * @short To facilitate crash/freeze bugs, by making it easy to mark code that has been processed
     *
     * Usage:
     *
     *     {
     *         Debug::stamp();
     *         function1();
     *         Debug::stamp();
     *         function2();
     *         Debug::stamp();
     *     }
     *
     * Will output (assuming the crash occurs in function2()
     *
     *     app: Stamp: 1
     *     app: Stamp: 2
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
     *     debug() << (Debug::List() << anInt << aString << aQStringList << aDouble) << endl;
     */

    typedef QList<QVariant> List;
}

#endif
