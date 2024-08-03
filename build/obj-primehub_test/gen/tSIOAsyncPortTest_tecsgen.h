/*
 * This file was automatically generated by tecsgen.
 * This file is not intended to be edited.
 */
#ifndef tSIOAsyncPortTest_TECSGEN_H
#define tSIOAsyncPortTest_TECSGEN_H

/*
 * celltype          :  tSIOAsyncPortTest
 * global name       :  tSIOAsyncPortTest
 * multi-domain      :  no
 * idx_is_id(actual) :  no(no)
 * singleton         :  yes
 * has_CB            :  no
 * has_INIB          :  no
 * rom               :  yes
 * CB initializer    :  yes
 */

/* global header #_IGH_# */
#include "global_tecsgen.h"

/* signature header #_ISH_# */
#include "sSIOAsyncPort_tecsgen.h"
#include "sSIOAsyncCBR_tecsgen.h"

#ifndef TOPPERS_MACRO_ONLY

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */
/* cell CB (dummy) type definition #_CCDP_# */
typedef struct tag_tSIOAsyncPortTest_CB {
    int  dummy;
} tSIOAsyncPortTest_CB;
/* singleton cell CB prototype declaration #_SCP_# */


/* celltype IDX type #_CTIX_# */
typedef int   tSIOAsyncPortTest_IDX;

/* prototype declaration of entry port function #_EPP_# */
/* sSIOAsyncPort */
void         tSIOAsyncPortTest_eSIOPort_open();
void         tSIOAsyncPortTest_eSIOPort_close();
void         tSIOAsyncPortTest_eSIOPort_putNotify();
void         tSIOAsyncPortTest_eSIOPort_enableCBR( uint_t cbrtn);
void         tSIOAsyncPortTest_eSIOPort_disableCBR( uint_t cbrtn);
/* sSIOAsyncCBR */
ER_UINT      tSIOAsyncPortTest_eSIOCBR_sizeSend();
ER_UINT      tSIOAsyncPortTest_eSIOCBR_popSend( char* dst);
ER_UINT      tSIOAsyncPortTest_eSIOCBR_pushReceive( char src);
#ifdef __cplusplus
}
#endif /* __cplusplus */
#endif /* TOPPERS_MACRO_ONLY */

/* to get the definition of CB type of referenced celltype for optimization #_ICT_# */
#ifndef  TOPPERS_CB_TYPE_ONLY
#define  tSIOAsyncPortTest_CB_TYPE_ONLY
#define TOPPERS_CB_TYPE_ONLY
#endif  /* TOPPERS_CB_TYPE_ONLY */
#include "tSerialAsyncPortMain_tecsgen.h"
#ifdef  tSIOAsyncPortTest_CB_TYPE_ONLY
#undef TOPPERS_CB_TYPE_ONLY
#endif /* tSIOAsyncPortTest_CB_TYPE_ONLY */
#ifndef TOPPERS_CB_TYPE_ONLY


/* celll CB macro #_GCB_# */
#define tSIOAsyncPortTest_GET_CELLCB(idx) ((void *)0)
#ifndef TECSFLOW
 /* call port function macro #_CPM_# */
#define tSIOAsyncPortTest_cSIOCBR_sizeSend( ) \
	  tSerialAsyncPortMain_eSIOCBR_sizeSend( \
	   &tSerialAsyncPortMain_CB_tab[2] )
#define tSIOAsyncPortTest_cSIOCBR_popSend( dst ) \
	  tSerialAsyncPortMain_eSIOCBR_popSend( \
	   &tSerialAsyncPortMain_CB_tab[2], (dst) )
#define tSIOAsyncPortTest_cSIOCBR_pushReceive( src ) \
	  tSerialAsyncPortMain_eSIOCBR_pushReceive( \
	   &tSerialAsyncPortMain_CB_tab[2], (src) )

