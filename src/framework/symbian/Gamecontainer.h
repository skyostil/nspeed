#ifndef GAMECONTAINER_H
#define GAMECONTAINER_H

#include <w32std.h>
#include <coecntrl.h>
#include "engine/engine.h"

class CGameEng;
   
class CGameContainer: public CCoeControl
{
public:
	CGameContainer(Game::Engine* aEngine);
	~CGameContainer();
    void ConstructL();

	void StartDrawingL();
	void StopDrawing();
	TBool IsDrawing();
	void SizeChanged();
	void Draw(const TRect& aRect) const;

	CGameEng*		iGameEng;
private:
	Game::Engine*	iEngine;
};

#endif
