/**
tags.h
TrueType和OpenType表的标记
*/


#ifndef TAGS_H_
#define TAGS_H_


#define MAKE_TAG(_x1, _x2, _x3, _x4) (uint32_t)(((uint32_t)_x1 << 24)|((uint32_t)_x2 << 16)|((uint32_t)_x3 << 8)|(uint32_t)_x4)


#define TAG_avar  "avar"
#define TAG_BASE  "BASE"
#define TAG_bdat  "bdat"
#define TAG_BDF   "BDF "
#define TAG_bhed  "bhed"
#define TAG_bloc  "bloc"
#define TAG_bsln  "bsln"
#define TAG_CBDT  "CBDT"
#define TAG_CBLC  "CBLC"
#define TAG_CFF   "CFF "
#define TAG_CID   "CID "
#define TAG_CPAL  "CPAL"
#define TAG_COLR  "COLR"
#define TAG_cmap  "cmap"
#define TAG_cvar  "cvar"
#define TAG_cvt   "cvt "
#define TAG_DSIG  "DSIG"
#define TAG_EBDT  "EBDT"
#define TAG_EBLC  "EBLC"
#define TAG_EBSC  "EBSC"
#define TAG_feat  "feat"
#define TAG_FOND  "FOND"
#define TAG_fpgm  "fpgm"
#define TAG_fvar  "fvar"
#define TAG_gasp  "gasp"
#define TAG_GDEF  "GDEF"
#define TAG_glyf  "glyf"
#define TAG_GPOS  "GPOS"
#define TAG_GSUB  "GSUB"
#define TAG_gvar  "gvar"
#define TAG_hdmx  "hdmx"
#define TAG_head  "head"
#define TAG_hhea  "hhea"
#define TAG_hmtx  "hmtx"
#define TAG_JSTF  "JSTF"
#define TAG_just  "just"
#define TAG_kern  "kern"
#define TAG_lcar  "lcar"
#define TAG_loca  "loca"
#define TAG_LTSH  "LTSH"
#define TAG_LWFN  "LWFN"
#define TAG_MATH  "MATH"
#define TAG_maxp  "maxp"
#define TAG_META  "META"
#define TAG_MMFX  "MMFX"
#define TAG_MMSD  "MMSD"
#define TAG_mort  "mort"
#define TAG_morx  "morx"
#define TAG_name  "name"
#define TAG_opbd  "opbd"
#define TAG_OS2   "OS/2"
#define TAG_OTTO  "OTTO"
#define TAG_PCLT  "PCLT"
#define TAG_POST  "POST"
#define TAG_post  "post"
#define TAG_prep  "prep"
#define TAG_prop  "prop"
#define TAG_sbix  "sbix"
#define TAG_sfnt  "sfnt"
#define TAG_SING  "SING"
#define TAG_trak  "trak"
#define TAG_true  "true"
#define TAG_ttc   "ttc "
#define TAG_ttcf  "ttcf"
#define TAG_TYP1  "TYP1"
#define TAG_typ1  "typ1"
#define TAG_VDMX  "VDMX"
#define TAG_vhea  "vhea"
#define TAG_vmtx  "vmtx"
#define TAG_wOFF  "wOFF"



#endif /* TAGS_H_ */


/* END */
