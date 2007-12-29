#ifndef CODEINE_MESSAGEBOX
#define CODEINE_MESSAGEBOX

#include <KMessageBox>
namespace Codeine {
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
