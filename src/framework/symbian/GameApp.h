/*
* ============================================================================
*  Name     : CGameApp from GameApp.h
*  Part of  : Game
*  Created  : 31.1.2004 by Sami Kyöstilä
*  Description:
*     Declares main application class.
*  Version  :
*  Copyright: Sami Kyöstilä
* ============================================================================
*/

#ifndef GAMEAPP_H
#define GAMEAPP_H

// INCLUDES
#include <aknapp.h>

// CONSTANTS
// UID of the application
const TUid KUidGame = { 0x06406E42 };

// CLASS DECLARATION

/**
* CGameApp application class.
* Provides factory to create concrete document object.
* 
*/
class CGameApp : public CAknApplication
    {
    
    public: // Functions from base classes
    private:

        /**
        * From CApaApplication, creates CGameDocument document object.
        * @return A pointer to the created document object.
        */
        CApaDocument* CreateDocumentL();
        
        /**
        * From CApaApplication, returns application's UID (KUidGame).
        * @return The value of KUidGame.
        */
        TUid AppDllUid() const;
    };

#endif

// End of File

