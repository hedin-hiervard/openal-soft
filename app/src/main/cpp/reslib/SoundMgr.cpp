#include "he/he.std.h"
#include "he/he.tables.h"
#include "he/he.container.map.h"
#include "he/he.container.cache.h"
#include "he/he.file.h"
#include "he/he.file.name.h"

#include "pa/pa.xml.builder.h"
#include "pa/pa.map.h"
#include "pa/pa.sax.pathhandler.h"


#include "SoundMgr.h"

//////////////////////////////////////////////////////////////////////////
class iSoundXmlReader : public Sax::PathHandler
{
public:
	iSoundXmlReader(iSoundMgr& soundMgr)
	: m_soundMgr(soundMgr)
	{}

	bool OnEndDocument()
	{
		return true;
	}

	bool OnEnterElement( const iStringT& path, const Attributes& attr )
	{
		if (path == _T("HMMSoundSet")) {
		} else if (path == _T("HMMSoundSet.Group")) {
		} else if (path == _T("HMMSoundSet.Group.Item")) {
			if (!m_soundMgr.AddSoundItem(attr[_T("chunk")], attr[_T("id")], attr[_T("constId")] )) return false;
		}

		return true;
	}

	bool OnEditorCheck( const iStringT& group ){

	#ifndef BUILD_EXPORTER
		if( group == _T("common") )
			return( true );
	#endif

		return( false );
	}
	
	iSoundMgr&		m_soundMgr;
};
//////////////////////////////////////////////////////////////////////////
iSoundMgr::iSoundMgr()
{
}

iSoundMgr::~iSoundMgr()
{
}

bool iSoundMgr::Init(const iStringT& fileName)
{
	try {
		iFileName fn(fileName);
		m_resPath = fn.GetDevPath();
		// Load sprites
		iSoundXmlReader reader(*this);
		if (!Sax::Parse( fileName, &reader )) throw 0;
	} catch(int) {
		return false;
	}
	return true;
}

sint32 iSoundMgr::FindItem(const iStringT& soundId) const
{
	for (uint32 xx=0; xx<m_items.GetSize(); ++xx) {
		if (m_items[xx].soundId == soundId) return xx;
	}
	return -1;
}

bool iSoundMgr::AddSoundItem(const iStringT& fileName, const iStringT& soundId, const iStringT& constId)
{
	iStringT fname = m_resPath + fileName;
	iSmartPtr<iFileI> wavFile(OpenWin32File(fname));
	if ( !wavFile ) {
		MessageBox(NULL,iFormat(_T("Unable to open wave file %s !"),fname.CStr()).CStr(),_T("Error"),MB_OK);
		return false;
	}

	OutputDebugString(iFormat(_T("Loading sound file '%s'\n"), fname.CStr()).CStr());
	m_items.Add(iSoundItem(fname, soundId, constId));
	return true;
}
