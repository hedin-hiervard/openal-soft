#include "gxl.inc.h"
#include "gxl.logmgr.h"

//static LPCTSTR NEW_LINE = _T("\r\n");

iLogMgr::iLogMgr()
: m_pLogFile(NULL)
{
}

iLogMgr::~iLogMgr()
{}

bool iLogMgr::Init(const iStringT& fname)
{
	// m_pLogFile = CreateWin32File(fname);
	// if (!m_pLogFile) return false;
	// uint16 hdr = 0xFEFF;
	// m_pLogFile->Write(&hdr, sizeof(hdr));

	return true;
}

void iLogMgr::Log(const iStringT& entry)
{
	// if (!m_pLogFile) return;
	// m_pLogFile->Write(entry.CStr(), entry.Length()*sizeof(iCharT));
	// m_pLogFile->Flush();
}

