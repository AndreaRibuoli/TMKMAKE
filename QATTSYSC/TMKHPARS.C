/* ================================================================= */
/*                                                                   */
/*  MODULE:    TMKHPARS                                              */
/*                                                                   */
/*                                                                   */
/*  MODULE-TYPE: C Library                                           */
/*                                                                   */
/*  Processor:  C                                                    */
/*                                                                   */
/*  Purpose:    Makefile parsing functions - header file             */
/*                                                                   */
/* ================================================================= */

#ifndef _TMKHPARS_H
#define _TMKHPARS_H

#ifndef __recio
#include  <recio.h>
#endif  /*__recio*/

/*****************************************************************************
        macros/structures definition
*****************************************************************************/

typedef struct  Element {
        struct  Element *nxt;          /* next object element        */
        Boolean         maked;         /* object processed indicator */
        File_spec_t     fs;            /* object file specification  */
        Char            name[1];       /* first char in element text */
} Element_t;

typedef struct  Cmd     {
        struct  Cmd     *nxt;          /* next command in list        */
        Int16           line;          /* line no in description file */
        Char            cmd_txt[1];    /* first char in command text  */
} Cmd_t;

typedef struct  Work    {
        Char            *t1;           /* target name text            */
        Char            *t2;           /* dependents list text        */
        Cmd_t           *cmd;          /* commands sequence list      */
        Int16           op;            /* operation code              */
        Int16           line;          /* starting line number        */
} Work_t;

typedef struct  Rules   {
        struct  Rules   *nxt;          /* next inference rule in list */
        Element_t       *target;       /* target definition structure */
        Element_t       *dependent;    /* dependents defn structure   */
        Cmd_t           *cmd;          /* associated commands seq list*/
        Int16           op;            /* rule operation code         */
        Int16           line;          /* line no in description file */
        Boolean         recursive;     /* recursion indicator         */
        Boolean         implicit_rule; /* implicit rule indicator     */
} Rules_t;

typedef struct  Incl    {
        Int16           line_no;       /* read line no before nesting */
        Int16           left_margin;   /* source left margin limit    */
        Int16           right_margin;  /* source right margin limit   */
        Int16           rec_len;       /* source record length        */
        Boolean         eof;           /* record I/O EOF indicator    */
        Char            *name;         /* transient source file name  */
        _RFILE          *f;            /* record I/O file pointer     */
} Incl_t;

#define OP_NO_KEYWORD   -2
#define OP_ERROR        -1
#define OP_MACRO        0
#define OP_RULES        1
#define OP_MULTI_RULES  2
#define OP_COMMAND      3
#define OP_SUFFIXES     4
#define OP_PRECIOUS     5
#define OP_INFER_RULES  6
#define OP_IGNORE       7
#define OP_SILENT       8

#define PARSE_BUILTIN   0
#define PARSE_MAKEFILE  1

#define INCLUDE_TXT     "include"
#define INCLUDE_SZ      ( sizeof( INCLUDE_TXT ) - 1 )

/* conditional directives keywords opcode                             */
typedef struct  cd      {
        Int16           op;            /* current directive op        */
        Int16           line;          /* line # of last nested op    */
        Int16           cd_expected;   /* next allowed directives mask*/
        Boolean         active;        /* directives in active mode   */
        Boolean         ifelse_before; /* directives in active before */
                                       /* use to handle if/elif/...   */
} Cd_t;

#define CD_NONE         0x0000         /* no directive found          */
#define CD_IF           0x0001         /* #if                         */
#define CD_ELSE         0x0002         /* #else                       */
#define CD_ELIF         0x0004         /* #elif                       */
#define CD_ENDIF        0x0008         /* #endif                      */
#define CD_IFDEF        0x0010         /* #ifdef                      */
#define CD_IFNDEF       0x0020         /* #ifndef                     */
#define CD_ERROR        0x0040         /* #error                      */
#define CD_UNDEF        0x0080         /* #undef                      */
#define CD_UNKNOWN      0x4000         /* unknown directives          */

/***********************************************************************
        External Variable declarations
***********************************************************************/

/***********************************************************************
        function prototypes
***********************************************************************/

Void    setup_parser ( Void );
Void    setup_parser_structures ( Void );
Int16   parse_makefile ( Char *, Boolean );
Rules_t *get_first_rule ( void );
Rules_t *get_next_rule ( void );
Rules_t *get_first_applied_target ( void );
Rules_t *get_next_applied_target ( void );
Rules_t *in_rule_list ( Char *name );
Void    add_inf_rule_to_rule_tail ( Element_t *tep, Element_t *dep,
                                    Cmd_t *cmd );
Char    *has_dyn_macro ( Char *txt );

Void    dump_rule_list( Void );
Char    *get_cur_filename( Void );

Void    free_element( Element_t *ep );
Void    free_cmd ( Cmd_t *cp );

#endif  /* _TMKHPARS_H */

