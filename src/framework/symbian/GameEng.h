#ifndef GAMEENG_H
#define GAMEENG_H

#include <w32std.h>
#include <mdaaudiooutputstream.h>
#include <mda\common\audio.h>
#include "engine/engine.h"
   
class CGameEng: public CTimer, public MDirectScreenAccess, public MMdaAudioOutputStreamCallback
{
public:
        // Construction & destruction
        static CGameEng* NewL 
                (
                Game::Engine* aEngine,
                RWsSession& aClient, 
                CWsScreenDevice& aScreenDevice, 
                RWindow& aWindow,
        TBool aUseFrameBuffer=EFalse    
                );

        ~CGameEng();    

public:
        // Start and stop the drawing
        void StartDrawingL();
        void StopDrawing();

        // Are we drawing
        inline TBool Drawing(){return iDrawing;}

private:

        // Implement MDirectScreenAccess 
        void Restart(RDirectScreenAccess::TTerminationReasons aReason);
        void AbortNow(RDirectScreenAccess::TTerminationReasons aReason);

        // Implement MMdaAudioOutputStreamCallback
        void MaoscOpenComplete(TInt aError);
        void MaoscBufferCopied(TInt aError, const TDesC8& aBuffer);
        void MaoscPlayComplete(TInt aError);
        void GetAudioData();

        // Implement CTimer
        void RunL();
        void DoCancel();

    CGameEng
        (
                Game::Engine* aEngine,
                RWsSession& aClient, 
                CWsScreenDevice& aScreenDevice, 
                RWindow& aWindow,
        TBool aUseFrameBuffer
        );

    void ConstructL();  

        // Window server handling
        RWsSession& iClient;
        CWsScreenDevice& iScreenDevice;
        RWindow& iWindow;

        // Direct Screen Access
        CDirectScreenAccess* iDirectScreenAccess;
        CFbsBitGc* iGc;
        RRegion* iRegion;

        // Positional plus directional information
        TPoint                  iPosition;      
        TRect                   iRect;

        // Are we drawing
        TBool                   iDrawing;

        // Video
        Game::Engine*   iEngine;
        Game::Surface*  iScreen;

        TUint8*                 iScreenAddr;        // frame buffer address
        CFbsBitmap*             iOffScreenBmp;          // offscreen bitmap for double-buffered drawing

        TBool                   iUseFrameBuffer;
    
        TRawEvent               iRedraw;                // raw event for forcing the update of screen 

        // Audio
        Game::SampleChunk*      iAudioBuffer;
        CMdaAudioOutputStream*  iStream;
        TMdaAudioDataSettings   iSettings;
        TPtr8*                  iAudioBufferPtr;
};

#endif
