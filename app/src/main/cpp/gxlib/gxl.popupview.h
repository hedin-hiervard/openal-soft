#ifndef GXLIB_GXL_POPUPVIEW_H_
#define GXLIB_GXL_POPUPVIEW_H_

class iViewMgr;
class iPopupView : public iView
{
public:
	// c-tor, d-tor
	iPopupView(iViewMgr* pViewMgr);
	virtual ~iPopupView() { }

	// operations
	void TrackPopup(const iPoint& pos, const iRect& bound, Alignment al);
	void HidePopup();

	// virtuals
	virtual iSize PopupViewSize() 		= 0;
	virtual iSize PopupViewMinSize() 	= 0;
	virtual void OnTrackPopup(const iRect& clRect) {}
	virtual void OnHidePopup() {}
};



#endif //GXLIB_GXL_POPUPVIEW_H_
