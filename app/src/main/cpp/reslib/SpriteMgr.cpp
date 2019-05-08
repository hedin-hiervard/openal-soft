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

#ifdef BUILD_EXPORTER
#include "he/he.crc32.h"
#endif


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

#include "ifaces.h"

extern iItemMgr gItemMgr;


#ifdef BUILD_EXPORTER


bool iSpriteDib::Init(iStringT &path)
{
    m_fn = path;
    m_hash = iTables::HashFn(path);
    return true;
}

bool iSpriteDib::LoadDib(iDib &dib) const
{
    return iDibLoader::Load(dib, m_fn);
}

#endif
/*
 *	
 */
void iSprite::Init(const iSpriteDib& _dib, const iPoint& _anchor, bool _spanned, bool _crop, bool _alpha, uint8 _shadow, const iRect& rc)
{
	id = -1;
	anchor = _anchor;
	spanned = _spanned;
	crop = _crop;
	shadow = _shadow;
	alpha = _alpha;
#ifndef BUILD_EXPORTER
	if (rc == cInvalidRect) 
		dib.Create(&_dib);
    else 
		dib.Create(&_dib,rc);
#else
    dib = _dib;
    dib.SetRect(rc);    
#endif
}

/*
 *	
 */
class iSpriteXmlReader : public Sax::PathHandler
{
public:
	iSpriteXmlReader(iSpriteMgr& spriteMgr)
	: m_SpriteMgr(spriteMgr)
	{}

	bool OnEndDocument()
	{
		return true;
	}

	bool OnEnterElement( const iStringT& path, const Attributes& attr )
	{
		if (path == _T("HMMSpriteSet")) {
		} else if (path == _T("HMMSpriteSet.Group")) {
			m_GroupName = attr[_T("id")];
		} else if (path == _T("HMMSpriteSet.Group.Item")) {
			uint8 shadow = 0;
			if (attr.Exists(_T("shadow"))) shadow = (uint8)attr[_T("shadow")].GetUInt();
			iPoint seq(1,1);
			if (attr.Exists(_T("sequence"))) seq = ParsePnt( attr[_T("sequence")]);
			bool spanned=true;
			if (attr.Exists(_T("spanned")) && !attr[_T("spanned")].GetUInt() ) spanned = false;
			bool alpha=false;
			if (attr.Exists(_T("alpha")) && attr[_T("alpha")].GetUInt() ) alpha = true;			
			if (!m_SpriteMgr.AddSprite(m_GroupName + _T(".") + attr[_T("id")], attr[_T("chunk")], ParsePnt( attr[_T("anchor")]), spanned, attr[_T("crop")].GetUInt() == 1, shadow, seq.x, seq.y, attr[_T("constId")], alpha )) return false;
		}

		return true;
	}

	bool OnEditorCheck( const iStringT& group ){

	#ifndef BUILD_EXPORTER
		if( group == _T("gui") || group == _T("castles") || group == _T("battle") || group == _T("flags") || group == _T("creatures") || group == _T("background") || group == _T("combat_hex") )
			return( true );
	#endif

		return( false );
	}
	
	iStringT		m_GroupName;
	iSpriteMgr&		m_SpriteMgr;
};

bool IsFirstIsMoreRecentThanSecond( const iStringT& first, const iStringT& second )
{
#pragma message("TODO: Write me!")
	return true;	// always rebuild cache for now
}

/*
 *	
 */
