#ifndef GAMEIMAGELOADER_H
#define GAMEIMAGELOADER_H

// This code based on image loader by Matti Dahlbom.

#include <e32base.h>
#include <MdaImageConverter.h>
#include <fbs.h>

#include "engine/Engine.h"

class CGameImageLoader : public CActive, public MMdaImageUtilObserver
{
   public:
       static Game::Surface *LoadImageL(const TDesC &aFilename, Game::PixelFormat *pf = NULL);

       // from MMdaImageUtilObserver 
       virtual void MiuoOpenComplete(TInt aError);
       virtual void MiuoConvertComplete(TInt aError);
       virtual void MiuoCreateComplete(TInt aError);

       // from CActive
       void RunL();
       void DoCancel();
   private:
       CGameImageLoader(const TDesC *aFilename, Game::PixelFormat *pf);
       ~CGameImageLoader();
       void ReadImageL();
       void CreateSurface();

       TDesC *iFilename;
       CMdaImageFileToBitmapUtility *iConverter;
       RTimer *iTimer;
       Game::Surface *iSurface;
       Game::PixelFormat *iPixelFormat;
       CFbsBitmap *iBitmap;
       TInt iErrorCode;
};

#endif