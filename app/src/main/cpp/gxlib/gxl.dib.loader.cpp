#include "gxl.inc.h"
#include "gxl.memory.h"
#include "gxl.file.h"
#include "gxl.dib.h"

#ifdef OS_WINCE
#include "gxl.dib.loader.wce.cpp"
#endif

#ifdef OS_WIN32
#include "gxl.dib.loader.w32.cpp"
#endif

#ifdef OS_APPLE
#include "gxl.dib.loader.iphone.cpp"
#endif

#ifdef OS_ANDROID
#include "gxl.dib.loader.android.cpp"
#endif

iDibReader::iDibReader()
: pimpl( new iDibReaderImpl() )
{
}

iDibReader::~iDibReader()
{ Cleanup(); }

void iDibReader::Cleanup()
{ pimpl->Cleanup(); }

bool iDibReader::Init()
{ return pimpl->Init();	}


bool iDibReader::FromFile(iDib& dib, const iStringT& fname) const
{
	// if (!iFile::Exists(fname)) return false;
	// IMG_TYPE it;
	// if (fname.Right(4).CompareIgnoreCase(_T(".bmp")) == 0) it = IT_BMP;
	// else if (fname.Right(4).CompareIgnoreCase(_T(".gif")) == 0) it = IT_GIF;
	// else if (fname.Right(4).CompareIgnoreCase(_T(".jpg")) == 0) it = IT_JPG;
	// else return false;

	// iFilePtr pFile = OpenWin32File(fname);
	// if (!pFile) return false;
	// bool bRes = FromStream(dib,pFile.get(),it);
	// return bRes;
  return false;
}

bool iDibReader::FromFile(iPaletteDib& dib, const iStringT& fname) const
{
	// if (!iFile::Exists(fname)) return false;
	// IMG_TYPE it;
	// if (fname.Right(4).CompareIgnoreCase(_T(".bmp")) == 0) it = IT_BMP;
	// else if (fname.Right(4).CompareIgnoreCase(_T(".gif")) == 0) it = IT_GIF;
	// else return false;

	// iFilePtr pFile = NULL;
	// pFile = OpenWin32File(fname);
	// if (!pFile) return false;
	// bool bRes = FromStream(dib,pFile.get(),it);
	// return bRes;
  return false;
}

bool iDibReader::FromStream(iDib& dib, iFileI* pFile, IMG_TYPE it) const
{ return pimpl->FromStream(dib, pFile, it); }

bool iDibReader::FromStream(iPaletteDib& dib, iFileI* pFile, IMG_TYPE it) const
{ return pimpl->FromStream(dib, pFile, it); }

