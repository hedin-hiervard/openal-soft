#include "stdafx.h"
#include "GfxHlp.h"

// Gradients for common font styles

const iDib::pixel gradBtnText[13] = {
	RGB16(255,160,0), RGB16(255,164,10), RGB16(255,169,21), RGB16(255,175,36), RGB16(255,181,52), RGB16(255,188,68), RGB16(255,195,85), RGB16(255,203,105), RGB16(255,211,123), 
	RGB16(255,218,140), RGB16(255,224,155), RGB16(255,230,170), RGB16(255,235,180)
};

iGradient defGrad(gradBtnText, 13);

iTextComposer::FontConfig fc_common(FS_SMALL, iDibFont::ComposeProps(RGB16(255,180,64)));

// Common font styles
iTextComposer::FontConfig dlgfc_hdr(fc_common);
iTextComposer::FontConfig dlgfc_topic(fc_common);
iTextComposer::FontConfig dlgfc_plain(fc_common);
iTextComposer::FontConfig dlgfc_stopic(fc_common);
iTextComposer::FontConfig dlgfc_splain(fc_common);

iTextComposer::FontConfig btnfc_normal(fc_common);
iTextComposer::FontConfig btnfc_small(fc_common);
iTextComposer::FontConfig btnfc_pressed(fc_common);
iTextComposer::FontConfig btnfc_disabled(fc_common);

iTextComposer::FontConfig menufc_large(fc_common);
iTextComposer::FontConfig menufc_medium(fc_common);
iTextComposer::FontConfig menufc_small(fc_common);
iTextComposer::FontConfig menufc_xsmall(fc_common);

iTextComposer::FontConfig savefc_hdr(fc_common);
iTextComposer::FontConfig savefc_text(fc_common);
iTextComposer::FontConfig listfc_text(fc_common);
iTextComposer::FontConfig creatqfc(fc_common);
iTextComposer::FontConfig infobarfc(fc_common);
iTextComposer::FontConfig infobarfc_small(fc_common);
iTextComposer::FontConfig mapeventfc(fc_common);
iTextComposer::FontConfig rewardqfc(fc_common) ;
iTextComposer::FontConfig spellbookfc(fc_common);
iTextComposer::FontConfig cellevfc(fc_common);
iTextComposer::FontConfig minsetfc(fc_common);
iTextComposer::FontConfig badgefc(fc_common);
iTextComposer::FontConfig grfc_tap(fc_common);
iTextComposer::FontConfig grfc_title(fc_common);
iTextComposer::FontConfig grfc_text(fc_common);


iTextComposer::FontConfig castletitlefc(fc_common);
iTextComposer::FontConfig castletopicfc(fc_common);
iTextComposer::FontConfig castlebldfc(fc_common);
iTextComposer::FontConfig castlemarketfc(fc_common);
iTextComposer::FontConfig castleheronamefc(fc_common);

iTextComposer::FontConfig creatinfofc_normal(fc_common);
iTextComposer::FontConfig creatinfofc_small(fc_common);
iTextComposer::FontConfig creatinfofc_qty(fc_common);

iTextComposer::FontConfig newmenufc_credits(fc_common);
iTextComposer::FontConfig newmenu_14fc(fc_common);
iTextComposer::FontConfig newmenufc_low_text(fc_common);
iTextComposer::FontConfig newmenufc_med_text(fc_common);
iTextComposer::FontConfig newmenufc_high_text(fc_common);
iTextComposer::FontConfig newmenufc_big_text(fc_common);
iTextComposer::FontConfig newmenufc_lrg_text(fc_common);
iTextComposer::FontConfig newmenufc_grt_text(fc_common);

iTextComposer::FontConfig herolevelfc(fc_common);
iTextComposer::FontConfig secskillnamefc(fc_common);
iTextComposer::FontConfig secskilllevelfc(fc_common);

iTextComposer::FontConfig spellscrollfc(fc_common);

iTextComposer::FontConfig ovrlndfc_herolvl(fc_common);
iTextComposer::FontConfig ovrlndfc_heroskills(fc_common);
iTextComposer::FontConfig ovrlndfc_infobar(fc_common);
iTextComposer::FontConfig ovrlndfc_infobar_small(fc_common);

iTextComposer::FontConfig ovrlndfc_heroexp(fc_common);

