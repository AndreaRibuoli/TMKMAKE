/* ================================================================= */
/*                                                                   */
/*  MODULE:    TMKHDICT                                              */
/*                                                                   */
/*                                                                   */
/*  MODULE-TYPE: C Library                                           */
/*                                                                   */
/*  Processor:  C                                                    */
/*                                                                   */
/*  Purpose:    Dictionery supporting function - header file         */
/*                                                                   */
/* ================================================================= */

#ifndef _TMKHDICT_H
#define _TMKHDICT_H

/*****************************************************************************
        macros/structures definition
*****************************************************************************/

#define DICT_TAB_SZ             23

typedef struct Sym_e {
        struct  Sym_e   *nxt;
        Char            *sym;
        Char            *txt;
        Boolean         perm;
} Sym_e_t;


/*****************************************************************************
        function prototypes
*****************************************************************************/

Void    setup_dict ( Void );
Boolean put_sym ( Char *sym, Char *txt, Boolean perm );
Boolean update_sym ( Char *sym, Char *txt, Boolean perm );
Boolean delete_sym ( Char *sym );
Char    *get_sym ( Char *sym );
Char    *text_substitution ( Char *txt, Int16 line );
Char    *bi_macro_substitution ( Char *txt );
Void    set_parse_state ( Void );
Void    set_make_state ( Void );

Void    dump_dict ( Void );

#endif  /* _TMKHDICT_H */

