#include "GameEng.h"

#include <eikenv.h>

CGameEng::CGameEng(Game::Engine* aEngine, RWsSession& aClient, CWsScreenDevice& aScreenDevice, RWindow& aWindow, TBool aUseFrameBuffer):
        CTimer(CActive::EPriorityHigh),
        iEngine(aEngine),
        iScreen(0),
        iClient(aClient),
        iScreenDevice(aScreenDevice),
        iWindow(aWindow),
    iDirectScreenAccess(0),
    iGc(0),
    iRegion(0), 
    iPosition(0,0),
    iDrawing(EFalse),
    iScreenAddr(0),
    iOffScreenBmp(0),
    iStream(0),
    iUseFrameBuffer(aUseFrameBuffer)
{    
        iRect.SetRect(iPosition, aWindow.Size());
}

CGameEng* CGameEng::NewL(Game::Engine* aEngine, RWsSession& aClient, CWsScreenDevice& aScreenDevice, RWindow& aWindow, TBool aUseFrameBuffer)
{
        CGameEng* self = new (ELeave) CGameEng(aEngine, aClient, aScreenDevice, aWindow, aUseFrameBuffer);
        CleanupStack::PushL(self);
        self->ConstructL();
        CleanupStack::Pop(); // self;
        return self;
}

CGameEng::~CGameEng()
{
        if(IsActive())
                Cancel();

        delete iDirectScreenAccess;     
        delete iOffScreenBmp;    
        delete iScreen;

	if (iStream)
		iStream->Stop();

        delete iStream;
        delete iAudioBuffer;
        delete iAudioBufferPtr;
}

void CGameEng::ConstructL()
{
        // contruct the timer
        CTimer::ConstructL();

        // Create the DSA object
        iDirectScreenAccess = CDirectScreenAccess::NewL(iClient, iScreenDevice, iWindow, *this);

        CActiveScheduler::Add(this);

        // create the offscreen bitmap
        iOffScreenBmp = new (ELeave) CFbsBitmap;
        iOffScreenBmp->Create(iScreenDevice.SizeInPixels(), EColor4K);

#if defined(__WINS__)
        // for emulator, always use offscreen bitmap
        iUseFrameBuffer = EFalse;
#endif
    
        if(iUseFrameBuffer)        
        {
                // fetch screen buffer address
                TScreenInfoV01 screenInfo;
                TPckg<TScreenInfoV01> sInfo(screenInfo);
                UserSvr::ScreenInfo(sInfo);

                if (screenInfo.iScreenAddressValid)
                {
                        iScreenAddr = (TUint8*)screenInfo.iScreenAddress;
                        User::LeaveIfNull(iScreenAddr);

                        // skip the palette data in the beginning of frame buffer (16 entries in 12bit mode)
                        iScreenAddr += 16 * 2;
                }

                // initialise the raw redraw event (used when drawing directly to frame buffer)
                iRedraw.Set(TRawEvent::ERedraw);
        }

        // 12 bits per pixel
        Game::PixelFormat pf(12);
        iScreen = new Game::Surface(
                &pf,
                (Game::Pixel*)iOffScreenBmp->DataAddress(),
                iOffScreenBmp->SizeInPixels().iWidth,
                iOffScreenBmp->SizeInPixels().iHeight);

        // tell the engine about the video surface
        iEngine->configureVideo(iScreen);

        // initialize audio
        Game::SampleFormat sf(16,1);
#if defined(__WINS__)
        iAudioBuffer = new Game::SampleChunk(&sf, 1024, 8000);
#else
        iAudioBuffer = new Game::SampleChunk(&sf, 1024, 8000);
#endif
        iAudioBufferPtr = new TPtr8((TUint8*)iAudioBuffer->data, iAudioBuffer->bytes, iAudioBuffer->bytes);
        iStream = CMdaAudioOutputStream::NewL(*this);
        iStream->Open(&iSettings);
}

void CGameEng::StartDrawingL()
{
        // Initialise DSA

        // Trap the call to CDirectScreenAccess->StartL() to suppress system
        // error notes (e.g. active screen saver may cause problems)
        TRAPD(dsaErr, iDirectScreenAccess->StartL());

        if(dsaErr == KErrNone)
        {
                // Get graphics context for it
                iGc = iDirectScreenAccess->Gc();

                // Get region that DSA can draw in
                iRegion = iDirectScreenAccess->DrawingRegion();

                // Set the display to clip to this region
                iGc->SetClippingRegion(iRegion);

                iDrawing = ETrue;

                // request a timer event after a defined interval
                After(TTimeIntervalMicroSeconds32(0));
        }
}
        