void InitFonts()
{
	uint32 width = gApp.Surface().GetWidth();
	
	if(width < 480) { 
		dlgfc_hdr = iTextComposer::FontConfig(FS_PT_16, iDibFont::ComposeProps(RGB16(255,180,64)));
		dlgfc_topic = iTextComposer::FontConfig(FS_PT_14, iDibFont::ComposeProps(RGB16(255,255,0)));
		dlgfc_plain = iTextComposer::FontConfig(FS_PT_14, iDibFont::ComposeProps(RGB16(210,210,220)));
		dlgfc_stopic = iTextComposer::FontConfig(FS_PT_10, iDibFont::ComposeProps(RGB16(255,255,0)));
		dlgfc_splain = iTextComposer::FontConfig(FS_PT_10, iDibFont::ComposeProps(RGB16(210,210,220)));
		
		btnfc_normal = iTextComposer::FontConfig(FS_PT_14, iDibFont::ComposeProps(defGrad, cColor_Black, iDibFont::DecBorder ));
		btnfc_pressed = iTextComposer::FontConfig(FS_PT_14, iDibFont::ComposeProps(defGrad, cColor_Black, iDibFont::DecBorder ));
		btnfc_disabled = iTextComposer::FontConfig(FS_PT_14, iDibFont::ComposeProps(RGB16(160,96,32), cColor_Black, iDibFont::DecBorder ));
	} else if(width < 1024) {
#ifdef PC_VERSION
        dlgfc_hdr = iTextComposer::FontConfig(FS_PT_30, iDibFont::ComposeProps(RGB16(255,180,64)));
        dlgfc_topic = iTextComposer::FontConfig(FS_PT_16, iDibFont::ComposeProps(RGB16(255,255,0)));
        dlgfc_plain = iTextComposer::FontConfig(FS_PT_16, iDibFont::ComposeProps(RGB16(210,210,220)));
        dlgfc_stopic = iTextComposer::FontConfig(FS_PT_16, iDibFont::ComposeProps(RGB16(255,255,0)));
        dlgfc_splain = iTextComposer::FontConfig(FS_PT_16, iDibFont::ComposeProps(RGB16(210,210,220)));
        
        btnfc_normal = iTextComposer::FontConfig(FS_PT_24, iDibFont::ComposeProps(defGrad, cColor_Black, iDibFont::DecBorder ));
        btnfc_pressed = iTextComposer::FontConfig(FS_PT_24, iDibFont::ComposeProps(defGrad, cColor_Black, iDibFont::DecBorder ));
        btnfc_disabled = iTextComposer::FontConfig(FS_PT_24, iDibFont::ComposeProps(RGB16(160,96,32), cColor_Black, iDibFont::DecBorder ));
#else
        dlgfc_hdr = iTextComposer::FontConfig(FS_PT_24, iDibFont::ComposeProps(cColor_White));
		dlgfc_topic = iTextComposer::FontConfig(FS_PT_24, iDibFont::ComposeProps(cColor_White));
		dlgfc_plain = iTextComposer::FontConfig(FS_PT_16, iDibFont::ComposeProps(cColor_White));
		dlgfc_stopic = iTextComposer::FontConfig(FS_PT_24, iDibFont::ComposeProps(cColor_White));
		dlgfc_splain = iTextComposer::FontConfig(FS_PT_16, iDibFont::ComposeProps(cColor_White));
		
		/* button fonts */
		btnfc_normal = iTextComposer::FontConfig(FS_PT_16, iDibFont::ComposeProps(defGrad, cColor_Black, iDibFont::DecBorder));
		btnfc_small = iTextComposer::FontConfig(FS_PT_14, cColor_White);
		btnfc_pressed = iTextComposer::FontConfig(FS_PT_16, iDibFont::ComposeProps(defGrad, cColor_Black, iDibFont::DecBorder ));
		btnfc_disabled = iTextComposer::FontConfig(FS_PT_16, iDibFont::ComposeProps(RGB16(160,96,32), cColor_Black, iDibFont::DecBorder ));
		
		menufc_large = iTextComposer::FontConfig(FS_PT_30, iDibFont::ComposeProps(defGrad, cColor_Black, iDibFont::DecBorder ));
		menufc_medium = iTextComposer::FontConfig(FS_PT_24, iDibFont::ComposeProps(defGrad, cColor_Black, iDibFont::DecBorder ));
		menufc_small = iTextComposer::FontConfig(FS_PT_14, iDibFont::ComposeProps(defGrad, cColor_Black, iDibFont::DecBorder ));
		menufc_xsmall = iTextComposer::FontConfig(FS_PT_14, iDibFont::ComposeProps(defGrad, cColor_Black, iDibFont::DecBorder ));
		
		savefc_hdr = iTextComposer::FontConfig(FS_PT_16, iDibFont::ComposeProps(RGB16(255,255,0)));
		savefc_text = iTextComposer::FontConfig(FS_PT_16, iDibFont::ComposeProps(RGB16(210,210,220)));
		listfc_text = iTextComposer::FontConfig(FS_PT_16,RGB16(255,128,128));
		creatqfc = iTextComposer::FontConfig(FS_PT_14, iDibFont::ComposeProps(RGB16(255,255,192), RGB16(48,48,64)));
		infobarfc = iTextComposer::FontConfig(FS_PT_14, iDibFont::ComposeProps(RGB16(255,220,192),RGB16(48,32,0)));		
		infobarfc_small = iTextComposer::FontConfig(FS_PT_10, iDibFont::ComposeProps(RGB16(255,220,192),RGB16(48,32,0)));		
		mapeventfc = iTextComposer::FontConfig(FS_PT_14, iDibFont::ComposeProps(cColor_White, cColor_Black,iDibFont::DecBorder));
		rewardqfc = iTextComposer::FontConfig(FS_PT_14, cColor_Yello);
		spellbookfc = iTextComposer::FontConfig(FS_PT_14, cColor_Yello);
		cellevfc = iTextComposer::FontConfig(FS_PT_14, iDibFont::ComposeProps(defGrad, cColor_Black, iDibFont::DecBorder));
		minsetfc = iTextComposer::FontConfig(FS_PT_14, cColor_White);		
		badgefc = iTextComposer::FontConfig(FS_PT_14, cColor_White);		
		
		/* castle fonts */
		castletitlefc = iTextComposer::FontConfig(FS_PT_30, cColor_White);		
		castletopicfc = iTextComposer::FontConfig(FS_PT_24, cColor_White);				
		castlebldfc = iTextComposer::FontConfig(FS_PT_16, RGB16(255, 255, 255));						
		castlemarketfc = iTextComposer::FontConfig(FS_PT_14, RGB16(255, 255, 255));						
		castleheronamefc = iTextComposer::FontConfig(FS_PT_14, RGB16(255, 255, 255));								
		spellscrollfc = iTextComposer::FontConfig(FS_PT_14, RGB16(255, 255, 255));								
		
		/* menu fonts */
        uint16 menufontclr = cColor_Black;//RGB16(128,128,128);
		newmenufc_credits   = iTextComposer::FontConfig(FS_PT_14, menufontclr);
        newmenufc_low_text  = iTextComposer::FontConfig(FS_PT_10, menufontclr);
        newmenufc_med_text  = iTextComposer::FontConfig(FS_PT_14, menufontclr);
        newmenufc_high_text = iTextComposer::FontConfig(FS_PT_14, menufontclr);
        newmenufc_big_text  = iTextComposer::FontConfig(FS_PT_16, menufontclr);
        newmenufc_lrg_text  = iTextComposer::FontConfig(FS_PT_24, menufontclr);
        newmenufc_grt_text  = iTextComposer::FontConfig(FS_PT_30, menufontclr);
		
		
		/* hero fonts */
		herolevelfc  = iTextComposer::FontConfig(FS_PT_10, RGB16(255,255,0));
		secskillnamefc  = iTextComposer::FontConfig(FS_PT_14, RGB16(225,223,66));
		secskilllevelfc  = iTextComposer::FontConfig(FS_PT_14, RGB16(206, 199, 206));
		
		/* creature info view fonts */
		creatinfofc_normal = iTextComposer::FontConfig(FS_PT_16, cColor_White);
		creatinfofc_small = iTextComposer::FontConfig(FS_PT_14, cColor_White);
		creatinfofc_qty = iTextComposer::FontConfig(FS_PT_14, cColor_White);
		
		
		/* game result screen */ 
		grfc_tap = iTextComposer::FontConfig(FS_PT_16, RGB16(160, 160, 160));
		grfc_title = iTextComposer::FontConfig(FS_PT_30, cColor_White);
		grfc_text = iTextComposer::FontConfig(FS_PT_16, cColor_White);
		
		/* overland fonts */
		ovrlndfc_herolvl = iTextComposer::FontConfig(FS_PT_14, RGB16(195, 154, 113));
		ovrlndfc_heroskills = iTextComposer::FontConfig(FS_PT_10, cColor_White);
		ovrlndfc_infobar = iTextComposer::FontConfig(FS_PT_14, RGB16(247, 214, 180));
		ovrlndfc_infobar_small = iTextComposer::FontConfig(FS_PT_10, RGB16(247, 214, 180));
		ovrlndfc_heroexp = iTextComposer::FontConfig(FS_PT_10, RGB16(195, 154, 113));
		
		/* setup legacy font sizes (S, M, L) */
		iTextComposer::FontSize fs[3];
		fs[0] = FS_PT_14;
		fs[1] = FS_PT_24;
		fs[2] = FS_PT_36;
		gTextComposer.SetRelSizes(fs);
#endif
	} else { // max resolution

		/* dialog fonts */
		/*dlgfc_hdr = iTextComposer::FontConfig(FS_PT_24, iDibFont::ComposeProps(RGB16(255,180,64)));
		dlgfc_topic = iTextComposer::FontConfig(FS_PT_24, iDibFont::ComposeProps(RGB16(255,255,0)));
		dlgfc_plain = iTextComposer::FontConfig(FS_PT_24, iDibFont::ComposeProps(RGB16(210,210,220)));
		dlgfc_stopic = iTextComposer::FontConfig(FS_PT_24, iDibFont::ComposeProps(RGB16(255,255,0)));
		dlgfc_splain = iTextComposer::FontConfig(FS_PT_24, iDibFont::ComposeProps(RGB16(210,210,220)));
		*/
		dlgfc_hdr = iTextComposer::FontConfig(FS_PT_30, iDibFont::ComposeProps(cColor_White));
		dlgfc_topic = iTextComposer::FontConfig(FS_PT_30, iDibFont::ComposeProps(cColor_White));
		dlgfc_plain = iTextComposer::FontConfig(FS_PT_24, iDibFont::ComposeProps(cColor_White));
		dlgfc_stopic = iTextComposer::FontConfig(FS_PT_24, iDibFont::ComposeProps(cColor_White));
		dlgfc_splain = iTextComposer::FontConfig(FS_PT_24, iDibFont::ComposeProps(cColor_White));

		/* button fonts */
		btnfc_normal = iTextComposer::FontConfig(FS_PT_24, iDibFont::ComposeProps(defGrad, cColor_Black, iDibFont::DecBorder));
		btnfc_small = iTextComposer::FontConfig(FS_PT_16, cColor_White);
		btnfc_pressed = iTextComposer::FontConfig(FS_PT_24, iDibFont::ComposeProps(defGrad, cColor_Black, iDibFont::DecBorder ));
		btnfc_disabled = iTextComposer::FontConfig(FS_PT_24, iDibFont::ComposeProps(RGB16(160,96,32), cColor_Black, iDibFont::DecBorder ));

		menufc_large = iTextComposer::FontConfig(FS_PT_36b, iDibFont::ComposeProps(defGrad, cColor_Black, iDibFont::DecBorder ));
		menufc_medium = iTextComposer::FontConfig(FS_PT_30, iDibFont::ComposeProps(defGrad, cColor_Black, iDibFont::DecBorder ));
		menufc_small = iTextComposer::FontConfig(FS_PT_16, iDibFont::ComposeProps(defGrad, cColor_Black, iDibFont::DecBorder ));
		menufc_xsmall = iTextComposer::FontConfig(FS_PT_16, iDibFont::ComposeProps(defGrad, cColor_Black, iDibFont::DecBorder ));

		savefc_hdr = iTextComposer::FontConfig(FS_PT_24, iDibFont::ComposeProps(RGB16(255,255,0)));
		savefc_text = iTextComposer::FontConfig(FS_PT_24, iDibFont::ComposeProps(RGB16(210,210,220)));
		listfc_text = iTextComposer::FontConfig(FS_PT_24,RGB16(255,128,128));
		creatqfc = iTextComposer::FontConfig(FS_PT_14, iDibFont::ComposeProps(RGB16(255,255,192), RGB16(48,48,64)));
		infobarfc = iTextComposer::FontConfig(FS_PT_14, iDibFont::ComposeProps(RGB16(255,220,192),RGB16(48,32,0)));		
		infobarfc_small = iTextComposer::FontConfig(FS_PT_10, iDibFont::ComposeProps(RGB16(255,220,192),RGB16(48,32,0)));		
		mapeventfc = iTextComposer::FontConfig(FS_PT_14, iDibFont::ComposeProps(cColor_White, cColor_Black,iDibFont::DecBorder));
		rewardqfc = iTextComposer::FontConfig(FS_PT_14, cColor_Yello);
		spellbookfc = iTextComposer::FontConfig(FS_PT_14, cColor_Yello);
		cellevfc = iTextComposer::FontConfig(FS_PT_16, iDibFont::ComposeProps(defGrad, cColor_Black, iDibFont::DecBorder));
		minsetfc = iTextComposer::FontConfig(FS_PT_14, cColor_White);		
		badgefc = iTextComposer::FontConfig(FS_PT_14, cColor_White);		
		
		/* castle fonts */
		castletitlefc = iTextComposer::FontConfig(FS_PT_36, cColor_White);		
		castletopicfc = iTextComposer::FontConfig(FS_PT_30, cColor_White);				
		castlebldfc = iTextComposer::FontConfig(FS_PT_24, RGB16(255, 255, 255));						
		castlemarketfc = iTextComposer::FontConfig(FS_PT_14, RGB16(255, 255, 255));						
		castleheronamefc = iTextComposer::FontConfig(FS_PT_16, RGB16(255, 255, 255));								
		spellscrollfc = iTextComposer::FontConfig(FS_PT_14, RGB16(255, 255, 255));								

		/* menu fonts */
		newmenufc_credits   = iTextComposer::FontConfig(FS_PT_24, RGB16(255,128,128));
        newmenufc_low_text  = iTextComposer::FontConfig(FS_PT_10, RGB16(255,128,128));
        newmenufc_med_text  = iTextComposer::FontConfig(FS_PT_14, RGB16(255,128,128));
        newmenufc_high_text = iTextComposer::FontConfig(FS_PT_16, RGB16(255,128,128));
        newmenufc_big_text  = iTextComposer::FontConfig(FS_PT_24, RGB16(255,128,128));
        newmenufc_lrg_text  = iTextComposer::FontConfig(FS_PT_30, RGB16(255,128,128));
        newmenufc_grt_text  = iTextComposer::FontConfig(FS_PT_36b, RGB16(255,128,128));
	
		
		/* hero fonts */
		herolevelfc  = iTextComposer::FontConfig(FS_PT_10, RGB16(255,255,0));
		secskillnamefc  = iTextComposer::FontConfig(FS_PT_10, RGB16(225,223,66));
		secskilllevelfc  = iTextComposer::FontConfig(FS_PT_10, RGB16(206, 199, 206));
		
		/* creature info view fonts */
		creatinfofc_normal = iTextComposer::FontConfig(FS_PT_24, cColor_White);
		creatinfofc_small = iTextComposer::FontConfig(FS_PT_14, cColor_White);
		creatinfofc_qty = iTextComposer::FontConfig(FS_PT_16, cColor_White);


		/* game result screen */ 
		grfc_tap = iTextComposer::FontConfig(FS_PT_24, RGB16(160, 160, 160));
		grfc_title = iTextComposer::FontConfig(FS_PT_36b, cColor_White);
		grfc_text = iTextComposer::FontConfig(FS_PT_24, cColor_White);
		
		/* overland fonts */
		ovrlndfc_herolvl = iTextComposer::FontConfig(FS_PT_14, RGB16(195, 154, 113));
		ovrlndfc_heroskills = iTextComposer::FontConfig(FS_PT_10, cColor_White);
		ovrlndfc_infobar = iTextComposer::FontConfig(FS_PT_16, RGB16(247, 214, 180));
		ovrlndfc_infobar_small = iTextComposer::FontConfig(FS_PT_10, RGB16(247, 214, 180));
		ovrlndfc_heroexp = iTextComposer::FontConfig(FS_PT_10, RGB16(195, 154, 113));
		
		/* setup legacy font sizes (S, M, L) */
		iTextComposer::FontSize fs[3];
		fs[0] = FS_PT_14;
		fs[1] = FS_PT_24;
		fs[2] = FS_PT_36;
		gTextComposer.SetRelSizes(fs);
	}

}

