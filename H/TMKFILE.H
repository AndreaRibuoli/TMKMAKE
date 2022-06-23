/* ================================================================= */
/*                                                                   */
/*  MODULE:    TMKHFILE                                              */
/*                                                                   */
/*                                                                   */
/*  MODULE-TYPE: C Library                                           */
/*                                                                   */
/*  Processor:  C                                                    */
/*                                                                   */
/*  Purpose:    File specification processing - header file          */
/*                                                                   */
/* ================================================================= */

#ifndef _TMKHFILE_H
#define _TMKHFILE_H

/***********************************************************************
*       macros/structures definition
***********************************************************************/

#define FS_L_LIBL       "*LIBL"       /* *LIBL library qualifier      */
#define FS_L_CURLIB     "*CURLIB"     /* *CUBLIB library qualifier    */

#define FILE_NM_SZ      10            /* object name size             */
#define SEU_TYP_SZ      10            /* SEU type name size           */

#define FS_T_UNKNOWN    "UNKNOWN"     /* unknown file type            */
#define FS_T_FILE       "FILE"        /* file object                  */
#define FS_T_PGM        "PGM"         /* pgm object                   */
#define FS_T_PGMSET     "PGMSET"      /* *PGM with SETPGMINF applied  */
#define FS_T_LIBFILE    "LIBFILE"     /* EPM LIBFILE                  */
#define FS_T_TXTLIB     "TXTLIB"      /* System C/400 TXTLIB          */
#define FS_T_BNDDIR     "BNDDIR"      /* ILE BNDDIR                   */


typedef struct  File_spec {
        Char    lib     [FILE_NM_SZ+1];/* library name                */
        Char    file    [FILE_NM_SZ+1];/* file/txtlib/libfile/targt nm*/
        Char    extmbr  [FILE_NM_SZ+1];/* file/txtlib/libfile mbr name*/
        Char    obj_type;              /* object type start with *    */
        Char    type    [FILE_NM_SZ+1];/* object type                 */
        Char    seu_type[SEU_TYP_SZ+1];/* SEU type name               */
        Boolean is_file;               /* is *FILE type indicator     */
        Date_t  last_update;           /* object last update date     */
        Date_t  proc_update;           /* object process update date  */
        Date_t  create_date;           /* object creation date        */
} File_spec_t;

#define LST_USRSPC_NM   "MAKEFLUSPCQTEMP     "
#define DLT_USRSPC_NM   "QTEMP/MAKEFLUSPC"

/* System C/400 TXTLIB libary data structure for STRMAKE library      */
/* maintenance portion of code                                        */
typedef struct LibHdr {
        Char    MagicCookie [80];
        Int32   MemTabStart;
        Int32   SymTabStart;
        Int32   MemberCount;
        Int32   SymbolCount;
} LibHdr_T;

typedef struct MemList {
        Char    Name [11];
        Char    Date [9];
        Char    Time [9];
        Int32   Flag_Size;
        Int32   Offset;
        Char    spare[10];
} MemList_T;


/* Object list exception global indicator and allowed constants       */
extern   Boolean obj_not_exist_flag;
#define  LSTOBJ_FOUND              0
#define  LSTOBJ_NOT_FOUND          1
#define  LSTOBJ_EXCEED_USRSPC      2
#define  LSTOBJ_INVALID_TYPE       3

#include        "tmkhpars.qattsysc"

/***********************************************************************
*       function prototypes
***********************************************************************/

Char    *parse_obj_name ( Char *f, File_spec_t *fs,
                          Int16 *len, Int16 line );
Boolean parse_inference_rule ( Char *f, File_spec_t *fsd,
                               File_spec_t *fst );
Char    *parse_file_ext ( Char *f, File_spec_t *fs );
Boolean update_file_date ( File_spec_t *fs, Int16 line );
Boolean obj_exists ( File_spec_t *fs, Int16 line );
Char    *skip_obj_name ( Char *txt );
Void    touch_target ( Element_t *ep );
Boolean same_obj_base ( File_spec_t *tfs, File_spec_t *dfs );
Void    obj_not_exist_trap ( int sig );
Char    *obj_full_name ( File_spec_t *fs );
Boolean is_file_type ( Char *type );

#endif  /* _TMKHFILE_H */

