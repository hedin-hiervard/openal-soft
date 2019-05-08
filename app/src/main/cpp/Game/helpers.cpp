#include "stdafx.h"
//#include "xxc/xxc.file.h"

sint32 GetRectsHeight(sint32 w, iSimpleArray<iSize>& szs)
{
	sint32 res = 0;
	sint32 curw = 0, curh = 0;
	for(uint32 xx=0; xx<szs.GetSize(); xx++) {
		curw += szs[xx].w;
		curh = iMAX<sint32>(curh, szs[xx].h);
		if(curw > w) {
			res += curh;
			curh = szs[xx].h;
			curw = szs[xx].w;
		}
	}	
	res += curh;
	return res;
}

iPoint GetRectOffset(sint32 w, uint32 idx, iSimpleArray<iSize>& szs)
{
	iPoint res;
	sint32 linestart = 0, lineend, linecnt;
	
	// traverse the list to find out our line
	sint32 curw = 0, curline = 0, curh = 0;
	bool bDone = false;
	for(uint32 xx=0; xx<szs.GetSize(); xx++) {
		curw += szs[xx].w;
		curh = iMAX<sint32>(curh, szs[xx].h);
		if(curw > w) {
			lineend = xx - 1;
			curline++;
			if(lineend >= idx) 
				bDone = true;
			else {
				res.y += curh;
				linestart = xx;
				curw = szs[linestart].w;
			}
		}
		if(bDone) { break; }
	}
	if(!bDone) lineend = szs.GetSize() - 1;
	
	// now, linestart <= idx <= lineend
	linecnt = lineend - linestart;
	
	if(linecnt % 2 == 0) { //even, not 0, with center element
		check(linecnt != 0);
		res.x = w / 2;
		sint32 s = (linestart+lineend)/2;
		if(s == idx) // our element is the center one, just center it
			res.x -= (sint32)szs[idx].w / 2;
		else {				
			sint32 sgn = iSIGN<sint32>(idx-s);	// the direction of movement
			sint32 add = iMIN<sint32>(sgn, 0);
			for(sint32 xx=s; xx != idx + add; xx += sgn) 
				res.x += (sgn * (sint32)szs[xx].w) / (xx == s ? 2 : 1); // move to half space for center element, full space for others
		}
	} else { // odd, with center space
		res.x = w / 2;
		double s = ((double)linestart + (double)lineend) / 2;
		sint32 sgn, fidx;
		if((double)idx > s) 
		{ 
			sgn = 1;
			fidx = ceil(s);
		}
		else
		{
			fidx = floor(s);
			sgn = -1;
		}
		sint32 add = iMIN<sint32>(sgn, 0);
		for(sint32 xx=fidx; xx != idx + add; xx += sgn)
			res.x += sgn * (sint32)szs[xx].w;
	}
	return res;
}

//////////////////////////////////////////////////////////////////////////
uint32 SelectSpells(iSpellList& spIds, iSpellFilter filter, uint32 count, MAGIC_SPELL reqSpell)
{
	check(count > 0 && spIds.GetSize() == 0);

	uint16 tsp[MSP_COUNT];
	uint32 tsp_cnt = 0;
	for (uint16 xx=0; xx<MSP_COUNT; ++xx) {
		if ((filter.typeMask & (1<<SPELL_DESCRIPTORS[xx].type)) && (filter.levelMask & (1<<SPELL_DESCRIPTORS[xx].level)) && (filter.schoolMask & (1<<SPELL_DESCRIPTORS[xx].school)) && xx != reqSpell) {
			tsp[tsp_cnt++] = xx;
		}
	}


	if (reqSpell != MSP_INVALID) count--;
	while (count) {
		sint32 rval = gGame.Map().Rand(tsp_cnt);
		spIds.Add(tsp[rval]);
		tsp[rval] = tsp[--tsp_cnt];
		count--;
	}

	if (reqSpell != MSP_INVALID) spIds.InsertBefore(gGame.Map().Rand(spIds.GetSize()+1), reqSpell);
	check(count == 0);

	return spIds.GetSize();
}

//////////////////////////////////////////////////////////////////////////
sint32 CalcRandValue(const uint8 pVals[], sint32 siz)
{
	check(siz>0);
	sint32 tot_vals=0;
	for (sint32 xx=0; xx<siz; ++xx) tot_vals += pVals[xx];
	if (!tot_vals) return -1;
	sint32 rval = gGame.Map().Rand(tot_vals);

	sint32 ridx=0;
	sint32 sval=pVals[ridx];
	while (sval<=rval) {
		ridx++;
		sval+=pVals[ridx];
	}

	check(pVals[ridx] != 0);

	return ridx;
}

sint32 InitialExpPts()
{
	return 50 + gGame.Map().Rand(100);
}

//////////////////////////////////////////////////////////////////////////
uint32 GetCurrentTimestamp()
{
#ifndef OS_APPLE
	SYSTEMTIME st;
	FILETIME ft;
	GetLocalTime(&st);
	SystemTimeToFileTime(&st, &ft);
	uint32 ut = (ft.dwLowDateTime + ((LONGLONG)(ft.dwHighDateTime)<<32)) / 600000000;
	return ut;
#else
	uint32 ut = time(0);
	return ut;
#endif

}