// CotullaHACK: look iTextComposer::GetCharHeight()
uint16 __GetCharSize(iTextComposer::FontSize fs)
{    
    switch (fs)
    {
    case FS_PT_10: return 10;
    case FS_PT_14: return 14;
    case FS_PT_16: return 16;
    case FS_PT_24: return 24;
    case FS_PT_30: return 30;
    case FS_PT_36: return 36;
	case FS_PT_36b: return 36;

    
	}    
    assert(0);
    return 0;
}


//
const sint32 DLG_SHADOW_OFFSET = 5;

// Dialog background shading

#define iabs(x) ( (x) < 0 ? -(x) : (x) )
#define ipow2(x) ( (x)*(x) )

sint32 dist_a( sint32 dx, sint32 dy )
{
	if ( dx > dy ) {
		return ( 61685 * dx + 26870 * dy ) >> 16;
	} else {
		return ( 61685 * dy + 26870 * dx ) >> 16;
	}
}

sint32 dist_b( sint32 dx, sint32 dy )
{
	return dx + dy - (dx < dy ? dx : dy) / 2;
}

struct grad_shader
{
	sint32	rx;
	sint32	ry;			
	sint32	rw;
	sint32	rh;
	
	enum { Max_Height = 256 };
	enum { Max_Radii  = 256+128 };
	
	uint32	vshade[ Max_Height ];
	uint32	rshade[ Max_Radii  ];
	
	void init( sint32 x, sint32 y, sint32 w, sint32 h )
	{
		rx = x;	ry = y;
		rw = w; rh = h;
	}
	
	void precalc()
	{
		// total presetup cost is:
		// height: sqrt, div
		// 256   : div
		const sint32 half = rh / 2;
		// vshade upper
		vshade[0] = 128;
		sint32 n;
		for( n = 0; n != half; n++ ) {
			sint32 arg = n;
			// base is: 1 - sqrt(x)
			sint32 val = 65535 - 256 * int_sqrt( (arg * 65536) / half );
			// safe 4'th power - faster falloff
			val >>= 8;
			val = val * val;
			val >>= 8;
			val = val * val;
			vshade[n] = val;
		}
		// vshare lower
		for( ; n != rh; n++ ) {
			sint32 arg = half - (n - half);
			// base is: 1 - sqrt(x)
			sint32 val = 65536 - 256 * int_sqrt( (arg * 65536) / half );
			vshade[n] = val >> 2;
		}
		
		// rshade [ 0..255..x ]
		sint32 radii = rh > rw ? rw : rh;
		check( radii <= Max_Radii );
		for( sint32 r = 0; r != Max_Radii; r++ ) {
			sint32 arg = (256 * r) / radii;
			sint32 val = 65536 - ( arg * arg );
			rshade[r] = val > 0 ? val >> 1 : 0;
		}
	}
	
	void draw( iDib::pixel* ptr, uint32 stride )
	{	
		// setup circle centre points
		sint32 px = rx + rw / 2;
		sint32 py = ry + rh - rh / 4;
		
		for( sint32 yy = 0; yy != rh; yy++ ) {
			iDib::pixel* pline = (ptr) + (yy+ry) * stride + rx;
			sint32 ady = iabs( py - (ry + yy) );
			// tight pixel loop
			for( sint32 xx = 0; xx != rw; xx++ ) {
				sint32 dx = px - (rx + xx);
				// fast eq.dist approximation
				sint32 dst = dist_a( iabs(dx), ady ); 
				// combined and normalized
				sint32 val = (rshade[ dst ] + vshade[ yy ] ) >> 8;
				// correct overflow (its possible to get rid of 
				// if we normalize beforehead, but wtf? )
				val = val > 255 ? 255 : val;
				// apply gamma and inverse
				val = 255 - (ipow2( val ) >> 8);
				// factor is ready to multiply: [0..255] range
				uint16 src = *pline;
				uint16 sr = val * ((src & (0x1F<<11)) >> 11);
				uint16 sg = val * ((src & (0x3F<<5)) >> 5);
				uint16 sb = val * ((src & (0x1F)));
				*pline++ = (((sr)>>8)<<11 | ((sg)>>8)<<5 | ((sb)>>8));
			}
		}
	}
};

// end of dialog background shading

enum QUANTITY_VAL {
	QVAL_FEW = 0,
	QVAL_SEVERAL,
	QVAL_PACK,
	QVAL_LOTS,
	QVAL_HORDE,
	QVAL_THRONG,
	QVAL_SWARM,
	QVAL_LEGION,
	QVAL_COUNT
};

inline QUANTITY_VAL QuantityVal(sint32 q)
{
	if (q < 5) return QVAL_FEW;
	else if (q < 10) return QVAL_SEVERAL;
	else if (q < 20) return QVAL_PACK;
	else if (q < 50) return QVAL_LOTS;
	else if (q < 100) return QVAL_HORDE;
	else if (q < 250) return QVAL_THRONG;
	else if (q < 500) return QVAL_SWARM;
	else return QVAL_LEGION;
}

iStringT FormatShortNumber(sint32 val)
{
	if (val >= 1000000) {
		return iFormat(_T("%dM"), val/1000000);
	} else if (val >= 1000) {
		return iFormat(_T("%dK"), val/1000);
	} else return iFormat(_T("%d"), val);
}

iStringT FormatNumber(sint32 val, bool bForseSign)
{
	iCharT postfix = 0;
	if (val >= 100000000) {
		val /= 1000000;
		postfix = 'M';
	} else if (val >= 100000) {
		val /= 1000;
		postfix = 'K';
	}

	iStringT result;
	if (bForseSign && val > 0) result += _T("+");
	result.Addf(_T("%d"), val);
	if (postfix)  result.Addf(_T("%c"), postfix);
	return result;
	//
}

iStringT CreatQuant2Text( sint32 q, VISION_LEVEL vl)
{
	if (vl == VL_BASIC) return gTextMgr[TRID_QUANT_FEW+QuantityVal(q)];
	else if (vl == VL_EXPERT) return FormatNumber(q);
	return iStringT(_T("?"));
}

iStringT CreatGroup2Text( CREATURE_TYPE ct, sint32 q, VISION_LEVEL vl)
{
	return CreatQuant2Text(q, vl) + _T(" ")+gTextMgr[TRID_CREATURE_PEASANT_F3+ct*3];
}

iStringT Mineral2Text( MINERAL_TYPE mt, sint32 quant, bool bBig /*= false*/ )
{
	return iFormat(_T("#I%04X%d"), bBig ? PDGG(RES_ICONS)+mt : PDGG(RES_MINI_ICONS)+mt, quant);
}

iStringT MineralSet2Text( const iMineralSet& ms, bool bBig /*= false*/ )
{
	iStringT res;
	for (uint32 xx=0; xx<MINERAL_COUNT; ++xx){
		if (ms.quant[xx]) res.Addf(_T("#I%04X%d  "), bBig ? PDGG(RES_ICONS)+xx : PDGG(RES_MINI_ICONS)+xx, ms.quant[xx]);
	}
	if (res.Empty()) res = _T("-");
	return res;
}

iStringT MineralSet2TextRelative(const iMineralSet& ms, const iMineralSet& of)
{
	iStringT res;
	for (uint32 xx=0; xx<MINERAL_COUNT; ++xx){
		if (ms.quant[xx]) res.Addf(_T("#I%04X%d/%d "),PDGG(RES_MINI_ICONS)+xx, ms.quant[xx], of.quant[xx]);
	}
	if (res.Empty()) res = _T("-");
	return res;
}

