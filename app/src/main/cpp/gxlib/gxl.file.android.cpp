#include "gxl.inc.h"
#include "gxl.memory.h"
#include "gxl.file.h"

#include <sys/stat.h>

#ifndef INVALID_FILE_ATTRIBUTES
#define INVALID_FILE_ATTRIBUTES 0xffffffff
#endif

//#include "iphone_common.h"
//#define fopen(A, B) fopen_iphone(A, B)

#define INVALID_HANDLE_VALUE NULL

class iFileWin32 : public ref_countable_impl<iFileI>
{
public:

	FILE* m_hFile;

	iFileWin32()
	: m_hFile(INVALID_HANDLE_VALUE)
	{
	}

	~iFileWin32()
	{
	  Close();
	}

	bool Open(const iStringT& fname)
	{
	  m_hFile = fopen(CvtT2A<>(fname.CStr()), "r");
	  return (m_hFile != INVALID_HANDLE_VALUE);
	}

	bool Create(const iStringT& fname)
	{
	  m_hFile = fopen(CvtT2A<>(fname.CStr()), "w");
	  return (m_hFile != INVALID_HANDLE_VALUE);
	}

	void Close()
	{
	  if (m_hFile != INVALID_HANDLE_VALUE) {
	    fclose(m_hFile);
	    m_hFile = INVALID_HANDLE_VALUE;
	  }
	}

	// iFileI interface implementation
	uint32 Seek(sint32 pos, SeekMode smode = FSEEK_SET)
	{
		switch(smode) {
			case FSEEK_SET:
				return fseek(m_hFile, pos, SEEK_SET);
			case FSEEK_CUR:
				return fseek(m_hFile, pos, SEEK_CUR);
			case FSEEK_END:
				return fseek(m_hFile, pos, SEEK_CUR);
		}

	   //PORTTODO (not used?)
	  /*check(IsOpen());
	  uint32 win32SeekMode;
	  LARGE_INTEGER offset;
	  offset.QuadPart = pos;

	  switch (smode) {
	  case FSEEK_SET:		win32SeekMode = _BEGIN; break;
	  case FSEEK_CUR:		win32SeekMode = FILE_CURRENT; break;
	  case FSEEK_END:		win32SeekMode = FILE_END; break;
	  default:			win32SeekMode = FILE_BEGIN; break;
	  }

	  offset.LowPart = ::SetFilePointer(m_hFile, offset.LowPart, &offset.HighPart, win32SeekMode);
	  return offset.LowPart;
	  */
	}

	uint32 GetPosition() const
	{
		check(IsOpen());
		return ftell(m_hFile);
	}

	uint32 Read(void *buff, uint32 buffsize)
	{
	  check(IsOpen());
	  DWORD readed = fread(buff, 1, buffsize, m_hFile);
	  return readed;
	}

	uint32 Write(const void *buff, uint32 buffsize)
	{
	  check(IsOpen());
	  DWORD writen = fwrite(buff, 1, buffsize, m_hFile);
	  return writen;
	}

	bool IsOpen() const
	{
	  return (m_hFile != INVALID_HANDLE_VALUE);
	}

	uint32 GetSize() const
	{
	  uint32 pos = ftell(m_hFile);
	  fseek (m_hFile, 0, SEEK_END);
	  uint32 lSize = ftell(m_hFile);
	  fseek(m_hFile, pos, SEEK_SET);
	  return lSize;
	}

	void Flush()
	{
	  check(IsOpen());
	  fflush(m_hFile);
	}
};

//
//
//
#if !defined( UNDER_CE) && !defined (OS_APPLE) && !defined(OS_ANDROID)
class iFileWinStg : public ref_countable_impl<iFileI>
{
public:

	IStream* m_pStream;

	iFileWinStg(IStream *pstream)
	: m_pStream(pstream)
	{
	}

	~iFileWinStg()
	{
		m_pStream->Release();
		m_pStream = NULL;
	}

	// iFileI interface implementation
	uint32 Seek(sint32 pos, SeekMode smode = FSEEK_SET)
	{
		check(IsOpen());
		LARGE_INTEGER lgint;
		ULARGE_INTEGER newPos;
		lgint.QuadPart = pos;
		HRESULT hr = m_pStream->Seek(lgint,smode,&newPos);
		check(hr == S_OK);
		if ( hr == S_OK ) return newPos.LowPart;
		else return ~0UL;
	}

	uint32 GetPosition() const
	{
		check(IsOpen());
		ULARGE_INTEGER ulgint;
		LARGE_INTEGER lgint;
		lgint.QuadPart = 0;
		HRESULT hr = m_pStream->Seek(lgint,STREAM_SEEK_CUR,&ulgint);
		check(hr == S_OK);
		return (uint32)ulgint.QuadPart;
	}

	uint32 Read(void *buff, uint32 buffsize)
	{
		check(IsOpen());
		DWORD readed;
		HRESULT hr = m_pStream->Read(buff,buffsize,&readed);
		check(hr == S_OK);
		return readed;
	}