iStringT FormatDate(uint32 timestamp, bool bShowTime)
{
  #ifndef OS_APPLE
    FILETIME ft;
	SYSTEMTIME st;
	LONGLONG ll;
	ll = Int32x32To64(timestamp, 600000000);
	ft.dwLowDateTime = (DWORD)ll;
	ft.dwHighDateTime = ll >> 32;
	FileTimeToSystemTime(&ft, &st);
	if (bShowTime) return iFormat(_T("%d %s %d (%d:%02d)"), st.wDay, gTextMgr[TRID_MONTH_JAN+st.wMonth-1], st.wYear, st.wHour, st.wMinute);
	else return iFormat(_T("%d %s %d"), st.wDay, gTextMgr[TRID_MONTH_JAN+st.wMonth-1], st.wYear);
#else
	time_t tim=timestamp;
	tm *now=localtime(&tim);

	if (bShowTime) return iFormat(_T("%d %s %d (%d:%02d)"), now->tm_mday , gTextMgr[TRID_MONTH_JAN+now->tm_mon], 1900 + now->tm_year, now->tm_hour, now->tm_min);
	else return iFormat(_T("%d %s %d"), now->tm_mday, gTextMgr[TRID_MONTH_JAN+now->tm_mon], 1900 + now->tm_year);
#endif
}

iStringT FormatDateDM(uint32 timestamp, bool bShowTime)
{
#ifndef OS_APPLE
    FILETIME ft;
    SYSTEMTIME st;
    LONGLONG ll;
    ll = Int32x32To64(timestamp, 600000000);
    ft.dwLowDateTime = (DWORD)ll;
    ft.dwHighDateTime = ll >> 32;
    FileTimeToSystemTime(&ft, &st);
    if (bShowTime) return iFormat(_T("%d %s (%d:%02d)"), st.wDay, gTextMgr[TRID_MONTH_JAN+st.wMonth-1], st.wHour, st.wMinute);
    else return iFormat(_T("%d %s"), st.wDay, gTextMgr[TRID_MONTH_JAN+st.wMonth-1]);
#else
    time_t tim=timestamp;
    tm *now=localtime(&tim);

    if (bShowTime) return iFormat(_T("%d %s (%d:%02d)"), now->tm_mday , gTextMgr[TRID_MONTH_JAN+now->tm_mon], now->tm_hour, now->tm_min);
    else return iFormat(_T("%d %s"), now->tm_mday, gTextMgr[TRID_MONTH_JAN+now->tm_mon]);
#endif
}

iStringT FormatTime(uint32 timestamp)
{
#ifndef OS_APPLE
    FILETIME ft;
    SYSTEMTIME st;
    LONGLONG ll;
    ll = Int32x32To64(timestamp, 600000000);
    ft.dwLowDateTime = (DWORD)ll;
    ft.dwHighDateTime = ll >> 32;
    FileTimeToSystemTime(&ft, &st);
    return iFormat(_T("(%d:%02d)"), st.wHour, st.wMinute);
#else
    time_t tim=timestamp;
    tm *now=localtime(&tim);

    return iFormat(_T("(%d:%02d)"), now->tm_hour, now->tm_min);    
#endif
}

//////////////////////////////////////////////////////////////////////////

bool SaveGameFile( const iStringT& fname, iGameWorld& gmap, bool MP )
{
#ifndef OS_APPLE
	// Check and create save directory (if not exists)
	iStringT saveDir = gSavePath.Left(gSavePath.Length()-1);
	bool dirIsOk = iFile::DirExists(saveDir);
	if ( !dirIsOk ) {
		dirIsOk = iFile::DirCreate(saveDir);
	}
	if (!dirIsOk) {
		DWORD dwErr = GetLastError();
		iTextDlg tdlg(&gApp.ViewMgr(), _T(""), iFormat(_T("Unable to create '%s' folder: ErrorCode: 0x%08X"), saveDir.CStr(), dwErr), PID_NEUTRAL);
		tdlg.DoModal();

	}
#endif
	
	// normally we should create it in the save directory
	iStringT tempSaveName( gSavePath + _T("tempsave.tmp") );
#ifdef HMM_COMPOVERSION
	iFilePtr pFile( xxc::CreateXFile( tempSaveName, HMM_COMPOCODE ) );
#else
	iFilePtr pFile( CreateWin32File( tempSaveName ) );
#endif

	if ( !pFile ) {
#ifndef OS_APPLE
		DWORD dwErr = GetLastError();
		iTextDlg dlg( &gApp.ViewMgr(), _T("Failure"), iFormat(_T("Unable to save [%s], ErrorCode: 0x%08X"), fname.CStr(), dwErr), PID_NEUTRAL);
		dlg.DoModal();
#endif
		return false;
	}
/*
	// a small hack for displaying "saving..." message while saving 
	
	iTextDlg dlg( &gApp.ViewMgr(), _T("Saving..."), _T(""), PID_NEUTRAL );
	dlg.Center();
	dlg.OnCreateDlg();
	dlg.SetVisible();
	gApp.ViewMgr().PushModalDlg(&dlg);
	gApp.ForceCompose();
	gApp.ViewMgr().PopModalDlg();
*/	
	if ( !gGame.Map().SaveToFile( pFile.get(), MP ) ) {
		iTextDlg dlg( &gApp.ViewMgr(), _T("Failure"), _T("Unable to save game!"), PID_NEUTRAL );
		dlg.DoModal();
		return false;
	}
	// close file
	pFile.reset();
	// rename file to the destination
	iFile::Delete( fname );
	iFile::Rename( tempSaveName, fname );
	iFile::Delete( tempSaveName );
	
	gGame.AddMsg(iStringT(_T("#F6B6")) + gTextMgr[TRID_MSG_GAME_SAVED]);          
	
	
	return true;
}


