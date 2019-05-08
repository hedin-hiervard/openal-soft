#ifndef GXLIB_GXL_DIALOG_H_
#define GXLIB_GXL_DIALOG_H_

enum DLG_RETCODE {
	DRC_UNDEFINED = -1,
	DRC_OK,
	DRC_CANCEL,
	DRC_YES,
	DRC_NO,
	DRC_QUICK
};

class iDialog : public iView
{
public:
	iDialog(iViewMgr* pViewMgr);
	virtual ~iDialog() {}

	sint32 DoModal();

    void UpdateSize();

	// Pure virtuals
	virtual iSize GetDlgMetrics() const =0;
	virtual void OnCreateDlg() =0;
	virtual void OnPlace(iRect& rect) {}
	virtual bool Process(fix32 t) { return true; }
	
	// Virtuals
	virtual bool KeyDown(sint32 key) { return false; }
	virtual bool KeyUp(sint32 key) { return false; }

	virtual void Center();

#if defined( PC_VERSION )
	virtual void OnClickNo() {};
	virtual void OnClickYes() {};
	virtual void OnDown() {}
#endif	
protected:
	inline bool IsValidDialog() const { return m_retCode == DRC_UNDEFINED; }
	
	bool EndDialog(sint32 retCode);
	
	void Invalidate();
	sint32	m_retCode;

private:
};

#endif //GXLIB_GXL_DIALOG_H_
