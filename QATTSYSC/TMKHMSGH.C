/* ================================================================= */
/*                                                                   */
/*  MODULE:    TMKHMSGH                                              */
/*                                                                   */
/*                                                                   */
/*  MODULE-TYPE: C Library                                           */
/*                                                                   */
/*  Processor:  C                                                    */
/*                                                                   */
/*  Purpose:    Message Handling module - header file                */
/*                                                                   */
/* ================================================================= */

#ifndef _TMKHMSGH_H
#define _TMKHMSGH_H

/*****************************************************************************
*       macros/structures definition
*****************************************************************************/

#define INV_INF_RULE                  "TMK9001"
#define INV_SUFFIX_SPEC               "TMK9002"
#define NEST_INCL_EXCEED_MAX          "TMK9003"
#define MISMATCH_PARENTH              "TMK9004"
#define CANT_PROC_TARGET              "TMK9005"
#define USER_CMD                      "TMK9006"
#define INV_MACRO_DEF                 "TMK9007"
#define MULT_RULE_SAME_TARGET         "TMK9008"
#define CANT_OPEN_INCLUDE             "TMK9009"
#define INV_RULE                      "TMK9010"
#define NO_HEAP                       "TMK9011"
#define CANT_OPEN_MAKEFILE            "TMK9012"
#define INV_MARGIN_SPEC               "TMK9013"
#define TARGET_NOT_FOUND              "TMK9014"
#define BUITLIN_QMAKSRC_NOT_FOUND     "TMK9015"
#define ERROR_DIRECTIVE               "TMK9016"
#define NO_EXPR_IN_IF_ELIF            "TMK9017"
#define UNKNOWN_DIRECTIVE             "TMK9018"
#define DIVIDED_BY_ZERO               "TMK9019"
#define EXPR_SYNTAX_ERROR             "TMK9020"
#define CMD_EXCEEDED_3000             "TMK9021"
#define INV_OCTAL_NO                  "TMK9022"
#define INV_HEX_NO                    "TMK9023"
#define EXPR_TOO_COMPLICATE           "TMK9024"
#define INTERNAL_EXPR_ERROR           "TMK9025"
#define INV_CHAR_IN_EXPR              "TMK9026"
#define MACRO_TOO_LONG                "TMK9027"
#define MISPLACE_ELIF                 "TMK9028"
#define MISPLACE_ELSE                 "TMK9029"
#define MISPLACE_ENDIF                "TMK9030"
#define UNEXPECT_EOF                  "TMK9031"
#define INV_CMD_SYNTAX                "TMK9032"
#define CMD_FAILED                    "TMK9033"
#define TOO_MANY_MBR                  "TMK9034"
#define ALL_NO_MBR                    "TMK9035"
#define INV_KEYWORD_FORMAT            "TMK9036"
#define INV_RULE_COLON_MISSING        "TMK9037"
#define INV_RULE_NO_TARGET            "TMK9038"
#define INV_RULE_CMD_ONLY             "TMK9039"
#define INV_OBJ_SPEC                  "TMK9040"
#define UNKNOWN_KEYWORD               "TMK9041"
#define RECURSIVE_TARGET              "TMK9042"
#define TARGET_UP_TO_DATE             "TMK9043"
#define INV_PREFIX_RTNSEV             "TMK9044"
#define UNDEF_MACRO_NOT_FOUND         "TMK9045"
#define NEST_DIRECTIVE_EXCEED_MAX     "TMK9046"
#define INV_OUTPUT_MACRO              "TMK9047"


#define MSGF_PATH                     "TMKMSGF   *LIBL     "
#define MSG_TXT_SZ                    3000
#define MSG_LINE_NO_SZ                5
#define MSG_NO_LINE_NO                0

#ifndef __ILEC400__
   #define SIGALL SIGABRT
#endif

/*****************************************************************************
*       function prototypes
*****************************************************************************/

Void    set_usrmsg_to_session ( Void );
Boolean get_usrmsg_to_session ( Void );
Void    log_error ( Char *msgid, Char *txt, Int32 line_no );
Void    log_usrmsg ( Char *txt );
Void    log_dbg ( Char *txt );

#endif  /* _TMKHMSGH_H */

