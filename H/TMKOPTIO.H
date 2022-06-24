/* ================================================================= */
/*                                                                   */
/*  MODULE:    TMKHOPTI                                              */
/*                                                                   */
/*                                                                   */
/*  MODULE-TYPE: C Library                                           */
/*                                                                   */
/*  Processor:  C                                                    */
/*                                                                   */
/*  Purpose:    Option processing function - header file             */
/*                                                                   */
/* ================================================================= */

#ifndef _TMKHOPTI_H
#define _TMKHOPTI_H

/**********************************************************************
*       macros/structures definition
**********************************************************************/

/*      command argument index                                       */
#define ARGV_TARGET             1
#define ARGV_SRCFILE            2
#define ARGV_SRCMBR             3
#define ARGV_MACRO              4
#define ARGV_OPTIONS            5
#define ARGV_MARGINS            6
#define ARGV_RTNCDE             7
#define ARGV_USRMSG             8

#ifdef SRVOPT
#define ARGV_SRVOPT             9               /* this must be last */
#endif

/*      options variable valid value bit definitions                 */
/*      zero value is always the default option for each bit         */
#define OPT_NOIGNORE            0x0000       /* don't ignore err code*/
#define OPT_IGNORE              0x0001       /* ignore error code    */
#define OPT_NOSILENT            0x0000       /* No silent mode       */
#define OPT_SILENT              0x0002       /* silent mode          */
#define OPT_BIRULES             0x0000       /* use built-in rule    */
#define OPT_NOBIRULES           0x0004       /* use no built-in rule */
#define OPT_EXEC                0x0000       /* execute mode         */
#define OPT_NOEXEC              0x0008       /* no execute mode      */
#define OPT_NOTOUCH             0x0000       /* don't touch targets  */
#define OPT_TOUCH               0x0010       /* touch target files   */
#define OPT_NODEBUG             0x0000       /* NO debug information */
#define OPT_DEBUG               0x0020       /* debug information    */

#define OPT_DEFAULT             OPT_NOIGNORE | OPT_NOSILENT |  \
                                OPT_BIRULES  | OPT_EXEC     |  \
                                OPT_NOTOUCH  | OPT_NODEBUG

/*      AS/400 MAKE return code handling definitions                 */
#define RTN_EXCEPTION           0x01         /* exception handling   */
#define RTN_LURC                0x02         /* L/U rtncde handling  */
#define RTN_BOTH                0x03         /* Both types handling  */

#define RTN_DEF_SEV             0            /* Default severiry     */


/*      AS/400 MAKE *CMD field length definitions                    */
#define CMDF_TARGET             54           /* target field sz      */
#define CMDF_TARGET_DEFAULT     "*FIRST"     /* default target name  */

#define CMDF_LIBFILE            20           /* makefile lib/file sz */
#define CMDF_MBR                10           /* makefile mbr sz      */
#define CMDF_MACRO              32           /* macro field sz       */


/*      option target list to process structure                      */
typedef struct  Targ_list {
        struct  Targ_list       *nxt;
        Char                    txt[1];
} Targ_list_t;

/*      Makefile file member list to process structure               */
typedef struct  Mbr_list {
        struct  Mbr_list        *nxt;
        Char                    txt[1];
} Mbr_list_t;

#ifdef  SRVOPT
#define SRVOPT_NONE              0x0000
#define SRVOPT_FUNCTION          0x0001
#define SRVOPT_FCTRTN            0x0002
#define SRVOPT_DETAIL            0x0004
#define SRVOPT_DUMP_MACRO        0x0008
#define SRVOPT_DUMP_BI           0x0010
#define SRVOPT_DUMP_RULES        0x0020
#define SRVOPT_DUMP_SUFFIX       0x0040
#define SRVOPT_DUMP_DEFCMD       0x0080
#define SRVOPT_DUMP_IO           0x0100

extern  Char   srv_cat[WRKBUF_SZ];

Boolean srvopt_function     ( Void );
Boolean srvopt_fctrtn       ( Void );
Boolean srvopt_detail       ( Void );
Boolean srvopt_dump_macro   ( Void );
Boolean srvopt_dump_bi      ( Void );
Boolean srvopt_dump_rules   ( Void );
Boolean srvopt_dump_suffix  ( Void );
Boolean srvopt_dump_defcmd  ( Void );
Boolean srvopt_dump_io      ( Void );
Char    *srv_fs             ( File_spec_t *fs );
Char    *srv_e              ( Element_t *e );
Char    *srv_cmd            ( Cmd_t *c );
Char    *srv_rule           ( Rules_t *r );
Char    *srv_sym            ( Sym_e_t *s );
#endif

/**********************************************************************
*       function prototypes
**********************************************************************/

Void    process_options ( int argc, Char **argv );
Void    setup_command_macro ( Void );
Char    *get_first_requested_target ( Void );
Char    *get_next_requested_target ( Void );
Int32   no_of_makefile_mbr ( Void );
Char    *next_makefile_mbr ( Void );

Void    opt_set_ingore ( Void );
Void    opt_set_silent ( Void );
Int16   opt_get_left_margin ( Void );
Int16   opt_get_right_margin ( Void );
Int16   opt_get_rtncde_methods ( Void );
Int16   opt_get_rtncde_sev ( Void );
Boolean opt_ignore_err_code ( Void );
Boolean opt_silent_mode ( Void );
Boolean opt_no_built_in_rule ( Void );
Boolean opt_no_execute ( Void );
Boolean opt_touch_target ( Void );
Boolean opt_debug ( Void );

#endif  /* _TMKHOPTI_H */

