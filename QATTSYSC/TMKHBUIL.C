/* ================================================================= */
/*                                                                   */
/*  MODULE:    TMKHBUIL                                              */
/*                                                                   */
/*                                                                   */
/*  MODULE-TYPE: C Library                                           */
/*                                                                   */
/*  Processor:  C                                                    */
/*                                                                   */
/*  Purpose:    Built-in and inference rules processing - header file*/
/*                                                                   */
/* ================================================================= */

#ifndef _TMKHBUIL_H
#define _TMKHBUIL_H

/*****************************************************************************
        macros/structures definition
*****************************************************************************/

typedef struct  Sufxl   {
        struct  Sufxl   *nxt;
        Char    file    [FILE_NM_SZ+1];
        Char    obj_type;
        Char    type    [FILE_NM_SZ+1];
        Char    seu_type[SEU_TYP_SZ+1];
        Boolean is_file;
} Sufxl_t;

/*****************************************************************************
        function prototypes
*****************************************************************************/

Void    read_qmaksrc_builtin ( Void );
Void    setup_builtin_structures ( Void );
Void    apply_inference_rules ( Void );
Void    update_inference_rules ( Char *tp, Cmd_t *cp, Int16 line );
Void    update_suffix ( Char *sp, Int32 line_no );
Void    dump_builtins ( Void );

#endif  /* _TMKHBUIL_H */