void Tile3x3Sprite(iDib& dib, const iRect& rc, SpriteId sid)
{
	iRect orc(rc);

	iSize ssiz = gGfxMgr.Dimension(sid);
	check( (ssiz.w % 3) == 0 && (ssiz.h % 3) == 0);
	sint32 elw = ssiz.w / 3;
	sint32 elh = ssiz.h / 3;
	
	sint32 xcnt = ((sint32)rc.w - elw * 2 + elw - 1) / elw;
	sint32 ycnt = ((sint32)rc.h - elh * 2 + elh - 1) / elh;
	sint32 xpos = rc.x + elw;
	sint32 ypos = rc.y + elh;
	sint32 xx,yy;
	
	for (yy=0; yy<ycnt; ++yy) {
		for (xx=0; xx<xcnt; ++xx){
			gGfxMgr.Blit(sid, dib, iRect(elw,elh,elw,elh), iPoint(xpos,ypos));
			xpos += elw;
		}
		ypos += elh;
		xpos = rc.x+elw;
	}
	
	xpos = rc.x+elw;
	for (xx=0; xx < xcnt; ++xx) {
		gGfxMgr.Blit(sid, dib, iRect(elw,0,elw,elh), iPoint(xpos,rc.y));
		gGfxMgr.Blit(sid, dib, iRect(elw,elh*2,elw,elh), iPoint(xpos,rc.y+rc.h-elh));
		xpos += elw;
	}
	
	ypos = rc.y+elh;
	for (yy=0; yy < ycnt; ++yy) {
		gGfxMgr.Blit(sid, dib, iRect(0,elh,elw,elh), iPoint(rc.x,ypos));
		gGfxMgr.Blit(sid, dib, iRect(elw*2,elh,elw,elh), iPoint(rc.x+rc.w-elw,ypos));
		ypos += elh;
	}
	
	gGfxMgr.Blit(sid, dib, iRect(0,0,elw,elh), iPoint(rc.x,rc.y));
	gGfxMgr.Blit(sid, dib, iRect(0,elh*2,elw,elh), iPoint(rc.x,rc.y+rc.h-elh));
	gGfxMgr.Blit(sid, dib, iRect(elw*2,0,elw,elh), iPoint(rc.x+rc.w-elw,rc.y));
	gGfxMgr.Blit(sid, dib, iRect(elw*2,elh*2,elw,elh), iPoint(rc.x+rc.w-elw,rc.y+rc.h-elh));
}


//////////////////////////////////////////////////////////////////////////
void FrameRoundRect(iDib& surf, const iRect& rect, iDib::pixel clr)
{
	surf.HLine(iPoint(rect.x+1, rect.y), rect.x+rect.w-2, clr);
	surf.HLine(iPoint(rect.x+1, rect.y+rect.h-1), rect.x+rect.w-2, clr);
	surf.VLine(iPoint(rect.x,rect.y+1), rect.y+rect.h-1,clr);
	surf.VLine(iPoint(rect.x+rect.w-1,rect.y+1), rect.y+rect.h-1,clr);
    
	/*
     * This is not out-of-screen drawing friendly,
     * because it doesn't check for DIB's bounds
     */
     
    /*surf.PutPixel(rect.x+1, rect.y+1, clr);
	surf.PutPixel(rect.x+rect.w-2, rect.y+1, clr);
	surf.PutPixel(rect.x+1, rect.y+rect.h-2, clr);
	surf.PutPixel(rect.x+rect.w-2, rect.y+rect.h-2, clr);
     */
}

void DrawRoundRect(iDib& surf, const iRect& rect, iDib::pixel fClr, iDib::pixel bClr)
{
	surf.FillRect(iRect(rect.x+1, rect.y+1, rect.w-2, rect.h-2), bClr);
	FrameRoundRect(surf, rect, fClr);
}


void ComposeDlgBkgnd(iDib& surf, const iRect& rect, bool bTransp)
{	
	//uint32 nval = (pid+1);
	uint32 toffs = 10;
	if (rect.IsEmpty()) return;
	
	if(rect.w <= 20 || rect.h <= 20) return; // too few space

	// tile background
	iRect bkRect(rect);	
	bkRect.DeflateRect(10);
	//gGfxMgr.BlitTile(bDecs?PDGG(BKTILE2):PDGG(BKTILE), surf,bkRect);
	surf.FillRect(bkRect, cColor_Black, bTransp ? 64 : 178);
		

	// top/bottom tiles
	gGfxMgr.BlitTile(PDGG(DLG_HTILES), surf, iRect(rect.x+toffs,rect.y + 8,rect.w-toffs*2,3));
	gGfxMgr.BlitTile(PDGG(DLG_HTILES), surf, iRect(rect.x+toffs,rect.y2()-9,rect.w-toffs*2, 3));

	// left/right tiles
	gGfxMgr.BlitTile(PDGG(DLG_VTILES), surf, iRect(rect.x + 8,rect.y+toffs,3, rect.h-toffs*2));
	gGfxMgr.BlitTile(PDGG(DLG_VTILES), surf, iRect(rect.x2()-9,rect.y+toffs,3, rect.h-toffs*2));
	
	// corners
	//if (bDecs){
//	gGfxMgr.Blit(PDGG(DLG_CORNERS)+0, surf, iPoint(rect.x+3,rect.y+3));
//	gGfxMgr.Blit(PDGG(DLG_CORNERS)+1, surf, iPoint(rect.x2()-30,rect.y+3));
//	gGfxMgr.Blit(PDGG(DLG_CORNERS)+2, surf, iPoint(rect.x+3,rect.y2()-30));
//	gGfxMgr.Blit(PDGG(DLG_CORNERS)+3, surf, iPoint(rect.x2()-30,rect.y2()-30));
}

uint16 opclr = RGB16(56, 64, 56);
uint16 trclr = RGB16(50, 49, 50);

void ComposeGlowBkgnd(iDib& surf, const iRect& rect, bool bTransp, bool bTopCorners)
{
    ComposeDlgBkgnd(surf, rect, false);
    return;
    
	if (rect.IsEmpty()) return;
    bTransp = true;
    
	if(rect.w < 46 || rect.h < 46) return; // too few space
    
	// fill background
	iRect bkRect(rect);	
	bkRect.DeflateRect(23);
    uint8 alpha = 221;
	
    if(bTransp)
		surf.FillRect(bkRect, trclr, alpha);
	else
		surf.FillRect(bkRect, opclr, 255);
	
    
	iRect top = AlignRect(iSize(rect.w-46, 10), rect, AlignTop) + iPoint(0, 2);
	iRect top2 = AlignRect(iSize(rect.w-46, 18), rect, AlignTop) + iPoint(0, 5);
	surf.FillRect(top2, trclr, alpha);
	iRect bot = AlignRect(iSize(rect.w-46, 10), rect, AlignBottom) - iPoint(0, 2);
	iRect bot2 = AlignRect(iSize(rect.w-46, 18), rect, AlignBottom) - iPoint(0, 5);
	surf.FillRect(bot2, trclr, alpha);
	iRect left = AlignRect(iSize(10, rect.h - 46), rect, AlignLeft) + iPoint(2, 0);
	iRect left2 = AlignRect(iSize(18, rect.h - 46), rect, AlignLeft) + iPoint(5, 0);
	surf.FillRect(left2, trclr, alpha);
	iRect right = AlignRect(iSize(10, rect.h - 46), rect, AlignRight) - iPoint(2, 0);
	iRect right2 = AlignRect(iSize(18, rect.h - 46), rect, AlignRight) - iPoint(5, 0);
	surf.FillRect(right2, trclr, alpha);
	if(!bTransp) {
		surf.FillRect(top2, opclr, 255);
		surf.FillRect(bot2, opclr, 255);
		surf.FillRect(left2, opclr, 255);
		surf.FillRect(right2, opclr, 255);
	}
    
	// top/bottom tiles
	gGfxMgr.BlitTile(PDGG(DLG_HTILES_GLOW), surf, top);
	gGfxMgr.BlitTile(PDGG(DLG_HTILES_GLOW), surf, bot);
    
	// left/right tiles
	gGfxMgr.BlitTile(PDGG(DLG_VTILES_GLOW), surf, left);
	gGfxMgr.BlitTile(PDGG(DLG_VTILES_GLOW), surf, right);
    
	
    
	// corners
	sint16 sid = PDGG(DLG_CORNERS_GLOW);
	if(bTopCorners) {
		gGfxMgr.Blit(sid+0, surf, AlignRect(iSize(23, 23), rect, AlignTopLeft));		
		gGfxMgr.Blit(sid+1, surf, AlignRect(iSize(23, 23), rect, AlignTopRight));
	}
	gGfxMgr.Blit(sid+2, surf, AlignRect(iSize(23, 23), rect, AlignBottomLeft));
	gGfxMgr.Blit(sid+3, surf, AlignRect(iSize(23, 23), rect, AlignBottomRight));
}

