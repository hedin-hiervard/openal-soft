/*
 *  FingerHelp.h
 *  pheroes
 *
 *  Created by Hedin on 7/20/11.
 *  Copyright 2011 Palm Heroes Team. All rights reserved.
 *
 */

#define FH_UP	 (1 << 0)
#define FH_LEFT  (1 << 1)
#define FH_RIGHT (1 << 2)

class iFingerHelp: public iDialog
{
public:
	iFingerHelp(iViewMgr* pViewMgr, uint32 flags);
	sint32 DoModal();
	void OnCompose();
	bool Process(fix32 t);
private:
	void OnCreateDlg();
	virtual iSize GetDlgMetrics() const;	
private:
	uint32 m_flags;
	fix32 m_timer;
	iDib m_underSrf;
};
