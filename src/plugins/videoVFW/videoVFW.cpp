////////////////////////////////////////////////////////
//
// GEM - Graphics Environment for Multimedia
//
// zmoelnig@iem.kug.ac.at
//
// Implementation file 
//
//    Copyright (c) 1997-1999 Mark Danks.
//    Copyright (c) G�nther Geiger.
//    Copyright (c) 2001-2002 IOhannes m zmoelnig. forum::f�r::uml�ute. IEM
//    For information on usage and redistribution, and for a DISCLAIMER OF ALL
//    WARRANTIES, see the file, "GEM.LICENSE.TERMS" in this distribution.
//
/////////////////////////////////////////////////////////
#ifdef HAVE_CONFIG_H
# include "config.h"
#endif

#include "videoVFW.h"
using namespace gem;
#include "Gem/RTE.h"

#ifdef HAVE_VFW_H
REGISTER_VIDEOFACTORY("VFW", videoVFW);
/////////////////////////////////////////////////////////
//
// videoVFW
//
/////////////////////////////////////////////////////////
// Constructor
//
/////////////////////////////////////////////////////////
videoVFW :: videoVFW()
  : video(0), 
    m_hWndC(NULL)
{
  provide("vfw");
  provide("dv");
  provide("iidc");
  provide("analog");
}

/////////////////////////////////////////////////////////
// Destructor
//
/////////////////////////////////////////////////////////
videoVFW :: ~videoVFW()
{
  close();
}


/////////////////////////////////////////////////////////
// openDevice
//
/////////////////////////////////////////////////////////
bool videoVFW :: openDevice()
{
  char driverName[256];
  char driverDesc[256];
  if (capGetDriverDescription(0, driverName, 256, driverDesc, 256))
    post("videoVFW: driver '%s'", driverName);

  if(m_hWndC)closeDevice();
  
  // Connect to the daemon
  m_hWndC = capCreateCaptureWindow ((LPSTR) "GEM video",	// window name if pop-up 
				    0,				// window style (not visible)
				    0, 0, m_width, m_height,// window position and dimensions
				    GetDesktopWindow(), 0);
  if (!m_hWndC) {
    error("Unable to create capture window");
    return false;
  } 

  if (!capDriverConnect(m_hWndC, 0)) {
    error("Unable to connect to video driver");
    closeDevice();
    return false;
  }

  CAPTUREPARMS params;
  if (!capCaptureGetSetup(m_hWndC, &params, sizeof(CAPTUREPARMS))) {
    error("Unable to get capture parameters");
    closeDevice();
    return false;
  }

  params.fYield = TRUE;
  params.fCaptureAudio = FALSE;
  params.wPercentDropForError = 100;
  params.fLimitEnabled = FALSE;
  params.AVStreamMaster = AVSTREAMMASTER_NONE;
  params.fStepCaptureAt2x = FALSE;
  params.fAbortLeftMouse = FALSE;
  params.fAbortRightMouse = FALSE;
  if (!capCaptureSetSetup(m_hWndC, &params, sizeof(CAPTUREPARMS)))
    {
      error("Unable to set capture parameters");
      closeDevice();
      return false;
    }

  if (!capSetCallbackOnVideoStream(m_hWndC, videoVFW::videoFrameCallback))
    {
      error("Unable to set frame callback");
      closeDevice();
      return false;
    }
  if (!capSetUserData(m_hWndC, this))
    {
      error("Unable to set user data");
      closeDevice();
      return false;
    }
  DWORD formSize = capGetVideoFormat(m_hWndC, NULL, 0);
  BITMAPINFO *videoFormat = (BITMAPINFO *)(new char[formSize]);
  if (!capGetVideoFormat(m_hWndC, videoFormat, formSize))
    {
      error("Unable to get video format");
      closeDevice();
      return false;
    }

  videoFormat->bmiHeader.biWidth = m_width;
  videoFormat->bmiHeader.biHeight = m_height;
  videoFormat->bmiHeader.biBitCount = 24;
  videoFormat->bmiHeader.biCompression = BI_RGB;
  videoFormat->bmiHeader.biClrUsed = 0;
  videoFormat->bmiHeader.biClrImportant = 0;
  videoFormat->bmiHeader.biSizeImage = 0;
  if (!capSetVideoFormat(m_hWndC, videoFormat, formSize)) {
      error("Unable to set video format");
      delete videoFormat;
      closeDevice();
      return false;
    }
  if (!capGetVideoFormat(m_hWndC, videoFormat, formSize)) {
    error("Unable to get video format");
  }
  m_width=static_cast<int>(videoFormat->bmiHeader.biWidth);
  m_height=static_cast<int>(videoFormat->bmiHeader.biHeight);

  verbose(1, "Connected with %dx%d @ %d",
          m_width, m_height,
          static_cast<int>(videoFormat->bmiHeader.biBitCount));

  delete videoFormat;

  m_image.image.xsize = m_width;
  m_image.image.ysize = m_height;
  m_image.image.setCsizeByFormat(GL_RGBA);

  m_image.image.reallocate();
  m_image.image.setBlack();

  return true;
}

/////////////////////////////////////////////////////////
// closeDevice
//
/////////////////////////////////////////////////////////
void videoVFW :: closeDevice()
{
  if (m_hWndC) {
    capDriverDisconnect(m_hWndC);
    DestroyWindow(m_hWndC);
  }
  m_hWndC=NULL;
}


/////////////////////////////////////////////////////////
// videoFrame callback
//
/////////////////////////////////////////////////////////
void videoVFW :: videoFrame(LPVIDEOHDR lpVHdr)
{
  int count = lpVHdr->dwBytesUsed;
  // notice that it is times 3 for the color!
  // incoming data is BGR
  const int dataSize = m_image.image.xsize * m_image.image.ysize * 3;

  if (count < dataSize)
    {
      return;
    }

  lock();
  m_image.image.fromBGR(lpVHdr->lpData);
  m_image.newimage = true;
  unlock();
}
void videoVFW :: videoFrameCallback(HWND hWnd, LPVIDEOHDR lpVHdr)
{
  void *ptr = reinterpret_cast<pix_videoVFW*>(capGetUserData(hWnd));
  ptr->videoFrame(lpVHdr);
}


/////////////////////////////////////////////////////////
// render
//
/////////////////////////////////////////////////////////
bool videoVFW :: grabFrame()
{
  return true;
}

/////////////////////////////////////////////////////////
// startTransfer
//
/////////////////////////////////////////////////////////
bool videoVFW :: startTransfer()
{
  bool result= capCaptureSequenceNoFile(m_hWndC);
  m_image.newfilm=result;

  return result;
}

/////////////////////////////////////////////////////////
// stopTransfer
//
/////////////////////////////////////////////////////////
bool videoVFW :: stopTransfer()
{
  capCaptureStop(m_hWndC);
  return true;
}

/////////////////////////////////////////////////////////
// dimenMess
//
/////////////////////////////////////////////////////////
bool videoVFW :: setDimen(int x, int y, int leftmargin, int rightmargin, int topmargin, int bottommargin){
  video::setDImen(x, y, leftmargin, rightmargin, topmargin, bottommargin);
  if (m_hWndC) resetDevice();
}

/////////////////////////////////////////////////////////
// csMess
//
/////////////////////////////////////////////////////////
void videoVFW :: setColor(int format)
{
  if(format)m_image.image.setCsizeByFormat(format);
}

#else
videoVFW ::  videoVFW() { }
videoVFW :: ~videoVFW() { }
#endif