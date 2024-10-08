/*
 * This file was automatically generated by tecsgen.
 * This file is not intended to be edited.
 */
#ifndef tInterruptRequest_TECSGEN_H
#define tInterruptRequest_TECSGEN_H

/*
 * celltype          :  tInterruptRequest
 * global name       :  tInterruptRequest
 * multi-domain      :  no
 * idx_is_id(actual) :  no(no)
 * singleton         :  no
 * has_CB            :  no
 * has_INIB          :  yes
 * rom               :  yes
 * CB initializer    :  yes
 */

/* global header #_IGH_# */
#include "global_tecsgen.h"

/* signature header #_ISH_# */
#include "sInterruptRequest_tecsgen.h"

#ifndef TOPPERS_MACRO_ONLY

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */
/* cell INIB type definition #_CIP_# */
typedef const struct tag_tInterruptRequest_INIB {
    /* call port #_NEP_# */ 
    /* attribute(RO) #_ATO_# */ 
    INTNO          interruptNumber;
}  tInterruptRequest_INIB;

/* CB not exist. CB corresponding to INIB #_DCI_# */
#define tInterruptRequest_CB_tab           tInterruptRequest_INIB_tab
#define tInterruptRequest_CB               tInterruptRequest_INIB
#define tag_tInterruptRequest_CB           tag_tInterruptRequest_INIB

/* singleton cell CB prototype declaration #_MCPB_# */
extern tInterruptRequest_INIB  tInterruptRequest_INIB_tab[];

/* celltype IDX type #_CTIX_# */
typedef const struct tag_tInterruptRequest_INIB *tInterruptRequest_IDX;

/* prototype declaration of entry port function #_EPP_# */
/* sInterruptRequest */
Inline ER           tInterruptRequest_eInterruptRequest_disable(tInterruptRequest_IDX idx);
Inline ER           tInterruptRequest_eInterruptRequest_enable(tInterruptRequest_IDX idx);
Inline ER           tInterruptRequest_eInterruptRequest_clear(tInterruptRequest_IDX idx);
Inline ER           tInterruptRequest_eInterruptRequest_raise(tInterruptRequest_IDX idx);
Inline ER_BOOL      tInterruptRequest_eInterruptRequest_probe(tInterruptRequest_IDX idx);
#ifdef __cplusplus
}
#endif /* __cplusplus */
#endif /* TOPPERS_MACRO_ONLY */

#define tInterruptRequest_ID_BASE        (1)  /* ID Base  #_NIDB_# */
#define tInterruptRequest_N_CELL        (1)  /*  number of cells  #_NCEL_# */

/* IDX validation macro #_CVI_# */
#define tInterruptRequest_VALID_IDX(IDX) (1)


/* celll CB macro #_GCB_# */
#define tInterruptRequest_GET_CELLCB(idx) (idx)

/* attr access  #_AAM_# */
#define tInterruptRequest_ATTR_interruptNumber( p_that )	((p_that)->interruptNumber)

#define tInterruptRequest_GET_interruptNumber(p_that)	((p_that)->interruptNumber)



#ifndef TECSFLOW
#else  /* TECSFLOW */
#endif /* TECSFLOW */
#ifndef TOPPERS_MACRO_ONLY

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

/* prototype declaration of entry port function (referenced when VMT useless optimise enabled) #_EPSP_# */

#ifdef __cplusplus
}
#endif /* __cplusplus */
#endif /* TOPPERS_MACRO_ONLY */

/* IDX validation macro (abbrev.) #_CVIA_# */
#define VALID_IDX(IDX)  tInterruptRequest_VALID_IDX(IDX)


/* cell CB macro (abbrev) #_GCBA_# */
#define GET_CELLCB(idx)  tInterruptRequest_GET_CELLCB(idx)

/* CELLCB type (abbrev) #_CCT_# */
#define CELLCB	tInterruptRequest_CB

/* celltype IDX type (abbrev) #_CTIXA_# */
#define CELLIDX	tInterruptRequest_IDX


/* attr access macro (abbrev) #_AAMA_# */
#define ATTR_interruptNumber tInterruptRequest_ATTR_interruptNumber( p_cellcb )





/* entry port function macro (abbrev) #_EPM_# */
#define eInterruptRequest_disable tInterruptRequest_eInterruptRequest_disable
#define eInterruptRequest_enable tInterruptRequest_eInterruptRequest_enable
#define eInterruptRequest_clear tInterruptRequest_eInterruptRequest_clear
#define eInterruptRequest_raise tInterruptRequest_eInterruptRequest_raise
#define eInterruptRequest_probe tInterruptRequest_eInterruptRequest_probe

/* iteration code (FOREACH_CELL) #_FEC_# */
#define FOREACH_CELL(i,p_cb)   \
    for( (i) = 0; (i) < tInterruptRequest_N_CELL; (i)++ ){ \
       (p_cb) = &tInterruptRequest_CB_tab[i];

#define END_FOREACH_CELL   }

/* CB initialize macro #_CIM_# */
#define INITIALIZE_CB(p_that)	(void)(p_that);
#define SET_CB_INIB_POINTER(i,p_that)\
	/* empty */
#ifndef TOPPERS_MACRO_ONLY

/*  include inline header #_INL_# */
#include "tInterruptRequest_inline.h"

#endif /* TOPPERS_MACRO_ONLY */

#ifdef TOPPERS_CB_TYPE_ONLY

/* undef for inline #_UDF_# */
#undef VALID_IDX
#undef GET_CELLCB
#undef CELLCB
#undef CELLIDX
#undef tInterruptRequest_IDX
#undef FOREACH_CELL
#undef END_FOREACH_CELL
#undef INITIALIZE_CB
#undef SET_CB_INIB_POINTER
#undef ATTR_interruptNumber
#undef tInterruptRequest_ATTR_interruptNumber
#undef tInterruptRequest_GET_interruptNumber
#undef ATTR_attribute
#undef tInterruptRequest_ATTR_attribute
#undef tInterruptRequest_GET_attribute
#undef ATTR_interruptPriority
#undef tInterruptRequest_ATTR_interruptPriority
#undef tInterruptRequest_GET_interruptPriority
#undef eInterruptRequest_disable
#undef eInterruptRequest_enable
#undef eInterruptRequest_clear
#undef eInterruptRequest_raise
#undef eInterruptRequest_probe
#endif /* TOPPERS_CB_TYPE_ONLY */

#endif /* tInterruptRequest_TECSGENH */