	uint32 Write(const void *buff, uint32 buffsize)
	{
		check(IsOpen());
		DWORD writen;
		HRESULT hr = m_pStream->Write(buff,buffsize,&writen);
		check(hr == S_OK);
		return writen;
	}

	bool IsOpen() const
	{
		return (m_pStream != NULL);
	}

	uint32 GetSize() const
	{
		STATSTG pst;
		HRESULT hr = m_pStream->Stat(&pst,STATFLAG_NONAME);
		check(hr == S_OK && &pst != NULL);
		return (uint32)pst.cbSize.QuadPart;
	}

	void Flush()
	{
	}
};
#endif

//
iFileI* CreateWin32File(const iStringT& fname)
{
	iFileWin32* result = new iFileWin32();
	if (!result->Create(fname)) {
		delete result;
		return NULL;
	}
	return result;
}

iFileI* OpenWin32File(const iStringT& fname)
{
	iFileWin32* result = new iFileWin32();
	if (!result->Open(fname)) {
		delete result;
		return NULL;
	}
	return result;
}

#if !defined(UNDER_CE) && !defined(OS_APPLE) && !defined(OS_ANDROID)
iFileI* CreateWinStgFile(IStream *stream)
{
	if (!stream) return NULL;
	return new iFileWinStg(stream);
}


iFileI* OpenWinStgFile(IStream *stream)
{
	if (!stream) return NULL;
	return new iFileWinStg(stream);
}
#endif


/*
 *	iFile static functions
 */
namespace iFile {

bool Exists(const iStringT& fname)
{
	FILE* f = fopen(CvtT2A<>(fname.CStr()), "r");
	if(f) {
		fclose(f);
		return true;
	} else
		return false;
}

bool Delete(const iStringT& fname)
{
  if ( !Exists(fname) ) return true;
  return !remove(CvtT2A<>(fname.CStr()));
}



bool Rename(const iStringT& fname, const iStringT& to_fname)
{
  if (rename(CvtT2A<>(fname.CStr()), CvtT2A<>(to_fname.CStr()))) {
  	return false;
  }
  return true;
}

bool Copy(const iStringT& fname, const iStringT& to_fname)
{
  //if (!Exists(fname) || !::CopyFileW(fname.CStr(), to_fname.CStr(), FALSE)) {
  //	return false;
  //}
  return true;
}

bool Move(const iStringT& fname, const iStringT& to_fname)
{
	return Rename(fname,to_fname);
}

bool DirExists(const iStringT& dname)
{

  //DWORD res = ::GetFileAttributesW(dname.CStr());
  //if ( res == INVALID_FILE_ATTRIBUTES ) return false;
  //else return (res & FILE_ATTRIBUTE_DIRECTORY) != 0;
}

bool DirDelete(const iStringT& dname)
{
  ///if ( !DirExists( dname ) ) return true;
  //return ::RemoveDirectoryW(dname.CStr()) ? true : false;
}

void DirRename(const iStringT& dname, const iStringT& to_dname)
{
  //if (!DirExists(dname)){
  //	return;
  //}
  //Rename(dname,to_dname);
}

bool DirCreate(const iStringT& dname)
{
	return mkdir(CvtT2A<>(dname.CStr()), 0777) == 0;
}

uint32 GetSize(const iStringT& fname)
{
  //HANDLE hFile = ::CreateFileW(fname.CStr(), GENERIC_READ, FILE_SHARE_READ | FILE_SHARE_WRITE, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
  //if (hFile == INVALID_HANDLE_VALUE) return 0;
  //uint32 size = ::GetFileSize(hFile,NULL);
  //if (size == 0xFFFFFFFF) return 0;
  //return size;
}

} // namespace iFile

/*
 *	FileName functions
 */
namespace iFileName {

void GetAppFullName(iStringT& str)
{
  //iCharW buff[MAX_PATH];
  //GetModuleFileName(0,buff,MAX_PATH);
  //str = buff;
}

void GetAppName(iStringT& str)
{
  //iStringT fname;
  //GetAppFullName(fname);
  //sint32 pos = fname.ReverseFind('\\');
  //if (pos != -1){
  //	str = fname.Mid(pos+1);
  //}
}



void GetAppPath(iStringT& str)
{
  // str = iStringT(CvtA2T<>(GetBundlePath()));
//#if defined(OS_IPHONE)

//#endif
//  iStringT fname;
//  GetAppFullName(fname);
/*#if defined(OS_MACOS)
  sint32 pos = str.ReverseFind('/');
  if (pos != -1){
  	str = str.Left(pos+1);
  }
#endif
 */

#ifdef OS_MACOS
	str += _T("/Contents/Resources");
#endif
	str += _T("/");
}

void GetAppDocPath(iStringT& str)
{
	// str = iStringT(CvtA2T<>(GetDocumentsPath())) + _T("/");
}

void GetAppLibPath(iStringT& str)
{
    // str = iStringT(CvtA2T<>(GetLibraryPath())) + _T("/");
}


} //namespace iFileName
