/*
* ============================================================================
*  Name     : CGameDocument from GameDocument.h
*  Part of  : Game
*  Created  : 31.1.2004 by Sami Kyöstilä
*  Description:
*     Declares document for application.
*  Version  :
*  Copyright: Sami Kyöstilä
* ============================================================================
*/

#ifndef GAMEDOCUMENT_H
#define GAMEDOCUMENT_H

// INCLUDES
#include <akndoc.h>
   
// CONSTANTS

// FORWARD DECLARATIONS
class  CEikAppUi;

// CLASS DECLARATION

/**
*  CGameDocument application class.
*/
class CGameDocument : public CAknDocument
    {
    public: // Constructors and destructor
        /**
        * Two-phased constructor.
        */
        static CGameDocument* NewL(CEikApplication& aApp);

        /**
        * Destructor.
        */
        virtual ~CGameDocument();

    public: // New functions

    protected:  // New functions

    protected:  // Functions from base classes

    private:

        /**
        * EPOC default constructor.
        */
        CGameDocument(CEikApplication& aApp);
        void ConstructL();

    private:

        /**
        * From CEikDocument, create CGameAppUi "App UI" object.
        */
        CEikAppUi* CreateAppUiL();
    };

#endif

// End of File