bool iSpriteMgr::Init(const iStringT& path, const iStringT& fname)
{
	try {

		/*iStringT cachePath = _T("c:\\__spritecache__");

		if ( iFile::Exists(cachePath) && !IsFirstIsMoreRecentThanSecond( fname, cachePath ) ) {
			iSmartPtr<iFileI> df2( OpenWin32File( cachePath ) );
			if ( df2 ) return InitFromCache( df2.Get() );
		}*/

		if(m_Packed) {
			iStringT fName = fname;
			fName.Replace(_T("\\"), _T("/"));
			iFileName fn(fName);

			int length;
			char* buffer = gItemMgr.GetZipBuffer(fName, length);

			iSpriteXmlReader reader(*this);

			if (!Sax::Parse(buffer, length, &reader)) throw 0;
		}
		else {
			iFileName fn(path + fname);
			m_ResPath = fn.GetDevPath();
			// Load sprites
			iSpriteXmlReader reader(*this);
			if (!Sax::Parse( path + fname, &reader )) throw 0;
		}

		// Normalize indeces
		uint32 constCount=0;
		for (uint32 xx=0; xx<m_ConstDefList.GetSize(); ++xx){
			if (m_ConstDefList[xx].m_SeqCnt == 1){
				operator[](m_ConstDefList[xx].m_SpriteName).id=constCount;
				constCount++;
			} else {
				for (uint32 zz=0; zz<m_ConstDefList[xx].m_SeqCnt; ++zz){
					iStringT nname = iFormat(_T("%s_%04d"),m_ConstDefList[xx].m_SpriteName.CStr(),zz);
					operator[](nname).id=constCount;
					constCount++;
				}
			}
		}

		uint32 idCounter = constCount;
		for (SpriteIt sit = m_SpriteHash.First(); sit != m_SpriteHash.End(); ++sit){
			if ( (*sit).m_Value.id == -1 ) {
				(*sit).m_Value.id = idCounter;
				idCounter++;
			}
		}

		OutputDebugString(iFormat(_T("Normilized %d sprites (%d const entries)\n"),idCounter,constCount).CStr());

		//iSmartPtr<iFileI> df( CreateWin32File( cachePath ) );
		//SaveCache( df.Get() );

	} catch ( int ) {
		return false;
	}
	return true;
}

#ifndef BUILD_EXPORTER
//////////////////////////////////////////////////////////////////////////
//
//

struct SerializeError {};

template< typename Except >
bool Throw()
{ throw Except(); return true; }

#define DECL_SERIALIZABLE( type ) \
void Serialize( iFileI* to, type v ) { \
	file.Write( to, v ) || Throw<SerializeError>(); \
} \
/* */

template< typename T >
void Serialize( iFileI* to, const T& v )
{
	iFileHandle file( to );
	file.Write( v ) || Throw<SerializeError>();
}

template< typename T >
void Deserialize( iFileI* to, T& v )
{
	iFileHandle file( to );
	file.Read( v ) || Throw<SerializeError>();
}

template< typename T >
void Serialize( iFileI* to, const T* v, uint32 num )
{
	iFileHandle file( to );
	// ?
	file.Write( v, sizeof(T) * num ) || Throw<SerializeError>();
}

template< typename T >
void Deserialize( iFileI* to, T* v, uint32 num )
{
	iFileHandle file( to );
	// ?
	file.Read( v, sizeof(T) * num ) || Throw<SerializeError>();
}

void SerializeBin( iFileI* to, const void* v, uint32 num )
{
	iFileHandle file( to );
	file.Write( v, num ) || Throw<SerializeError>();
}

void DeserializeBin( iFileI* to, void* v, uint32 num )
{
	iFileHandle file( to );
	file.Read( v, num ) || Throw<SerializeError>();
}


void Serialize( iFileI* to, const iPoint& pt )
{
	Serialize( to, pt.x );
	Serialize( to, pt.y );
}

void Deserialize( iFileI* to, iPoint& pt )
{
	Deserialize( to, pt.x );
	Deserialize( to, pt.y );
}

void Serialize( iFileI* to, const iSize sz )
{
	Serialize( to, sz.w );
	Serialize( to, sz.h );
}

void Deserialize( iFileI* to, iSize& sz )
{
	Deserialize( to, sz.w );
	Deserialize( to, sz.h );
}

void Serialize( iFileI* to, const iRect& rc )
{
	Serialize( to, (const iPoint&)rc );
	Serialize( to, (const iSize&)rc );
}

void Deserialize( iFileI* to, iRect& rc )
{
	Deserialize( to, (iPoint&)rc );
	Deserialize( to, (iSize&)rc );
}

void Serialize( iFileI* to, const iDib& dib )
{
	Serialize( to, dib.GetSize() );
	if ( ( dib.GetWidth() | dib.GetHeight() ) ) 
		SerializeBin(  to, dib.GetBits(), dib.GetByteSize() );
}

