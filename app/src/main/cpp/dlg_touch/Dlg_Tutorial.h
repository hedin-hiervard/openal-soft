//
//  Dlg_Tutorial.cpp
//  pheroes
//
//  Created by Andrey Kiryushkin on 1/30/12.
//  Copyright (c) 2012 Palm Heroes Team. All rights reserved.
//

class iDlg_Tutorial : public iBaseGameDlg
{
public:
	iDlg_Tutorial(iViewMgr* pViewMgr, TextResId text, TUT_MSG tut);
    
private:
	void OnCreateDlg();
	void DoCompose(const iRect& rect);
	iSize ClientSize() const;
	void iCMDH_ControlCommand(iView* pView, CTRL_CMD_ID cmd, sint32 param);
	inline void OnMouseUp(const iPoint &pos, MouseId mID, MouseButtonId mbID) {}
#ifdef PC_VERSION
    void UpdateSize(){}
#endif
private:
    TextResId m_text;
    TUT_MSG m_tut;
};

