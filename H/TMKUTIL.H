/* ================================================================= */
/*                                                                   */
/*  MODULE:    TMKHUTIL                                              */
/*                                                                   */
/*                                                                   */
/*  MODULE-TYPE: C Library                                           */
/*                                                                   */
/*  Processor:  C                                                    */
/*                                                                   */
/*  Purpose:    Utility functions header file                        */
/*                                                                   */
/* ================================================================= */

#ifndef _TMKHUTIL_H
#define _TMKHUTIL_H

/*****************************************************************************
        structure definitions
*****************************************************************************/

typedef struct  Date {
        Int32   val;
} Date_t;

typedef struct  Buf {
        Char    *bp;
        Char    *tp;
        Int16   tsz;
} Buf_t;

/*****************************************************************************
*       macros/structures definition
*****************************************************************************/

extern  Buf_t   t1;
extern  Buf_t   t2;

extern  Char    txtbuf[ WRKBUF_SZ ];

/*****************************************************************************
*       function prototypes
*****************************************************************************/

Char    *skip_white_spaces ( Char *txt );
Char    *skip_non_white_spaces ( Char *txt );
Int16   skip_trail_spaces ( Char *txt );
Int16   skip_trail_spaces_sz ( Char *txt, Int16 read_cnt );
Char    *skip_macro_sym ( Char *txt );
Char    *skip_alnum ( Char *txt );
Char    *skip_alpha ( Char *txt );
Char    *look_for ( Char *txt, Char c );
Char    *to_upper_str ( Char *s );
Char    *strstr_no_case ( Char *s, Char *c );
Void    *alloc_buf ( Int16 sz, Char *err_txt );

Void    reset_buf ( Buf_t *buf );
Void    append_buf ( Buf_t *buf, Char *ap );

Int16   cmp_date ( Date_t d1, Date_t d2 );
Boolean set_date ( Date_t *d, Int32 val );
Int32   get_date ( Date_t *d );

Int32   conv_as400_date ( Char *date );
Char    *obj_name_toupper ( Char *obj_name, Int16 sz );
Void    creat_usrspc ( Void );
Void    delete_usrspc ( Void );

#endif  /* _TMKHUTIL_H */

