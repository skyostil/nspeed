#include "GameContainer.h"
#include <eikenv.h>
#include "GameEng.h"

CGameContainer::CGameContainer(Game::Engine* aEngine):
	iEngine(aEngine)
{
}

void CGameContainer::ConstructL()
{
	CreateWindowL();

	SetExtentToWholeScreen();

	// Set up our engine
	iGameEng = CGameEng::NewL(
				iEngine,
				iEikonEnv->WsSession(), 
				*(CCoeEnv::Static()->ScreenDevice()), 
				Window(), ETrue);

    ActivateL();
}

CGameContainer::~CGameContainer()
{
	if(iGameEng && iGameEng ->Drawing())
		iGameEng ->StopDrawing();
	delete iGameEng ;
	iGameEng = NULL;
}

void CGameContainer::SizeChanged()
{
}


void CGameContainer::Draw(const TRect& aRect) const
{
    // if the engine is running, no need to draw any controls    
    if(iGameEng->Drawing())
        return;

	// clear the screen
    CWindowGc& gc = SystemGc();
    gc.SetPenStyle(CGraphicsContext::ENullPen);
    gc.SetBrushColor(KRgbBlack);
    gc.SetBrushStyle(CGraphicsContext::ESolidBrush);
    gc.DrawRect(aRect);

	gc.UseFont(CEikonEnv::Static()->TitleFont());
    gc.SetPenColor(KRgbWhite);
	gc.DrawText(_L("Paused"), aRect, aRect.Height()/2, CGraphicsContext::ECenter);
}

void CGameContainer::StartDrawingL()
{    
	iGameEng->StartDrawingL();
}

void CGameContainer::StopDrawing()
{    
	iGameEng->StopDrawing();    
}

TBool CGameContainer::IsDrawing()
{    
	return iGameEng->Drawing();	
}
