#ifndef GAMEAPPUI_H
#define GAMEAPPUI_H

#include <eikapp.h>
#include <eikdoc.h>
#include <e32std.h>
#include <coeccntx.h>
#include <aknappui.h>

#include "engine/engine.h"

class CGameContainer;

class CGameAppUi: public CAknAppUi, Game::Framework
{
public:
    void ConstructL();

    ~CGameAppUi();
    
        // Framework stuff
        void exit();
        unsigned int getTickCount();
        unsigned int getTicksPerSecond();

        Game::Surface *loadImage(const char *name, Game::PixelFormat *pf = 0);
	const char *findResource(const char *name);

private:
        void DynInitMenuPaneL(TInt aResourceId,CEikMenuPane* aMenuPane);
	TDesC& GetFilenameDes(const char *name);

private:
        void HandleCommandL(TInt aCommand);
        virtual TKeyResponse HandleKeyEventL(
        const TKeyEvent& aKeyEvent,TEventCode aType);

private: //Data
    CGameContainer* iAppContainer; 
    Game::Engine*   iEngine;
    TInt            iTimerFreq;
	unsigned int	iTimerBase;
    TBuf<512>       iResourcePathBuf;
    char            iResourcePath[256];
};

#endif

// End of File