void Deserialize( iFileI* to, iDib& dib )
{
	iSize size;
	Deserialize( to, size );
	
	if ( !(size.w && size.h) ) {
		dib.Destroy();
		return;
	} 

	dib.Create( size ) || Throw<SerializeError>();
	DeserializeBin(  to, dib.GetBits(), dib.GetByteSize() );
}

template< typename HK, typename HV, typename HF  >
void Serialize( iFileI* to, const iHashTable<HK,HV,HF>& ht )
{
	typedef iHashTable<HK,HV,HF> HT;

	uint32 count = ht.Count();
	Serialize( to, count );
	for( HT::ConstIterator it = ht.First(); it != ht.End(); ++it ) {
		Serialize( to, *it );
	}		
}

template< typename HK, typename HV, typename HF  >
void Deserialize( iFileI* to, iHashTable<HK,HV,HF>& ht )
{
	typedef iHashTable<HK,HV,HF> HT;

	uint32 count;
	Deserialize( to, count );
	for( uint32 n = 0; n != count; ++n ) {
		HV val;
		Deserialize( to, val );
		ht.Insert( val );
	}
}

template< typename T >
void Serialize( iFileI* to, const iSimpleArray<T>& arr )
{
	Serialize( to, arr.GetSize() );
	for( uint32 n = 0; n != arr.GetSize(); ++n )
		Serialize( to, arr[n] );
}

template< typename T >
void Deserialize( iFileI* to, iSimpleArray<T>& arr )
{
	uint32 size;
	Deserialize( to, size );
	for( uint32 n = 0; n != size; ++n ) {
		T val;
		Deserialize( to, val );
		arr.Add( val );
	}
}

void Serialize( iFileI* to, const iSprite& sp )
{
	Serialize( to, sp.id );
	Serialize( to, sp.shadow );
	Serialize( to, sp.anchor );
	Serialize( to, sp.dib );
}

void Deserialize( iFileI* to, iSprite& sp )
{
	Deserialize( to, sp.id );
	Deserialize( to, sp.shadow );
	Deserialize( to, sp.anchor );
	Deserialize( to, sp.dib );
}

void Serialize( iFileI* to, const iConstDef& def )
{
	Serialize( to, def.m_SpriteName );
	Serialize( to, def.m_ConstName );
	Serialize( to, def.m_SeqCnt );
}

void Deserialize( iFileI* to, iConstDef& def )
{
	Deserialize( to, def.m_SpriteName );
	Deserialize( to, def.m_ConstName );
	Deserialize( to, def.m_SeqCnt );
}

void Serialize( iFileI* to, const iSpriteMgr::KeyVal & ci )
{
	Serialize( to, ci.m_Key );
	Serialize( to, ci.m_Value );
}

void Deserialize( iFileI* to, iSpriteMgr::KeyVal & ci )
{
	Deserialize( to, ci.m_Key );
	Deserialize( to, ci.m_Value );
}

#endif // BUILD_EXPORTER

bool
iSpriteMgr::InitFromCache(iFileI* to )
{
/*
	try {

		Deserialize( to, m_CellFactor );
		Deserialize( to, m_ResPath );
		Deserialize( to, m_InvalidSprite );

		Deserialize( to, m_pdggTiles );
		Deserialize( to, m_pdggTransWS );
		Deserialize( to, m_pdggTransMask );
		Deserialize( to, m_pdggGrid );
		Deserialize( to, m_pdggPathEl );
		Deserialize( to, m_pdggHeroes );
		Deserialize( to, m_pdggFlags );

		Deserialize( to, m_SpriteHash );
		Deserialize( to, m_ConstDefList );
	} catch (SerializeError&) {
		return false;
	}
*/
	return true;
}