void ComposeGlowBkgndGrid(iDib& surf, const iRect& rect, iSize sz)
{
	if (rect.IsEmpty()) return;

	if(rect.w <= 40 || rect.h <= 40) return; // too few space

	// fill background
	iRect bkRect(rect);	
	bkRect.DeflateRect(23);

	surf.FillRect(bkRect, trclr, 103);
    
    iRect top = AlignRect(iSize(rect.w-46, 10), rect, AlignTop) + iPoint(0, 2);
	iRect top2 = AlignRect(iSize(rect.w-46, 18), rect, AlignTop) + iPoint(0, 5);
	surf.FillRect(top2, trclr, 103);
	iRect bot = AlignRect(iSize(rect.w-46, 10), rect, AlignBottom) - iPoint(0, 2);
	iRect bot2 = AlignRect(iSize(rect.w-46, 18), rect, AlignBottom) - iPoint(0, 5);
	surf.FillRect(bot2, trclr, 103);
	iRect left = AlignRect(iSize(10, rect.h - 46), rect, AlignLeft) + iPoint(2, 0);
	iRect left2 = AlignRect(iSize(18, rect.h - 46), rect, AlignLeft) + iPoint(5, 0);
	surf.FillRect(left2, trclr, 103);
	iRect right = AlignRect(iSize(10, rect.h - 46), rect, AlignRight) - iPoint(2, 0);
	iRect right2 = AlignRect(iSize(18, rect.h - 46), rect, AlignRight) - iPoint(5, 0);
/*
	iRect top = AlignRect(iSize(rect.w-46, 10), rect, AlignTop);
	iRect top2 = AlignRect(iSize(rect.w-46, 18), rect, AlignTop) + iPoint(0, 5);
	surf.FillRect(top2, trclr, 103);
	iRect bot = AlignRect(iSize(rect.w-46, 10), rect, AlignBottom) + iPoint(0, 1);
	iRect bot2 = AlignRect(iSize(rect.w-46, 18), rect, AlignBottom) - iPoint(0, 5);
	surf.FillRect(bot2, trclr, 103);
	iRect left = AlignRect(iSize(10, rect.h - 46), rect, AlignLeft);
	iRect left2 = AlignRect(iSize(18, rect.h - 46), rect, AlignLeft) + iPoint(5, 0);
	surf.FillRect(left2, trclr, 103);
	iRect right = AlignRect(iSize(10, rect.h - 46), rect, AlignRight) + iPoint(1, 0);
	iRect right2 = AlignRect(iSize(18, rect.h - 46), rect, AlignRight) - iPoint(5, 0);
*/	surf.FillRect(right2, trclr, 103);

	// top/bottom tiles	
	for(uint32 x=0; x<=sz.h; x++) {
		iRect tmp = top + iPoint(0, x*(bot.y - top.y) / sz.h);
		if(x != 0 && x != sz.h)
			tmp.InflateRect(18, 0);
		gGfxMgr.BlitTile(PDGG(DLG_HTILES_GLOW), surf, tmp);	
	}

	// left/right tiles
	for(uint32 x=0; x<=sz.w; x++) {
		iRect tmp = left + iPoint(x*(right.x - left.x) / sz.w, 0);
		if(x != 0 && x != sz.w)
			tmp.InflateRect(0, 18);
		gGfxMgr.BlitTile(PDGG(DLG_VTILES_GLOW), surf, tmp);	
	}
	
	// corners
//	gGfxMgr.Blit(PDGG(DLG_CORNERS_GLOW)+0, surf, AlignRect(iSize(23, 23), rect, AlignTopLeft));
//	gGfxMgr.Blit(PDGG(DLG_CORNERS_GLOW)+1, surf, AlignRect(iSize(23, 23), rect, AlignTopRight));
//	gGfxMgr.Blit(PDGG(DLG_CORNERS_GLOW)+2, surf, AlignRect(iSize(23, 23), rect, AlignBottomLeft));
//	gGfxMgr.Blit(PDGG(DLG_CORNERS_GLOW)+3, surf, AlignRect(iSize(23, 23), rect, AlignBottomRight));
}



void ComposeDlgBkgndPanel(iDib& surf, const iRect& rect, bool bTransp)
{
	iRect rc = rect;
	rc.InflateRect(8, 8, 8, 8);
	ComposeDlgBkgnd(surf, rc, bTransp);
}


void ComposeDlgBkgndGrid(iDib& surf, const iRect& rc, PLAYER_ID pid, bool bDecs, iSize sz)
{
	iRect rect = rc;
	rect.InflateRect(8, 8, 8, 8);
	//uint32 nval = (pid+1);
	uint32 toffs = (bDecs)?12:10;
	if (rect.IsEmpty()) return;
	
	// tile background
	iRect bkRect(rect);
	bkRect.DeflateRect(10);
//	gGfxMgr.BlitTile(bDecs?PDGG(BKTILE2):PDGG(BKTILE), surf,bkRect);		
	//surf.FillRect(bkRect, cColor_White);
	surf.FillRect(bkRect, trclr, 210);
	
	// top/bottom tiles
	for(uint32 x=0; x<=sz.h; x++)
		gGfxMgr.BlitTile(PDGG(DLG_HTILES), surf, iRect(rect.x+toffs,rect.y + 8 + (x*(rect.h-18)/sz.h),rect.w-toffs*2,3));
	
	// left/right tiles
	for(uint32 x=0; x<=sz.w; x++)
		gGfxMgr.BlitTile(PDGG(DLG_VTILES), surf, iRect(rect.x + 8 + (x*(rect.w-18)/sz.w),rect.y+toffs,3,rect.h-toffs*2));
	
	// corners	
//	gGfxMgr.Blit(PDGG(DLG_CORNERS)+0, surf, iPoint(rect.x+3,rect.y+3));
//	gGfxMgr.Blit(PDGG(DLG_CORNERS)+1, surf, iPoint(rect.x2()-30,rect.y+3));
//	gGfxMgr.Blit(PDGG(DLG_CORNERS)+2, surf, iPoint(rect.x+3,rect.y2()-30));
//	gGfxMgr.Blit(PDGG(DLG_CORNERS)+3, surf, iPoint(rect.x2()-30,rect.y2()-30));
}


void BlitIcon(iDib& dib, SpriteId sid, const iRect& orc, uint8 a, const iPoint& offset, bool bHighlight)
{
	iSize ssiz = gGfxMgr.Dimension(sid);
	iPoint offs = gGfxMgr.Anchor(sid);
	iPoint op(orc.x+((sint32)(orc.w/2)-(sint32)(ssiz.w/2))-offs.x+offset.x, orc.y+((sint32)(orc.h/2)-(sint32)(ssiz.h/2))-offs.y+offset.y);	
	gGfxMgr.BlitUniversal( sid, dib, op, cInvalidRect, bHighlight ? iGfxManager::EfxGlow : iGfxManager::EfxNone, false, -1, a);
}

void ButtonFrame(iDib& dib, const iRect& rc, uint32 state)
{
	if(gSettings.GetInterfaceType() == IT_TOUCH) return;
	iDib::pixel ltClr = (state&iButton::Pressed)?cColor_Black:cColor_White;
	iDib::pixel rbClr = (state&iButton::Pressed)?cColor_White:cColor_Black;
	dib.HLine(iPoint(rc.x+1,rc.y),rc.x2()-1,ltClr,64);
	dib.HLine(iPoint(rc.x+1,rc.y2()),rc.x2()-1,rbClr,64);
	dib.VLine(iPoint(rc.x,rc.y+1),rc.y2(),ltClr,64);
	dib.VLine(iPoint(rc.x2(),rc.y+1),rc.y2(),rbClr,64);
	//if (state&iButton::Disabled) dib.Darken25Rect(rc);
}

const iTextComposer::FontConfig& GetButtonFont(uint32 state)
{
	if (state & iButton::Disabled) return btnfc_disabled;
	else if (state & iButton::Pressed) return btnfc_pressed;
	else return btnfc_normal;
}

void ComposeDlgButton(iDib& dib, const iRect& rc, uint32 state)
{
	if(gSettings.GetInterfaceType() == IT_STYLUS) {
		iRect orc(rc);
		SpriteId btnSid; 
		if (state&iButton::Disabled) btnSid = PDGG(DLG_BTN);
		else if (state&iButton::Pressed) btnSid = PDGG(DLG_BTN) + 1;
		else btnSid = PDGG(DLG_BTN) + 2;

		// Compose outer frame
		iRect rect = rc;
		rect.InflateRect(1);
		dib.HLine(iPoint(rect.x+2, rect.y), rect.x+rect.w-3, 0);
		dib.HLine(iPoint(rect.x+2, rect.y+rect.h-1), rect.x+rect.w-3, 0);
		dib.VLine(iPoint(rect.x,rect.y+2), rect.y+rect.h-2, 0);
		dib.VLine(iPoint(rect.x+rect.w-1,rect.y+2), rect.y+rect.h-2, 0);

		// Compose button background
		}
	else {
		ComposeDlgBkgndPanel(dib, rc);
		iRect orc(rc);
		orc.DeflateRect(2);
		if(!(state & iButton::Pressed))
			dib.FillRect(orc, RGB16(99, 66, 33));	
		else
			dib.FillRect(orc, RGB16(180, 120, 66));	
	}
}

void ComposeDlgTextButton(iDib& dib, const iRect& rc, uint32 state, TextResId textKey)
{
	ComposeDlgButton(dib, rc, state);

	// Compose text
	if (textKey != TRID_VOID) {
		iTextComposer::FontConfig fc (GetButtonFont(state));
		gTextComposer.TextOut(fc,dib,rc,gTextMgr[textKey],rc,AlignCenter, (state & iButton::Pressed)?iPoint(1,1):iPoint());
	}
}


void ComposeGlowButton(iDib& dib, const iRect& rc, uint32 state)
{	
	ComposeGlowBkgnd(dib, rc, false);
	iRect orc(rc);
	orc.DeflateRect(2);
	/*if(!(state & iButton::Pressed))
		dib.FillRect(orc, RGB16(99, 66, 33));	
	else
		dib.FillRect(orc, RGB16(180, 120, 66));	
		*/

}

void ComposeGlowTextButton(iDib& dib, const iRect& rc, uint32 state, TextResId textKey)
{
	ComposeGlowButton(dib, rc, state);

	// Compose text
	if (textKey != TRID_VOID) {
		iTextComposer::FontConfig fc (GetButtonFont(state));
		gTextComposer.TextOut(fc,dib,rc,gTextMgr[textKey],rc,AlignCenter, (state & iButton::Pressed)?iPoint(1,1):iPoint());
	}
}


void ComposeDlgIconButton(iDib& dib, const iRect& rc, uint32 state, SpriteId sid)
{
	ComposeDlgButton(dib, rc, state);

	// Compose Icon
	iSize ssiz = gGfxMgr.Dimension(sid);
	iPoint op(rc.x+((sint32)(rc.w/2)-(sint32)(ssiz.w/2)), rc.y+((sint32)(rc.h/2)-(sint32)(ssiz.h/2)));
	BlitIcon( dib, sid, rc, (state&iButton::Disabled)?128:255, (state & iButton::Pressed)?iPoint(1,1):iPoint());
}

