#include "GameImageLoader.h"
#include <e32math.h>

#define IMAGEREAD_TIMEOUT 5 * 1000 * 1000

void CGameImageLoader::DoCancel()
{
   // cancel the timer    
   iTimer->Cancel();
   iTimer->Close();

   // end blocking in ReadTextureL()
   CActiveScheduler::Stop();
}

void CGameImageLoader::RunL()
{
   // timer timeout - loading failed
   Cancel();
   iTimer->Close();
}

Game::Surface *CGameImageLoader::LoadImageL(const TDesC &aFilename, Game::PixelFormat *pf)
{
   // construct new loader instance
   CGameImageLoader *loader = new (ELeave) CGameImageLoader(&aFilename, pf);
   CleanupStack::PushL(loader);

   // add the loader to active scheduler
   CActiveScheduler::Add(loader);    

   // perform operations for reading and converting the image
   loader->ReadImageL();

   // start a nested scheduling; blocks until CActiveScheduler::Stop() 
   // is called in DoCancel()
   CActiveScheduler::Start();

   // if error, leave with correct error code
   if( loader->iSurface == NULL ) {
       // instance will be destroyed by the cleanupstack
       User::Leave(loader->iErrorCode);
   }

   // get a local copy of the instance's created surface
   // if error(s) in process, this will be NULL
   Game::Surface *texture = loader->iSurface;

   // deallocate the instance
   CleanupStack::PopAndDestroy();

   // return the created texture
   return texture;
}

CGameImageLoader::CGameImageLoader(const TDesC *aFilename, Game::PixelFormat *pf) 
   : CActive(CActive::EPriorityStandard),
   iPixelFormat(pf)
{
   // make a local copy of the filename
   iFilename = aFilename->Alloc();
}

CGameImageLoader::~CGameImageLoader()
{
   // deallocate all data
   delete iFilename;
   delete iTimer;
   delete iConverter;
   delete iBitmap;
}

// performs the actual reading and conversion of the image
void CGameImageLoader::ReadImageL()
{
   // reset the texture 
   iSurface = NULL;

   // create + initialize operation timeout timer
   iTimer = new RTimer();
   iTimer->CreateLocal();

   // set timeout for the image read + conversion process
   iTimer->After(iStatus, IMAGEREAD_TIMEOUT);
   SetActive();

   // start loading the image
   iConverter = CMdaImageFileToBitmapUtility::NewL(*this);
   iConverter->OpenL(*iFilename);
}

// called when OpenL() finishes
void CGameImageLoader::MiuoOpenComplete(TInt aError)
{
   if( aError != KErrNone ) {
       iErrorCode = aError;
       Cancel();
       return;
   }

   TFrameInfo info;
   iConverter->FrameInfo(0, info);
        
   // create a bitmap to write into 
   iBitmap = new (ELeave) CFbsBitmap();
   TInt rc = iBitmap->Create(info.iOverallSizeInPixels, EColor4K);
   if( rc != KErrNone )
   {
       iErrorCode = rc;
       Cancel();
       return;
   }

   // convert the gif into a bitmap
   TRAPD(error, iConverter->ConvertL(*iBitmap));
        
   // handle the error
   if( error != KErrNone)
   {
       iErrorCode = error;
       Cancel();
       return;
   }
}

// called when ConvertL() finishes
void CGameImageLoader::MiuoConvertComplete(TInt aError)
{
   if( aError != KErrNone ) {
       iErrorCode = aError;
       Cancel();
       return;
   }

   // create the iSurface out of the bitmap data
   CreateSurface();

   // cancel the timeout timer to end blocking in LoadTextureL()
   Cancel();
}

void CGameImageLoader::CreateSurface()
{
   TSize imagesize = iBitmap->SizeInPixels();

   // allocate memory for the indexed texture data    
   Game::PixelFormat bitmapPixelFormat(12);

   if (iPixelFormat)
   {
	// the image must be converted to the requested pixel format
	int x, y;
	TRgb pixel;
	TPoint point;

	TRAPD(error, iSurface = new Game::Surface(iPixelFormat, imagesize.iWidth, imagesize.iHeight));

	if( error != KErrNone )
	{
		iErrorCode = error;
		return;
	}

	if (!iSurface)
	{
		iErrorCode = KErrNoMemory;
		return;
	}

	for(y=0; y<imagesize.iWidth; y++)
	for(x=0; x<imagesize.iWidth; x++)
	{
		point.iX = x;
		point.iY = y;
		iBitmap->GetPixel(pixel, point);
		iSurface->setPixel(x, y, iPixelFormat->makePixel(pixel.Red(), pixel.Green(), pixel.Blue()));
	}
   }
   else
   {
	// no pixel conversion required
	Game::PixelFormat bitmapPixelFormat(12);
	
	TRAPD(error, iSurface = new Game::Surface(&bitmapPixelFormat, (Game::Pixel*)iBitmap->DataAddress(), imagesize.iWidth, imagesize.iHeight));

	if( error != KErrNone )
	{
		iErrorCode = error;
		return;
	}

	if (!iSurface)
	{
		iErrorCode = KErrNoMemory;
		return;
	}
   }
}

// not used 
void CGameImageLoader::MiuoCreateComplete(TInt /*aError*/)
{
}