bool
iSpriteMgr::SaveCache( iFileI* to )
{
/*
	try {

		Serialize( to, m_CellFactor );
		Serialize( to, m_ResPath );
		Serialize( to, m_InvalidSprite );

		Serialize( to, m_pdggTiles );
		Serialize( to, m_pdggTransWS );
		Serialize( to, m_pdggTransMask );
		Serialize( to, m_pdggGrid );
		Serialize( to, m_pdggPathEl );
		Serialize( to, m_pdggHeroes );
		Serialize( to, m_pdggFlags );

		Serialize( to, m_SpriteHash );
		Serialize( to, m_ConstDefList );

	} catch (SerializeError&) {
		return false;
	}
*/

	return true;
}


bool iSpriteMgr::AddSprite(const iStringT& name, const iStringT& chunk, const iPoint& anchor, bool spanned, bool crop, uint8 shadow, uint32 seqX, uint32 seqY, const iStringT& constName, bool alpha)
{   
    uint32 dib_width, dib_height;
#ifndef BUILD_EXPORTER
    iDib tdib;
#endif

	if (m_Packed) 
    {
#ifdef BUILD_EXPORTER
        MessageBox(NULL,_T("Exporter doesn't support packed resources!"),_T("Error"),MB_ICONERROR);;
        return false;
#else
		iStringT zipPath = _T("hmm/gfx/");
		zipPath += chunk;
		zipPath.Replace(_T("\\"), _T("/"));
		zipPath.Trim();

		int length;
		char* buffer = gItemMgr.GetZipBuffer(zipPath, length);
		check(buffer);

		if (!seqX || !seqY || !iDibLoader::Load(tdib, buffer, length))
        {
			MessageBox(NULL,iFormat(_T("Unable to open packed sprite %s !"),zipPath.CStr()).CStr(),_T("Error"),MB_OK);
			return false;
		}
#endif
	}
	else 
    {
#ifndef BUILD_EXPORTER
		if (!seqX || !seqY || !iDibLoader::Load(tdib, m_ResPath + chunk)) 
#else
        if (!seqX || !seqY || !iDibLoader::LoadOnlySize(m_ResPath + chunk, dib_width, dib_height)) 
#endif
        {
			MessageBox(NULL,iFormat(_T("Unable to open sprite %s !"),(m_ResPath + chunk).CStr()).CStr(),_T("Error"),MB_OK);
			return false;
		}
	}
	//OutputDebugString(iFormat(_T("Sprite [%s] loaded.\n"),(m_ResPath + chunk).CStr()).CStr());

#ifdef BUILD_EXPORTER
    iSpriteDib sdib;
    sdib.Init(m_ResPath + chunk);
#else
    dib_width = tdib.GetWidth();
    dib_height = tdib.GetHeight();
#endif

	if (seqX == 1 && seqY == 1)
    {
		iKVHash::Iterator it = m_SpriteHash.Insert(name);
#ifdef BUILD_EXPORTER
        (*it).m_Value.Init(sdib, anchor, spanned, crop, alpha, shadow);
#else
		(*it).m_Value.Init(tdib, anchor, spanned, crop, alpha, shadow);
#endif
	} 
    else
    {
		uint32 iw = dib_width / seqX;
		uint32 ih = dib_height / seqY;
		iRect rc(0,0,iw,ih);
		for (uint32 yy=0; yy<seqY; ++yy) 
        {
			for (uint32 xx=0; xx<seqX; ++xx) 
            {
				iStringT nname = iFormat(_T("%s_%04d"),name.CStr(),yy*seqX+xx);
				iKVHash::Iterator it = m_SpriteHash.Insert(nname);
#ifdef BUILD_EXPORTER
                (*it).m_Value.Init(sdib, anchor, spanned, crop, alpha, shadow, rc);
#else
				(*it).m_Value.Init(tdib, anchor, spanned, crop, alpha, shadow, rc);
#endif
				rc.x+=iw;
			}
			rc.y+=ih;
			rc.x=0;
		}
	}

	if (!constName.Empty()) {
		m_ConstDefList.Add(iConstDef(name,constName,seqX*seqY));
	}

	return true;
}

void iSpriteMgr::LoadMemPng(iDib &dib, iStringT name) {
	int length;
	char* buffer = gItemMgr.GetZipBuffer(name, length);
	iDibLoader::Load(dib, buffer, length);
}

