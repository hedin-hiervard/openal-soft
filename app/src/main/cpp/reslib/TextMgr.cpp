#include "he/he.std.h"
#include "he/he.tables.h"
#include "he/he.container.map.h"
#include "he/he.container.cache.h"
#include "he/he.container.simple.array.h"
#include "he/he.file.h"
#include "he/he.file.name.h"

#include "pa/pa.xml.builder.h"
#include "pa/pa.map.h"
#include "pa/pa.sax.pathhandler.h"

#include "xe/xe.std.h"
#include "xe/xe.dib.h"
#include "xe/xe.dc.hlp.h"

#include "si/si.std.h"
#include "si/si.wctrl.tree.h"


#include "common.h"
#include "cm_magic.h"
#include "cm_creatures.h"
#include "cm_castles.h"
#include "cm_constructions.h"
#include "Army.h"
#include "rtti.h"
#include "defines.h"
#include "objects.h"
#include "nodes.h"
#include "Artifacts.h"
#include "treasury_cnst.h"
#include "Constructions.h"

#include "SpriteMgr.h"
#include "AvatarMgr.h"
#include "TextMgr.h"
#include "SoundMgr.h"

#include "resmgr.h"



iStringT iTextMgr::sm_emptyStr = _T("");

struct iLangFileEntry {
	iStringT	key;
	iStringT	orig;
	iStringT	trans;
};

bool ParseLangLine(const iStringT& line, iLangFileEntry& entry)
{
	if (line[0] == _T('{')) {
		sint32 bidx = 0;
		sint32 len = 0;
		sint32 eidx = line.Find(_T('}'),bidx+1);
		if (eidx == -1) return false;
		len = eidx-bidx-1;
		check(len);
		entry.key = len?line.Mid(bidx+1,len):_T("");
		bidx = line.Find(_T('{'),eidx);
		if (bidx == -1) return false;
		eidx = line.Find(_T('}'),bidx);
		if (eidx == -1) return false;
		len = eidx-bidx-1;
		entry.orig = len?line.Mid(bidx+1,len):_T("");
		bidx = line.Find(_T('{'),eidx);
		if (bidx == -1) return false;
		eidx = line.Find(_T('}'),bidx);
		if (eidx == -1) return false;
		len = eidx-bidx-1;
		entry.trans = len?line.Mid(bidx+1,len):_T("");
		
		return true;
	}
	return false;
}


bool AddLanguageFile(char* buffer, int length, GAME_LANG lang, iTextMgr& tmgr, bool bPrimary) {
	iMemTextFileW langFile;
	iSimpleMemoryFile memFile(buffer, length);

	if (!langFile.Open(&memFile)) {
		MessageBox(NULL, iFormat(_T("Unable to open language file from memory!")).CStr(), NULL, MB_OK);
		return false;
	}

	if (bPrimary) {
		//check(tmgr.GetHash().Count() == 0);
		tmgr.Reset();
	} else {
		check(tmgr.GetHash().Count() != 0);
	}

	uint32 cnt = langFile.GetStringsCount();
	iLangFileEntry fentry;
	for (uint32 xx=0; xx<cnt; ++xx) {
		if (ParseLangLine(langFile.GetConstString(xx), fentry)) {
			if (bPrimary) {
				iTextMgr::iTextEntry& newEntry = tmgr.AddTextEntry(fentry.key,fentry.orig);
				newEntry.text[lang] = fentry.trans;
			} else {
				iTextMgr::iTextEntry* pEntry = tmgr.GetUnsafeEntryPtr(fentry.key);
				if (pEntry) {
					//if (pEntry->text[GLNG_ENGLISH].Compare(fentry.orig) == 0) {
					pEntry->text[lang] = fentry.trans;
					//} else {
					//MessageBox(0, iFormat(L"<%s> langauage file has different original text for key <%s>!\n<%s>\n<%s>", fileName.CStr(), fentry.key.CStr(), pEntry->text[GLNG_ENGLISH].CStr(), fentry.orig.CStr()).CStr(), 0, MB_OK);
					//}
				} else {
					MessageBox(0, iFormat(L"Invalid key <%s> in translation!", fentry.key.CStr()).CStr(), 0, MB_OK);
				}
			}
		}
	}

	// Validate language entries
	if (!bPrimary) {
		for (iTextMgr::iKVHash::ConstIterator it = tmgr.GetHash().First(); it != tmgr.GetHash().End(); ++it) {
			const iTextMgr::iTextEntry& entry = *it;
			if (!entry.text[GLNG_ENGLISH].Empty()) {
				if (entry.text[lang].Empty()) {
					//	MessageBox(0, iFormat(L"No translation for key <%s> in <%s>!", entry.key.CStr(), fileName.CStr()).CStr(), 0, MB_OK);
				}
			}
		}
	}

	return true;
}

bool AddLanguageFile(const iStringT& fileName, GAME_LANG lang, iTextMgr& tmgr, bool bPrimary)
{
	iTextFileW langFile;
	if (!langFile.Open(fileName)) {
		MessageBox(NULL, iFormat(_T("Unable to open language file <%s>!"), fileName.CStr()).CStr(), NULL, MB_OK);
		return false;
	}

	if (bPrimary) {
		//check(tmgr.GetHash().Count() == 0);
		tmgr.Reset();
	} else {
		check(tmgr.GetHash().Count() != 0);
	}

	uint32 cnt = langFile.GetStringsCount();
	iLangFileEntry fentry;
	for (uint32 xx=0; xx<cnt; ++xx) {
		if (ParseLangLine(langFile.GetConstString(xx), fentry)) {
			if (bPrimary) {
				iTextMgr::iTextEntry& newEntry = tmgr.AddTextEntry(fentry.key,fentry.orig);
				newEntry.text[lang] = fentry.trans;
			} else {
				iTextMgr::iTextEntry* pEntry = tmgr.GetUnsafeEntryPtr(fentry.key);
				if (pEntry) {
					//if (pEntry->text[GLNG_ENGLISH].Compare(fentry.orig) == 0) {
						pEntry->text[lang] = fentry.trans;
					//} else {
						//MessageBox(0, iFormat(L"<%s> langauage file has different original text for key <%s>!\n<%s>\n<%s>", fileName.CStr(), fentry.key.CStr(), pEntry->text[GLNG_ENGLISH].CStr(), fentry.orig.CStr()).CStr(), 0, MB_OK);
					//}
				} else {
					MessageBox(0, iFormat(L"Invalid key <%s> in <%s> translation <%s>!", fentry.key.CStr(), fileName.CStr()).CStr(), 0, MB_OK);
				}
			}
		}
	}

	// Validate language entries
	if (!bPrimary) {
		for (iTextMgr::iKVHash::ConstIterator it = tmgr.GetHash().First(); it != tmgr.GetHash().End(); ++it) {
			const iTextMgr::iTextEntry& entry = *it;
			if (!entry.text[GLNG_ENGLISH].Empty()) {
				if (entry.text[lang].Empty()) {
				//	MessageBox(0, iFormat(L"No translation for key <%s> in <%s>!", entry.key.CStr(), fileName.CStr()).CStr(), 0, MB_OK);
				}
			}
		}
	}

	return true;

}