void CGameEng::StopDrawing()
{
        // Cancel timer and display
        // This is CActive::Cancel, which calls derived method DoCancel
        Cancel();
        iDrawing = EFalse;
}
        
// Implement MDirectScreenAccess
void CGameEng::Restart(RDirectScreenAccess::TTerminationReasons /*aReason*/)
{
        // Restart display
        // Note that this will result in the clipping region being updated
        // so that menus, overlaying dialogs, etc. will not be drawn over      
        StartDrawingL();    
}

void CGameEng::AbortNow(RDirectScreenAccess::TTerminationReasons /*aReason*/)
{
        // Cancel timer and display
        Cancel();
        iDrawing = EFalse;
}

// Timer's RunL()
void CGameEng::RunL()
{
        iEngine->renderVideo(iScreen);

        // blit the offscreen bitmap (if used) to screen
        if(!iUseFrameBuffer)
        {        
                iGc->BitBlt(iPosition, iOffScreenBmp, iRect);
                // Force screen update: this is required for WINS, but may
                // not be for all hardware. 
                // For Series 60 devices this is necessary, 
                // we can't access screen memory directly, 
                // rather we access a screen buffer.
                iDirectScreenAccess->ScreenDevice()->Update();
        }
        else
        {        
                // copy bitmap contents to frame buffer
                Mem::Copy(iScreenAddr, iOffScreenBmp->DataAddress(), iScreen->bytes);

                // Drawing to screen buffer (ScreenInfo) - force update
                // We do not use Graphics context, so we generate an event to get screen refreshed
                // As a side-effect, the backlight stays on        
                UserSvr::AddEvent(iRedraw);
        }        

        // Renew request
        After(TTimeIntervalMicroSeconds32(10000));
}

// Timer's DoCancel()
void CGameEng::DoCancel()
{
        // Cancel timer
        CTimer::DoCancel();

        // Cancel DSA
        iDirectScreenAccess->Cancel();    
}

// Audio        
void CGameEng::MaoscOpenComplete(TInt aError)
{
        if (aError==KErrNone)
        {
                TInt channels = (iAudioBuffer->format.channels==1)?TMdaAudioDataSettings::EChannelsMono:TMdaAudioDataSettings::EChannelsStereo;
#if defined(__WINS__)
                TInt rate = TMdaAudioDataSettings::ESampleRate22050Hz;
#else
                TInt rate = TMdaAudioDataSettings::ESampleRate8000Hz;
#endif

                switch(iAudioBuffer->rate)
                {
                case 8000:
                        rate = TMdaAudioDataSettings::ESampleRate8000Hz;
                break;
                case 11025:
                        rate = TMdaAudioDataSettings::ESampleRate11025Hz;
                break;
                case 16000:
                        rate = TMdaAudioDataSettings::ESampleRate16000Hz;
                break;
                case 22050:
                        rate = TMdaAudioDataSettings::ESampleRate22050Hz;
                break;
                case 32000:
                        rate = TMdaAudioDataSettings::ESampleRate32000Hz;
                break;
                case 44100:
                        rate = TMdaAudioDataSettings::ESampleRate44100Hz;
                break;
                case 48000:
                        rate = TMdaAudioDataSettings::ESampleRate48000Hz;
                break;
                default:
#if defined(__WINS__)
                        iAudioBuffer->rate = 22050;
#else
                        iAudioBuffer->rate = 8000;
#endif
                break;
                }

                // Set stream format and rate.
                TRAPD(status,iStream->SetAudioPropertiesL(rate, channels));

                if (status==KErrNone)
                {
                        iEngine->configureAudio(iAudioBuffer);

                        // Note that MaxVolume() is different in the emulator and the real device!
                        iStream->SetVolume(iStream->MaxVolume() / 2);
                        iStream->SetPriority(EPriorityNormal, EMdaPriorityPreferenceNone);
                        GetAudioData();
                }
        }
}

void CGameEng::MaoscBufferCopied(TInt aError, const TDesC8& aBuffer)
{
        if (aError==KErrNone)
        {
                GetAudioData();
        }
}

void CGameEng::MaoscPlayComplete(TInt aError)
{
        // We only want to restart in case of an underflow.
        // If aError!=KErrUnderflow the stream probably was stopped manually.
        if (aError==KErrUnderflow)
        {
                GetAudioData();
        }
}

void CGameEng::GetAudioData()
{
#if 0	// The most annoying sound in the world
	int l = iAudioBuffer->length;
	short *d = (short*)iAudioBuffer->data;

	while(l--)
		*d++ = (l&1)*10000;
#else
        iEngine->renderAudio(iAudioBuffer);
#endif
        iStream->WriteL(*iAudioBufferPtr);
}

