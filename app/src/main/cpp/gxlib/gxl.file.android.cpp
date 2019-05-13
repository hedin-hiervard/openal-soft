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
  std::shared_ptr<std::ostream> m_writing_stream = nullptr;

	iFileWin32()
	{
	}

	~iFileWin32()
	{
	  Close();
	}

	bool Open(const iStringT& fname)
	{
        return Open(RelativeFilePath(fname.CStr()));
	}

    bool Open(const RelativeFilePath& path)
    {
        m_buffer = FileAccessor::sharedFileAccessor()->getFileBuffer(path, "r");
        m_buffer_pos = 0;
        return (m_buffer != nullptr);
    }

  bool Create(const RelativeFilePath& path)
  {
    m_writing_stream = FileAccessor::sharedFileAccessor()->getWritingStream(path);
    return m_writing_stream != nullptr;
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
		auto to_read = iMIN(buffsize, m_buffer->size() - m_buffer_pos);
      memcpy(buff, &((*m_buffer)[m_buffer_pos]), to_read);
      m_buffer_pos += to_read;
	  return to_read;
	}

	uint32 Write(const void *buff, uint32 buffsize)
	{
    m_writing_stream->write((char*)buff, buffsize);
    return buffsize;
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
iFileI* CreateWin32File(const RelativeFilePath& path)
{
	iFileWin32* result = new iFileWin32();
	if (!result->Create(path)) {
		delete result;
		return NULL;
	}
	return result;
}

iFileI* OpenWin32File(const RelativeFilePath& path)
{
    iFileWin32* result = new iFileWin32();
    if (!result->Open(path)) {
        delete result;
        return NULL;
    }
    return result;
}

/*
 *	iFile static functions
 */
namespace iFile {

bool Exists(const RelativeFilePath& file)
{
  return FileAccessor::sharedFileAccessor()->fileExists(file);
}

bool Delete(const RelativeFilePath& file)
{
  FileAccessor::sharedFileAccessor()->deleteFile(file);
  return true;
}


bool Rename(const RelativeFilePath& from, const RelativeFilePath& to)
{
  	auto absFrom = FileAccessor::sharedFileAccessor()->resolve(from);
	auto absTo = FileAccessor::sharedFileAccessor()->resolve(to);
	std::rename(absFrom.to_string().c_str(), absTo.to_string().c_str());
	return true;
}

bool Copy(const RelativeFilePath& from, const RelativeFilePath& to)
{
  return false;
}

bool Move(const RelativeFilePath& from, const RelativeFilePath& to)
{
	return Rename(from, to);
}

bool DirExists(const RelativeFilePath& dir)
{
  return FileAccessor::sharedFileAccessor()->fileExists(dir);
}

bool DirDelete(const RelativeFilePath& dir)
{
  auto abs = FileAccessor::sharedFileAccessor()->resolve(dir);
  FileAccessor::sharedFileAccessor()->deleteDirectory(abs);
  return true;
}

void DirRename(const RelativeFilePath& from, const RelativeFilePath& to)
{

}

bool DirCreate(const RelativeFilePath& dir)
{
  auto absPath = FileAccessor::sharedFileAccessor()->resolve(dir);
  FileAccessor::sharedFileAccessor()->createDirectory(absPath);
  return true;
}

uint32 GetSize(const RelativeFilePath& file)
{
  auto buffer = FileAccessor::sharedFileAccessor()->getFileBuffer(file, "r");
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