#else  /* TECSFLOW */
#define tSIOAsyncPortTest_cSIOCBR_sizeSend( ) \
	  (p_that)->cSIOCBR.sizeSend__T( \
 )
#define tSIOAsyncPortTest_cSIOCBR_popSend( dst ) \
	  (p_that)->cSIOCBR.popSend__T( \
 (dst) )
#define tSIOAsyncPortTest_cSIOCBR_pushReceive( src ) \
	  (p_that)->cSIOCBR.pushReceive__T( \
 (src) )

#endif /* TECSFLOW */
#endif /* TOPPERS_CB_TYPE_ONLY */

#ifndef TOPPERS_MACRO_ONLY

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

/* prototype declaration of entry port function (referenced when VMT useless optimise enabled) #_EPSP_# */
/* eSIOPort */
void           tSIOAsyncPortTest_eSIOPort_open_skel( const struct tag_sSIOAsyncPort_VDES *epd);
void           tSIOAsyncPortTest_eSIOPort_close_skel( const struct tag_sSIOAsyncPort_VDES *epd);
void           tSIOAsyncPortTest_eSIOPort_putNotify_skel( const struct tag_sSIOAsyncPort_VDES *epd);
void           tSIOAsyncPortTest_eSIOPort_enableCBR_skel( const struct tag_sSIOAsyncPort_VDES *epd, uint_t cbrtn);
void           tSIOAsyncPortTest_eSIOPort_disableCBR_skel( const struct tag_sSIOAsyncPort_VDES *epd, uint_t cbrtn);

#ifndef TOPPERS_CB_TYPE_ONLY

#endif /* TOPPERS_CB_TYPE_ONLY */

#ifdef __cplusplus
}
#endif /* __cplusplus */
#endif /* TOPPERS_MACRO_ONLY */

#ifndef TOPPERS_CB_TYPE_ONLY


/* cell CB macro (abbrev) #_GCBA_# */
#define GET_CELLCB(idx)  tSIOAsyncPortTest_GET_CELLCB(idx)

/* CELLCB type (abbrev) #_CCT_# */
#define CELLCB	tSIOAsyncPortTest_CB

/* celltype IDX type (abbrev) #_CTIXA_# */
#define CELLIDX	tSIOAsyncPortTest_IDX

/* call port function macro (abbrev) #_CPMA_# */
#define cSIOCBR_sizeSend( ) \
          tSIOAsyncPortTest_cSIOCBR_sizeSend( )
#define cSIOCBR_popSend( dst ) \
          tSIOAsyncPortTest_cSIOCBR_popSend( dst )
#define cSIOCBR_pushReceive( src ) \
          tSIOAsyncPortTest_cSIOCBR_pushReceive( src )




/* entry port function macro (abbrev) #_EPM_# */
#define eSIOPort_open    tSIOAsyncPortTest_eSIOPort_open
#define eSIOPort_close   tSIOAsyncPortTest_eSIOPort_close
#define eSIOPort_putNotify tSIOAsyncPortTest_eSIOPort_putNotify
#define eSIOPort_enableCBR tSIOAsyncPortTest_eSIOPort_enableCBR
#define eSIOPort_disableCBR tSIOAsyncPortTest_eSIOPort_disableCBR
#define eSIOCBR_sizeSend tSIOAsyncPortTest_eSIOCBR_sizeSend
#define eSIOCBR_popSend  tSIOAsyncPortTest_eSIOCBR_popSend
#define eSIOCBR_pushReceive tSIOAsyncPortTest_eSIOCBR_pushReceive

/* CB initialize macro #_CIM_# */
#define INITIALIZE_CB()
#define SET_CB_INIB_POINTER(i,p_that)\
	/* empty */
#endif /* TOPPERS_CB_TYPE_ONLY */

#ifndef TOPPERS_MACRO_ONLY

#endif /* TOPPERS_MACRO_ONLY */

#endif /* tSIOAsyncPortTest_TECSGENH */