void ComposeDlgTab(iDib& dib, const iRect& rc, uint32 state, SpriteId sid)
{
	if (state&iButton::Selected) {
		gGfxMgr.BlitTile(PDGG(BKTILE),dib,rc);
	}
	iDib::pixel ltClr = (state&iButton::Pressed)?cColor_Black:cColor_White;
	iDib::pixel rbClr = (state&iButton::Pressed)?cColor_White:cColor_Black;
	uint8 a = (state&iButton::Selected)?64:32;
	dib.HLine(iPoint(rc.x+1,rc.y2()),rc.x2()-1,rbClr,a);
	dib.VLine(iPoint(rc.x,rc.y+1),rc.y2(),ltClr,a);
	dib.VLine(iPoint(rc.x2(),rc.y+1),rc.y2(),rbClr,a);

	if (state&iButton::Selected) BlitIcon( dib, sid, rc);
	else BlitIcon( dib, sid, rc, 128);
}

void ComposeIconButton(iDib& dib, const iRect& rc, uint32 state, SpriteId sid)
{
	if(gSettings.GetInterfaceType() == IT_STYLUS) {
		gGfxMgr.BlitTile(PDGG(BKTILE),dib,rc);
		ButtonFrame(dib,rc,state);
	} else {	
		ComposeDlgBkgndPanel(dib, rc);
	}
	
	iRect frc = rc;
	if(gSettings.GetInterfaceType() == IT_TOUCH)
		frc.DeflateRect(3, 3);
	if (state&iButton::Selected || state & iButton::Pressed) dib.FillRect(frc, RGB16(255,192,0), 96);

	iSize ssiz = gGfxMgr.Dimension(sid);
	iPoint op(rc.x+((sint32)(rc.w/2)-(sint32)(ssiz.w/2)), rc.y+((sint32)(rc.h/2)-(sint32)(ssiz.h/2)));
	if (state&iButton::Disabled) dib.Darken25Rect(rc);
	BlitIcon( dib, sid, rc, (state&iButton::Disabled)?128:255);
}

void ComposeProgressBar(bool bVertical, iDib& dib, const iRect& rc, iDib::pixel clr, sint32 cval, sint32 mval)
{
	iDib::pixel bclr = (iDib::pixel)0;
	dib.FillRect(rc,bclr);
	iRect trc;
	if (bVertical) {
		sint32 mg = iCLAMP<sint32>(0,rc.h,(cval * rc.h) / mval);
		trc = iRect(rc.x,rc.y+rc.h-mg,rc.w,mg);
	} else {
		sint32 mg = iCLAMP<sint32>(0,rc.w,(cval * rc.w) / mval);
		trc = iRect(rc.x,rc.y,mg,rc.h);
	}
	dib.FillRect(trc,clr);

	dib.HLine(iPoint(rc.x,rc.y),rc.x+rc.w-2,cColor_White,48);
	dib.VLine(iPoint(rc.x,rc.y+1),rc.y+rc.h-1,cColor_White,48);
	dib.HLine(iPoint(rc.x+1,rc.y+rc.h-1),rc.x+rc.w-1,cColor_Black,48);
	dib.VLine(iPoint(rc.x+rc.w-1,rc.y+1),rc.y+rc.h-1,cColor_Black,48);
}

void ComposeCreatureIcon(iDib& surf, const iRect& rc, CREATURE_TYPE ct, bool bHighlighted, bool bFlipped)
{
    if(ct == CREAT_UNKNOWN || ct < 0 || ct >= CREAT_COUNT) return;
	SpriteId sid = CREAT_DESC[ct].sprite;				
    
	iRect objrc = gGfxMgr.ObjRect(sid);
	iRect srcrect = iRect(objrc.x, objrc.y, objrc.w, iMIN(rc.h, objrc.h));

	sint32 xoff =  0;
		
	xoff = ((sint32)rc.w - (sint32)objrc.w) / 2;

		
	// these guys are too tall, so move the rect lower to show their face and body, not weapon
	if(srcrect.h == rc.h) {	
		sint32 cor = 0;
		if(ct == CREAT_PIKEMAN || ct == CREAT_STEEL_GUARD || ct == CREAT_HALBERDIER)
			cor = 10;
		else if(ct == CREAT_AIR_MAGE)
			cor = 5;
		else if(ct == CREAT_HEAVY_PIKEMAN)
			cor = 20;

		srcrect.y += cor;	
	}
	
	iPoint sa = gGfxMgr.Anchor(sid);
	iPoint pt = rc.point() + iPoint(xoff, 0);

	if(srcrect.h < rc.h)
		pt.y += rc.h - srcrect.h;

	if(bFlipped)
		pt += iPoint(sa.x, -sa.y);
	else
		pt += iPoint(-sa.x, -sa.y);

	gGfxMgr.BlitUniversal(sid, surf, pt, srcrect, bHighlighted ? iGfxManager::EfxGlow : iGfxManager::EfxNone, false, -1, 255, bFlipped ? iGfxManager::FlipVert : iGfxManager::FlipNone);
}

void ComposeCreatureIconGlow(iDib& surf, const iRect& rc, CREATURE_TYPE ct, bool bHighlighted, bool bFlipped)
{
	SpriteId sid = CREAT_DESC[ct].sprite;			
	gGfxMgr.BlitUniversal(sid, surf, GetRealSpritePos(sid, rc, bFlipped, AlignBottom), cInvalidRect, bHighlighted ? iGfxManager::EfxGlow : iGfxManager::EfxNone, false, -1, 255, bFlipped ? iGfxManager::FlipVert : iGfxManager::FlipNone);
}


void ComposeCreatureButton(iDib& surf, const iRect& rc, CREATURE_TYPE ct, sint32 cnt, VISION_LEVEL vl, bool bPressed, bool bDrag, bool bDrop)
{	
	iRect bkrc(rc);
    
    bkrc += iPoint(1,0);
    
/*	bkrc.DeflateRect(2);
	bkrc.InflateRect(0, 0, 0, 0);
*/	
	if(!bPressed){
		surf.FillRect(bkrc,cColor_Gray128,32);
	}
    else{
		surf.FillRect(bkrc, cColor_White, 200);
    }

	ButtonFrame(surf, rc, bPressed);
	if (ct != CREAT_UNKNOWN && ct >= 0 && ct < CREAT_COUNT && !bDrag) {
		ComposeCreatureIcon(surf,iRect(rc.x+2,rc.y,rc.w-4,rc.h),ct, false, false);
		surf.Darken50Rect(iRect(rc.x,rc.y2()-10,rc.w,11));
		iTextComposer::FontConfig fc(creatqfc);
		if(vl == VL_BASIC)
			fc.fontSize = FS_PT_10;
		gTextComposer.TextOut(fc, surf, iPoint(rc.x,rc.y2()-10), CreatQuant2Text(cnt,vl),iRect(rc.x,rc.y2()-10,rc.w,11),AlignCenter);
	}
	
	if (bDrag) surf.FrameRect(rc,cColor_Blue,64);
	if (bDrop) surf.FrameRect(rc,cColor_Yello,64);
}

void ComposeCreatureButtonWithText(iDib& surf, const iRect& rc, CREATURE_TYPE ct, iStringT text, VISION_LEVEL vl, bool bPressed, bool bDrag, bool bDrop)
{
	if (ct != CREAT_UNKNOWN) surf.FillRect(rc,cColor_Gray128,32);
	else surf.Darken25Rect(rc);
	ButtonFrame(surf, rc, bPressed);
	if (ct != CREAT_UNKNOWN && !bDrag) {
		ComposeCreatureIcon(surf,iRect(rc.x+2,rc.y+2,rc.w-4,rc.h-4),ct, bPressed, false);
		surf.Darken50Rect(iRect(rc.x,rc.y2()-10,rc.w,11));
		//iTextComposer::FontConfig fc((vl == VL_BASIC)?FS_SMALL:FS_MEDIUM, RGB16(192,192,192));
		gTextComposer.TextOut(creatqfc, surf, iPoint(rc.x,rc.y2()-10), text,iRect(rc.x,rc.y2()-10,rc.w,11),AlignCenter);
	}
	
	if (bDrag) surf.FrameRect(rc,cColor_Blue,64);
	if (bDrop) surf.FrameRect(rc,cColor_Yello,64);
}

iSize CreatureCellSize() {
	return iSize(50, 50);
}

void ComposeCreatureCell(iDib& surf, const iRect& rc, CREATURE_TYPE ct, sint32 cnt, VISION_LEVEL vl, bool bBack)
{
	if(bBack) {
		if (ct != CREAT_UNKNOWN) surf.FillRect(rc,cColor_Gray128,32);
		else surf.FillRect(rc,cColor_Black,32);
		ButtonFrame(surf, rc, 0);
	}
	if (ct != CREAT_UNKNOWN) {
		ComposeCreatureIcon(surf,iRect(rc.x+2,rc.y+2,rc.w-4,rc.h-4),ct, false, false);
//		surf.Darken50Rect(iRect(rc.x,rc.y2()-10,rc.w,11));
		iTextComposer::FontConfig fc(creatqfc);
		if(vl == VL_BASIC)
			fc.fontSize = FS_PT_10;
		gTextComposer.TextOut(fc, surf, iPoint(rc.x,rc.y2()), CreatQuant2Text(cnt,vl),iRect(rc.x,rc.y2()-10,rc.w,11),AlignCenter);
	}
}

iSize CalcArmyCellsSize(const iSize& cellSize)
{
	return iSize(cellSize.w*4+3, cellSize.h*2+1);
}

void ComposeArmyCells(iDib& surf, const iPoint& pos, const iSize& cellSize, const iArmy& army, VISION_LEVEL vl)
{
	iPoint p(pos);
	for (uint32 xx=0; xx<3; ++xx) {
		ComposeCreatureCell(surf, iRect(p.x+15,p.y,cellSize.w,cellSize.h), army[xx].Type(), army[xx].Count(), vl);
		p.x += cellSize.w;
	}
	
	p.y+=cellSize.h+1;
	p.x = pos.x;
	for (uint32 xx=3; xx<7; ++xx) {
		ComposeCreatureCell(surf, iRect(p.x,p.y,cellSize.w,cellSize.h), army[xx].Type(), army[xx].Count(), vl);
		p.x += cellSize.w;
	}
}

