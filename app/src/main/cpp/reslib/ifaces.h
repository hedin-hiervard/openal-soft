#ifndef RESLIB_IFACES_H_
#define RESLIB_IFACES_H_

//////////////////////////////////////////////////////////////////////////
inline iPoint ParsePnt( const iStringT& str )
{
	sint32 commaPos = str.Find( _T(',') );
	if ( commaPos < 1 ) return iPoint(0,0);
	return iPoint( 
		str.Left(commaPos).GetInt(), 
		str.Mid( commaPos+1 ).GetInt() 
		);
}

//////////////////////////////////////////////////////////////////////////
interface IStatusBar
{
	virtual ~IStatusBar() {}
	virtual void SetText(const iStringT& text) = NULL;
};

#endif //RESLIB_IFACES_H_
