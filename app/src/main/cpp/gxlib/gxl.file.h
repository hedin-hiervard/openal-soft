#ifndef GXLIB_GXL_FILE_H_
#define GXLIB_GXL_FILE_H_

#include "xxl.ref.countable.h"
#include "xxl.ref.ptr.h"
#include "FileAccessor/FileAccessor.h"

/*
 *	iFileI interface
 */
class iFileI : public ref_countable
{
public:
	enum SeekMode {
		FSEEK_SET			= 0, // From the begin
		FSEEK_CUR			= 1, // From the current position
		FSEEK_END			= 2  // From the end
	};


	//
	virtual uint32 Seek(sint32 pos, SeekMode smode = FSEEK_SET) = 0;
	virtual uint32 GetPosition() const = 0;
	virtual uint32 Read(void *buff, uint32 buffsize) = 0;
	virtual uint32 Write(const void *buff, uint32 buffsize) = 0;
	virtual bool IsOpen() const = 0;
	virtual uint32 GetSize() const = 0;
	virtual void Flush() = 0;

protected:
	// made protected to prohibit delete operator
	virtual ~iFileI() {}
};

// smart ptr
typedef ref_ptr<iFileI> iFilePtr;

/*
 *	FileI helper
 *	NOTE: Not byte-order safe!
 */
class iFileStream
{
	iFilePtr f_;

	iFileStream( const iFileStream& );
	iFileStream& operator=( const iFileStream& );
public:

	iFileStream( const iFilePtr& f ) : f_(f) {}
	inline iFileI* Get() { return f_.get();}
	inline bool IsValid() const { return f_.get() != NULL; }

	void Seek( sint32 pos, iFileI::SeekMode mode = iFileI::FSEEK_SET )	{ f_->Seek( pos, mode);	}
	uint32 Position() const { return f_->GetPosition(); }
	uint32 Size() const { return f_->GetSize();	}

	inline bool Write( uint8 d )	{ return sizeof(uint8) == f_->Write( &d, sizeof(uint8) );}
	inline bool Write( sint8 d )	{ return sizeof(sint8) == f_->Write( &d, sizeof(sint8) );}
	inline bool Write( uint16 d )	{ return sizeof(uint16) == f_->Write( &d, sizeof(uint16) );}
	inline bool Write( sint16 d )	{ return sizeof(sint16) == f_->Write( &d, sizeof(sint16) );}
	inline bool Write( uint32 d )	{ return sizeof(uint32) == f_->Write( &d, sizeof(uint32) );}
	inline bool Write( sint32 d )	{ return sizeof(sint32) == f_->Write( &d, sizeof(sint32) );}
	inline bool Write( float d )	{ return sizeof(float) == f_->Write( &d, sizeof(float) );}


	inline bool Read( uint8& d )	{ return sizeof(uint8) == f_->Read( &d, sizeof(uint8) ); }
	inline bool Read( sint8& d )	{ return sizeof(sint8) == f_->Read( &d, sizeof(sint8) ); }
	inline bool Read( uint16& d )	{ return sizeof(uint16) == f_->Read( &d, sizeof(uint16) ); }
	inline bool Read( sint16& d )	{ return sizeof(sint16) == f_->Read( &d, sizeof(sint16) ); }
	inline bool Read( uint32& d )	{ return sizeof(uint32) == f_->Read( &d, sizeof(uint32) ); }
	inline bool Read( sint32& d )	{ return sizeof(sint32) == f_->Read( &d, sizeof(sint32) ); }
	inline bool Read( float& d )	{ return sizeof(float) == f_->Read( &d, sizeof(float) ); }

	inline bool Write( const void* p, size_t sz )	{ return sz == f_->Write( p, sz ); }
	inline bool Read( void* d, size_t sz )			{ return sz == f_->Read( d, sz ); }

	template<typename T>
	inline bool Write( const iStringTemplate<T>& str )
	{ return Write( str.Length() ) && Write( str.CStr(), sizeof(T) * str.Length() ); }

	bool Read( iStringT& str )
	{
		uint32 len;
		Read( len );
		if (len) {
			str.Set((uint32)0, len);
			Read( str.Str(), len * sizeof(iCharT));
			str.SetLength(len);
		} else str.SetEmpty();
		return true;
	}
};

/*
 *	Fabric functions
 */
iFileI* CreateWin32File(const fileaccessor::RelativeFilePath& path);
iFileI* OpenWin32File(const fileaccessor::RelativeFilePath& path);

#if !defined(UNDER_CE) && !defined(OS_APPLE) && !defined(OS_LINUX) && !defined(OS_ANDROID)
iFileI* CreateWinStgFile(IStream *stream);
iFileI* OpenWinStgFile(IStream *stream);

#endif
iFileI* CreateMemFile();
iFileI*	OpenMemFileReadOnly( iDynamicBuffer& buf );
iFileI*	OpenMemFileReadOnly( const void* ptr, size_t len );

//
// New in-memory file interface
// Uses arena object for memory management
// Thus allocation is government by the arena rules
// i.e. only in the stack order!



//
bool	CopyFile( iFileI* src, iFileI* dst );

/*
 *	iFile static functions
 */
namespace iFile {
	bool Exists(const fileaccessor::RelativeFilePath& path);
	bool Delete(const fileaccessor::RelativeFilePath& path);
	bool Rename(const fileaccessor::RelativeFilePath& from, const fileaccessor::RelativeFilePath& to);
  bool Copy(const fileaccessor::RelativeFilePath& from, const fileaccessor::RelativeFilePath& to);
	bool Move(const fileaccessor::RelativeFilePath& from, const fileaccessor::RelativeFilePath& to);
	bool DirExists(const fileaccessor::RelativeFilePath& dir);
	bool DirDelete(const fileaccessor::RelativeFilePath& dir);
	void DirRename(const fileaccessor::RelativeFilePath& from, const fileaccessor::RelativeFilePath& to);
	bool DirCreate(const fileaccessor::RelativeFilePath& dir);
	uint32 GetSize(const fileaccessor::RelativeFilePath& dir);
} // namespace iFile

/*
 *	FileName functions
 */
namespace iFileName {
	void GetAppFullName(iStringT& str);
	void GetAppName(iStringT& str);
	void GetAppPath(iStringT& str);

	void GetAppDocPath(iStringT& str);
#ifdef OS_IPHONE
	void GetAppLibPath(iStringT& str);
#endif
#ifdef OS_WIN32
	void GetAppDataPath(iStringT& str );
#endif
} //namespace iFileName


#endif //GXLIB_GXL_FILE_H_