void ComposeArmyCellsRow(iDib& surf, const iPoint& pos, const iSize& cellSize, const iArmy& army, VISION_LEVEL vl, bool bBack)
{
	iPoint p(pos);
	for (uint32 xx=0; xx<7; ++xx) {
		ComposeCreatureCell(surf, iRect(p.x+15,p.y,cellSize.w,cellSize.h), army[xx].Type(), army[xx].Count(), vl, bBack);
		p.x += cellSize.w;
	}
}

void ComposeGuard(iDib& surf, const iPoint& pos, const iSize& cellSize, const iArmy& army, VISION_LEVEL vl, bool bBack)
{
	iPoint p(pos);
	for (uint32 xx=0; xx<7; ++xx) {
		if(army[xx].Type() != CREAT_UNKNOWN)
		{
			ComposeCreatureCell(surf, iRect(p.x+15,p.y,cellSize.w,cellSize.h), army[xx].Type(), army[xx].Count(), vl, bBack);
			p.x += cellSize.w;
		}
	}
}


void ComposeSpellLevel(iDib& surf, const iRect& rect, MAGIC_SPELL spell, const iHero* pCaster)
{
	//sint32 spLvl = SPELL_DESCRIPTORS[spell].level + 1;
	sint32 msLvl = pCaster->FurtSkill((FURTHER_SKILLS)(FSK_MAG_AIR+SPELL_DESCRIPTORS[spell].school));
	sint32 p = rect.x + rect.w/2-18;
	for (sint32 xx=1; xx<=3; ++xx) {
		SpriteId sid = PDGG(MSCHICN_S)+SPELL_DESCRIPTORS[spell].school;
		if (msLvl<xx) sid += 4;
		gGfxMgr.Blit(sid, surf, iPoint(p, rect.y));
		p += 12;
	}
}

void ComposeMoraleGlyph(iDib& surf, const iRect& orc, sint8 val, bool bHighlight, bool bBig)
{
	if (!val) {
		SpriteId sid;
		if(bBig)
			sid = PDGG(DGLYPH_MORALE)+1;
		else 
			sid = PDGG(ICN_MORALE) + 1;

		iSize siz = gGfxMgr.Dimension(sid);
		sint32 opx = orc.x + (sint32)(orc.w/2) - (sint32)(siz.w/2) - gGfxMgr.Anchor(sid).x;
		sint32 opy = orc.y + (sint32)(orc.h/2) - (sint32)(siz.h/2) - gGfxMgr.Anchor(sid).y;
		gGfxMgr.BlitEffect(sid, gApp.Surface(), iPoint(opx,opy), bHighlight ? iGfxManager::EfxGlow : iGfxManager::EfxNone);
	} else {
		SpriteId sid;
		if(bBig)
			sid = (val>0)?PDGG(DGLYPH_MORALE):(PDGG(DGLYPH_MORALE)+2);
		else
			sid = (val>0)?PDGG(ICN_MORALE):(PDGG(ICN_MORALE)+2);


		sint32 cval = iMIN<sint8>(iABS(val), 3);
		iSize siz = gGfxMgr.Dimension(sid);
		siz.w += (cval-1) * 6;
		sint32 opx = orc.x + (sint32)(orc.w/2) - (sint32)(siz.w/2) - gGfxMgr.Anchor(sid).x;
		sint32 opy = orc.y + (sint32)(orc.h/2) - (sint32)(siz.h/2) - gGfxMgr.Anchor(sid).y;
		for (sint32 xx=0; xx<cval; ++xx) {
			gGfxMgr.BlitEffect(sid, gApp.Surface(), iPoint(opx,opy),  bHighlight ? iGfxManager::EfxGlow : iGfxManager::EfxNone);
			opx += 6;
		}
	}
}

void ComposeLuckGlyph(iDib& surf, const iRect& orc, sint8 val, bool bHighlight, bool bBig)
{
	if (val <= 0) {
		SpriteId sid;
		if(bBig)
			sid = (!val)?PDGG(DGLYPH_LUCK)+1:PDGG(DGLYPH_LUCK)+2;
		else
			sid = (!val)?PDGG(ICN_LUCK)+1:PDGG(ICN_LUCK)+2;

		iSize siz = gGfxMgr.Dimension(sid);
		sint32 opx = orc.x + (sint32)(orc.w/2) - (sint32)(siz.w/2) - gGfxMgr.Anchor(sid).x;
		sint32 opy = orc.y + (sint32)(orc.h/2) - (sint32)(siz.h/2) - gGfxMgr.Anchor(sid).y;
		gGfxMgr.BlitEffect(sid, gApp.Surface(), iPoint(opx,opy), bHighlight ? iGfxManager::EfxGlow : iGfxManager::EfxNone);
	} else {
		SpriteId sid;
		if(bBig)
			sid = PDGG(DGLYPH_LUCK);
		else
			sid = PDGG(ICN_LUCK);

		sint32 cval = iMIN<sint8>(iABS(val), 3);
		iSize siz = gGfxMgr.Dimension(sid);
		siz.w += (cval-1) * 6;
		sint32 opx = orc.x + (sint32)(orc.w/2) - (sint32)(siz.w/2) - gGfxMgr.Anchor(sid).x;
		sint32 opy = orc.y + (sint32)(orc.h/2) - (sint32)(siz.h/2) - gGfxMgr.Anchor(sid).y;
		for (sint32 xx=0; xx<cval; ++xx) {
			gGfxMgr.BlitEffect(sid, gApp.Surface(), iPoint(opx,opy),  bHighlight ? iGfxManager::EfxGlow : iGfxManager::EfxNone);
			opx += 6;
		}
	}
}

const uint16 stars[4] = { RGB16(220,220,220), RGB16(192,192,192), RGB16(160,160,160), RGB16(128,128,128)};

void FillStaredRect(iDib& surf, const iRect& rect, const iPoint& anchor)
{
	iRandomizer rand;
	surf.FillRect(rect, cColor_Black);
	for (uint32 nn=0; nn<2048; ++nn) {
		sint32 px = (rand.Rand() - anchor.x) % 320;
		sint32 py = (rand.Rand() - anchor.y) % 240;
		if (rect.PtInRect(px, py)) surf.PutPixel(px,py,stars[nn&3]);
	}
}

/*
 *	
 */
#ifdef _DEBUG
LPCTSTR FURT_SKILLS_NAMES[FSK_COUNT] = {
	_T("Attack"), _T("Defence"), _T("Spell power"), _T("Knowledge"),
	_T("ActPoints"), _T("Logistics"), _T("Pathfinding"), _T("Scouting"), _T("Vision"),
	_T("Ballistics"), _T("Learning"), _T("Diplomacy"), _T("Necromancy"),
	_T("Sorcery"), _T("ManaPoints"), _T("Intelligence"), _T("Wisdom"),
	_T("AirMagic"), _T("EarthMagic"), _T("FireMagic"), _T("WaterMagic"),
	_T("AirResistance"), _T("EarthResistance"), _T("FireResistance"), _T("WaterResistance"),
	_T("Archery"), _T("Offence"), _T("AirShield"), _T("Shield"), _T("Armorer"), _T("RangeAttack"), _T("MeleeAttack"), _T("Resistance"),
	_T("CreatHits"), _T("CreatSpeed"), _T("Morale"), _T("Luck"), _T("Counterstrike"),
	_T("Gold"), _T("Ore"), _T("Wood"), _T("Mercury"), _T("Gems"), _T("Crystal"), _T("Sulfur")
};

void DbgDumpFurtSkills(const iStringT& hdr, const iFurtSkills& fsk)
{
	OutputDebugString(_T("*********************************\n"));
	OutputDebugString(hdr.CStr());
	OutputDebugString(_T(":\n"));
	OutputDebugString(_T("*********************************\n"));
	for (uint32 xx=0; xx<FSK_COUNT; ++xx){
		if (fsk.Value((FURTHER_SKILLS)xx) != 0) {
			OutputDebugString(iFormat(_T("%s: %d\n"),FURT_SKILLS_NAMES[xx],fsk.Value((FURTHER_SKILLS)xx)).CStr());
		}
	}
	OutputDebugString(_T("------------------------------\n"));
}
#endif

void ShowLoadingBox()
{
	iDisplay& display = gApp.Display();
	iRect rcScreen = display.SurfMetrics();
	iPoint ldrOrg  = rcScreen.Center();
	//ldrOrg.y += rcScreen.h / 3;
	uint32 pbarWidth = (rcScreen.w * 5) / 6;
	iRect rcPbar( ldrOrg.x - pbarWidth/2, ldrOrg.y-22, pbarWidth, 44 );

	iDib& surf = display.GetSurface();
	surf.Darken50Rect(rcPbar);
	surf.Darken50Rect(rcPbar);
	static const wchar_t loadingTxt[] = _T("Loading...");
	gTextComposer.TextOut( dlgfc_hdr, surf, rcPbar.TopLeft(), loadingTxt, rcPbar, AlignCenter  );
	//gTextComposer.TextOut(surf,iPoint(rc.x+34,rc.y),iFormat(_T("#FFF5%s"),gTextMgr[TRID_HSKILL_ESTATES+ss]),iRect(rc.x+34,rc.y,rc.w-30,18),AlignLeft);
	display.DoPaint( display.SurfMetrics() );
}

