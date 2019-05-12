#include "gxl.inc.h"
#include "gxl.memory.h"
#include "gxl.file.h"

#include "FileAccessor/FileAccessor.h"
#include <sys/stat.h>

#ifndef INVALID_FILE_ATTRIBUTES
#define INVALID_FILE_ATTRIBUTES 0xffffffff
#endif

//#include "iphone_common.h"
//#define fopen(A, B) fopen_iphone(A, B)

#define INVALID_HANDLE_VALUE NULL

using namespace fileaccessor;

class iFileWin32 : public ref_countable_impl<iFileI>
{
public:

	std::shared_ptr<std::vector<char>> m_buffer = nullptr;
  sint32 m_buffer_pos = 0;

	iFileWin32()
	{
	}

	~iFileWin32()
	{
	  Close();
	}

	bool Open(const iStringT& fname)
	{
    m_buffer = FileAccessor::sharedFileAccessor()->getFileBuffer(RelativeFilePath(fname.CStr()), "r");
    m_buffer_pos = 0;
	  return (m_buffer != nullptr);
	}

	bool Create(const iStringT& fname)
	{
	  // m_hFile = fopen(CvtT2A<>(fname.CStr()), "w");
	  // return (m_hFile != INVALID_HANDLE_VALUE);
    return false;
	}

	void Close()
	{
	   m_buffer = nullptr;
	}

	// iFileI interface implementation
	uint32 Seek(sint32 pos, SeekMode smode = FSEEK_SET)
	{
	    switch(smode) {
	        case FSEEK_CUR:
	            m_buffer_pos += pos;
	            break;
	        case FSEEK_SET:
	            m_buffer_pos = pos;
	            break;
	        case FSEEK_END:
	            m_buffer_pos = m_buffer->size() + pos;
	            break;
	    }
        return pos;
	}

	uint32 GetPosition() const
	{
		return m_buffer_pos;
	}

	uint32 Read(void *buff, uint32 buffsize)
	{
    memcpy(buff, &((*m_buffer)[m_buffer_pos]), buffsize);
    m_buffer_pos += buffsize;
	  return buffsize;
	}

	uint32 Write(const void *buff, uint32 buffsize)
	{
    return 0;
	  // check(IsOpen());
	  // DWORD writen = fwrite(buff, 1, buffsize, m_hFile);
	  // return writen;
	}

	bool IsOpen() const
	{
	  return m_buffer != nullptr;
	}

	uint32 GetSize() const
	{
	   return m_buffer->size();
	}

	void Flush()
	{

	}
};

//
//
//

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


/*
 *	iFile static functions
 */
namespace iFile {

bool Exists(const iStringT& fname)
{
  return FileAccessor::sharedFileAccessor()->fileExists(RelativeFilePath(fname.CStr()));
}

bool Delete(const iStringT& fname)
{
  FileAccessor::sharedFileAccessor()->deleteFile(RelativeFilePath(fname.CStr()));
  return true;
}



bool Rename(const iStringT& fname, const iStringT& to_fname)
{
  return false;
}

bool Copy(const iStringT& fname, const iStringT& to_fname)
{
  return false;
}

bool Move(const iStringT& fname, const iStringT& to_fname)
{
	return Rename(fname,to_fname);
}

bool DirExists(const iStringT& dname)
{
  return FileAccessor::sharedFileAccessor()->fileExists(RelativeFilePath(dname.CStr()));
}

bool DirDelete(const iStringT& dname)
{
  auto absPath = FileAccessor::sharedFileAccessor()->resolve(RelativeFilePath(dname.CStr()));
  FileAccessor::sharedFileAccessor()->deleteDirectory(absPath);
  return true;
}

void DirRename(const iStringT& dname, const iStringT& to_dname)
{

}

bool DirCreate(const iStringT& dname)
{
  auto absPath = FileAccessor::sharedFileAccessor()->resolve(RelativeFilePath(dname.CStr()));
  FileAccessor::sharedFileAccessor()->createDirectory(absPath);
  return true;
}

uint32 GetSize(const iStringT& fname)
{
  auto buffer = FileAccessor::sharedFileAccessor()->getFileBuffer(RelativeFilePath(fname.CStr()), "r");
  return buffer->size();
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
    //str = _T("assets/");
}

void GetAppDocPath(iStringT& str)
{
  // str = "";
}

void GetAppLibPath(iStringT& str)
{
    // str = "";
}


} //namespace iFileName
