/* ================================================================= */
/*                                                                   */
/*  MODULE:    TMKHMAKE                                              */
/*                                                                   */
/*                                                                   */
/*  MODULE-TYPE: C Library                                           */
/*                                                                   */
/*  Processor:  C                                                    */
/*                                                                   */
/*  Purpose:    Makefile processing functions - header file          */
/*                                                                   */
/* ================================================================= */

#ifndef _TMKHMAKE_H
#define _TMKHMAKE_H

/*****************************************************************************
        macros/structures definition
*****************************************************************************/
#define STRMAKE_MACRO          "TMKMAKE"
#define QCMDEXC_MAX_SZ         3000

extern  Boolean usrspc_created;

/*****************************************************************************
        function prototypes
*****************************************************************************/

Void    process_makefile ( Void );

#endif  /* _TMKHMAKE_H */