iPoint GetRealSpritePos( SpriteId sid, const iRect& rc, bool bFlipped, Alignment al )
{
	iSize sz = gGfxMgr.Dimension(sid);
	iPoint sa = gGfxMgr.Anchor(sid);

	iRect src = gGfxMgr.ObjRect(sid);
	iPoint pt = AlignRect(sz, rc, al);	

	pt += iPoint(0, sz.h - src.y2());
	if(bFlipped)
		pt += iPoint(sa.x, -sa.y);
	else
		pt += iPoint(-sa.x, -sa.y);
	return pt;
}


sint32 ComposeChildScreenTitle(iDib& surf, const iRect& rc, const iStringT& text) {
	return gTextComposer.TextBoxOut(castletitlefc, surf, text, rc);
}

sint32 ComposeHeroName(iDib& surf, const iRect& rc, const iHero* pHero)
{
	return ComposeChildScreenTitle(surf, rc, pHero->Name());
}

sint32 ComposeHeroAvatarTitleName(iDib& surf, const iRect& rc, const iHero* pHero, bool bPressed, bool bMoveBar, bool bManaBar) {
	iSize sz = gTextComposer.GetTextSize(iStringT(gTextMgr[TRID_HERO_TYPE_KNIGHT+pHero->Proto()->m_hType]) + _T(" ") + pHero->Name(), castletitlefc);
	sz.w += 55;
	iRect trc = AlignRect(sz, rc, AlignCenter);
	iRect avrc = AlignRect(iSize(50, 50), trc, AlignLeft);
	ComposeHeroAvatar(surf, avrc, pHero, bPressed, bMoveBar, bManaBar);
	trc.DeflateRect(50, 0, 0, 0);
	return ComposeHeroTitleName(surf, trc, pHero);
}

sint32 ComposeHeroAvatarName(iDib& surf, const iRect& rc, const iHero* pHero, bool bPressed, bool bMoveBar, bool bManaBar) {
	iSize sz = gTextComposer.GetTextSize(pHero->Name(), castletitlefc);
	sz.w += 60;
	iRect trc = AlignRect(sz, rc, AlignCenter);
	iRect avrc = AlignRect(iSize(50, 50), trc, AlignLeft);
	ComposeHeroAvatar(surf, avrc, pHero, bPressed, bMoveBar, bManaBar);
	trc.DeflateRect(50, 0, 0, 0);
	return ComposeHeroName(surf, trc, pHero);
}

sint32 ComposeHeroTitleName(iDib& surf, const iRect& rc, const iHero* pHero)
{
	return ComposeChildScreenTitle(surf, rc, iStringT(gTextMgr[TRID_HERO_TYPE_KNIGHT+pHero->Proto()->m_hType]) + _T(" ") + pHero->Name());
}

sint32 ComposeHeroAvatar(iDib& surf, const iRect& rc, const iHero* pHero, bool bPressed, bool bMoveBar, bool bManaBar) 
{
	iSize siz = gGfxMgr.Dimension(pHero->Proto()->m_icn48);
	iRect avrc = AlignRect(siz, rc, AlignTop);

	avrc.InflateRect(10, 10, 11, 11);
	ComposeDlgBkgnd(surf, avrc);

	gGfxMgr.Blit(pHero->Proto()->m_icn48, surf, avrc + iPoint(11, 11));

	if(bMoveBar) {
		iRect movebar = AlignRect(iSize(48, 3), rc, AlignBottom) + iPoint(1, -1);
		
		sint32 fillw = iCLAMP<sint32>(0, movebar.w, pHero->Moves() * movebar.w / pHero->TotalMoves());
		iRect tofill = AlignRect(iSize(fillw, movebar.h), movebar, AlignLeft);
		surf.FillRect(tofill, cColor_Green);
	} else if(bManaBar) {
		iRect manabar = AlignRect(iSize(48, 3), rc, AlignBottom) + iPoint(1, -1);
		
		sint32 fillw = iCLAMP<sint32>(0, manabar.w, pHero->ManaPts() * manabar.w / pHero->MaxManaPts());
		iRect tofill = AlignRect(iSize(fillw, manabar.h), manabar, AlignLeft);
		surf.FillRect(tofill, cColor_Blue);		
	}

	if(bPressed)
		gGfxMgr.Blit(PDGG(BTN_FRAME_BIG_PRESSED), surf, avrc + iPoint(5, 5));
	return 48;
}

sint32 ComposeHeroLevel( iDib& surf, const iRect& rc, const iHero *pHero, bool bHighlight /*= false*/ )
{
	iRect exprc(rc);
	iSize siz = gGfxMgr.Dimension(PDGG(DGLYPH_EXP));
	gGfxMgr.Blit(PDGG(DGLYPH_EXP), surf, AlignRect(siz, exprc, AlignTop));
	if(bHighlight)
		gGfxMgr.BlitEffect(PDGG(DGLYPH_EXP), surf, AlignRect(siz, exprc, AlignTop),iGfxManager::EfxGlow);
	exprc.y += siz.h;

	iRect trc = AlignRect(iSize(40, 11), exprc, AlignTop);
	DrawRoundRect(surf, trc, RGB16(192,128,32), cColor_Black);
	gTextComposer.TextOut(herolevelfc, surf, iPoint(), iFormat(_T("%s %d"), gTextMgr[TRID_LVL], pHero->Level()), trc, AlignCenter);		
	return siz.h + 11;
}

sint32 ComposeHeroAvatarLevel(iDib& surf, const iRect& rc, const iHero* pHero)
{
	// avatar & level
	iRect exprc = AlignRect(iSize(50, 50), rc, AlignTop);
	ComposeHeroAvatar(surf, exprc, pHero);		

	exprc.y += 55;

	ComposeHeroLevel(surf, exprc, pHero);

	return exprc.h;
}


void ComposeResourceIcon(MINERAL_TYPE mineral, const iRect &rc, bool bPressed, bool bEnabled, const iStringT& text, bool bGlow, bool bDeluxe, bool bMini)
{
	sint16 sid;
    if(bMini)
        sid = (PDGG(RES_MINI_ICONS)) + mineral + (bEnabled ? 0 : MINERAL_COUNT);
    else {
        if(bGlow) {
            if(bDeluxe)
                sid = (PDGG(RES_ICONS_DELUXE)) + mineral + (bEnabled ? 0 : MINERAL_COUNT);
            else
                sid = (PDGG(RES_ICONS_GLOW)) + mineral + (bEnabled ? 0 : MINERAL_COUNT);
        }
        else
            sid = PDGG(RES_ICONS) + mineral + (bEnabled ? 0 : MINERAL_COUNT);
    }
	iRect sprrect = AlignRect(gGfxMgr.Dimension(sid), rc, AlignTop) - gGfxMgr.Anchor(sid);

	gGfxMgr.Blit(sid, gApp.Surface(), sprrect);			
	gTextComposer.TextBoxOut(minsetfc, gApp.Surface(), text,  AlignRect(iSize(rc.w, gTextComposer.GetFontHeight(minsetfc.fontSize)), rc, AlignBottom));
	if(bPressed && bGlow)
		gGfxMgr.Blit(PDGG(RES_ICON_PRESSED), gApp.Surface(), AlignRect(gGfxMgr.Dimension(PDGG(RES_ICON_PRESSED)), sprrect, AlignCenter));			
}
iSize GetMineralSetComposeSize( const iMineralSet& cost, bool bGlow, bool bMini, bool bZeroAlso) {
	uint32 count = 0;
    iSize mintextbox = gTextComposer.GetTextSize(_T("99999"), minsetfc);
    iSize oneSize = bMini ? iSize(14, 9) : (bGlow ? iSize(50, 50) : iSize(50, 30));
    oneSize.w = iMAX(oneSize.w, mintextbox.w);
    oneSize.h += mintextbox.h;
    
	for(uint32 xx=0; xx<MINERAL_COUNT; xx++)
		if(cost.quant[xx] != 0 || bZeroAlso)
			count++;
	return iSize(oneSize.w * count, oneSize.h);
}

void ComposeMineralSet( const iMineralSet& cost, const iMineralSet& resources, const iRect& rc, bool bGlow, bool bShorten, bool bDeluxe, bool bMini, bool bZeroAlso) 
{ 
	//gApp.Surface().FrameRect(rc, cColor_Red);
	uint32 count = 0;
	for(uint32 xx=0; xx<MINERAL_COUNT; xx++)
		if(cost.quant[xx] != 0 || bZeroAlso)
			count++;

	if(count == 0) return;

	iRect crc(rc.x, rc.y, rc.w / count, rc.h);
	for(uint32 xx=0; xx<MINERAL_COUNT; xx++) 
		if(cost.quant[xx] != 0 || bZeroAlso)
		{		
			ComposeResourceIcon((MINERAL_TYPE)(MINERAL_GOLD + xx), crc, false, cost.quant[xx] <= resources.quant[xx], bShorten ? FormatShortNumber(cost.quant[xx]) : iFormat(_T("%d"), cost.quant[xx]), bGlow, bDeluxe, bMini );
			crc.x += rc.w / count;
		}
}

void ComposeResourcesBarGlow(const iRect& rc, const iMineralSet& ms)
{
	iRect resrc = AlignRect(gGfxMgr.Dimension(PDGG(CASTLE_RES)), rc, AlignTop);
	gGfxMgr.Blit(PDGG(CASTLE_RES), gApp.Surface(), resrc.point());	

	resrc.y += 10;

	iRect crc(resrc.x +  resrc.w / ((MINERAL_COUNT+1)*2), resrc.y, resrc.w / (MINERAL_COUNT+1), resrc.h);
	for(uint32 xx=0; xx<MINERAL_COUNT; xx++) 
	{
		iRect trc =  AlignRect(iSize(crc.w, gTextComposer.GetFontHeight(minsetfc.fontSize)), crc, AlignBottom);
		trc.InflateRect(10, 0);
		gTextComposer.TextBoxOut(minsetfc, gApp.Surface(), FormatShortNumber(ms.quant[xx]), trc);
		crc.x += crc.w;
	}		
}
