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

private:
	void DynInitMenuPaneL(TInt aResourceId,CEikMenuPane* aMenuPane);

private:
	void HandleCommandL(TInt aCommand);
	virtual TKeyResponse HandleKeyEventL(
		const TKeyEvent& aKeyEvent,TEventCode aType);

private: //Data
    CGameContainer* iAppContainer; 
	Game::Engine*	iEngine;
	TInt			iTimerFreq;
};

#endif

// End of File
