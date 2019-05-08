#ifndef GXLIB_GXL_TOPMOSTVIEW_H_
#define GXLIB_GXL_TOPMOSTVIEW_H_

class iViewMgr;
class iTopmostView : public iView
{
public:
	// c-tor, d-tor
	iTopmostView(iViewMgr* pViewMgr);
	virtual ~iTopmostView() { }

	// Message handler
	bool ProcessMessage(const iInput::iEntry& msg);
	bool KeyDown(sint32 key);
	bool KeyUp(sint32 key);

	virtual iSize GetDefaultSize() const=0; 
	
	// Overrides
	virtual bool OnKeyDown(sint32 key) { return false; }
	virtual bool OnKeyUp(sint32 key) { return false; }
};


#endif //GXLIB_GXL_TOPMOSTVIEW_H_
