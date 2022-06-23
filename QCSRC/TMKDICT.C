/* ================================================================= */
/*                                                                   */
/*  MODULE:    TMKDICT                                               */
/*                                                                   */
/*                                                                   */
/*  MODULE-TYPE: C Library                                           */
/*                                                                   */
/*  Processor:  C                                                    */
/*                                                                   */
/*  Purpose:    Dictionery support functions.                        */
/*                                                                   */
/* ================================================================= */


#include        <stdio.h>
#include        <stdlib.h>
#include        <string.h>
#include        <ctype.h>

#include        "tmkbase.h"
#include        "tmkutil.h"
#include        "tmkfile.h"
#include        "tmkparse.h"
#include        "tmkdict.h"
#include        "tmkoptio.h"
#include        "tmkmsghd.h"


/***********************************************************************
        Variable declarations
***********************************************************************/

Static  Sym_e_t *dict_table[ DICT_TAB_SZ ];

Static  Int16   subtxt_sz       = 0;          /* text substitution    */
Static  Char    *subtxt         = NULL;       /* buffer size/location */
Static  Boolean making;                       /* parse/make state flag*/


/* ================================================================= */
/*  Function:    setup_dict ()                                       */
/* ================================================================= */

Void    setup_dict ( Void ) {
        Int16   cnt;
        Sym_e_t *pp;
        Sym_e_t *np;

#ifdef SRVOPT
        if( srvopt_function() )
            printf( "FCT:setup_dict(Void)\n");
#endif
        for( cnt = 0 ; cnt < DICT_TAB_SZ ; ++cnt ) {
            pp = dict_table[ cnt ];
            while( pp ) {
                np = pp;
                pp = pp->nxt;
                free( np );
            }
            dict_table[ cnt ]       = NULL;
        }

        if( subtxt == NULL ) {
            subtxt = (Char *)alloc_buf( WRKBUF_SZ, "setup_dict()" );
            subtxt_sz       = WRKBUF_SZ;
        }
        subtxt[0]       = 0;
}


/* ================================================================= */
/*  Function:    set_parse_state ()                                  */
/* ================================================================= */

Void    set_parse_state ( Void ) {
#ifdef SRVOPT
        if( srvopt_function() )
            printf( "FCT:set_parse_state(Void)\n");
#endif
        making  = FALSE;
}

Void    set_make_state ( Void ) {
#ifdef SRVOPT
        if( srvopt_function() )
            printf( "FCT:set_make_state(Void)\n");
#endif
        making  = TRUE;
}


/* ================================================================= */
/*  Function:    expand_subtxt ()                                    */
/* ================================================================= */

Static
Void    expand_subtxt( Int32 sz ) {
        Char    *lp     = subtxt;

#ifdef SRVOPT
        if( srvopt_function() )
            printf( "FCT:expand_subtxt(%d)\n",sz);
#endif
        subtxt_sz = (sz / WRKBUF_SZ + 1) * WRKBUF_SZ;
        subtxt    = (Char *)alloc_buf( subtxt_sz, "expand_subtxt()" );
        strcpy( subtxt, lp );
        free( lp );
}


/* ================================================================= */
/*  Function:    hash_sym (0                                         */
/* ================================================================= */

Static
Int16   hash_sym ( Char *sym ) {
        Int16   h_val   = 0;

#if 0
#ifdef SRVOPT
        if( srvopt_function() )
            printf( "FCT:hash_sym(\"%s\")\n",sym);
#endif
#endif
        while( *sym ) {
            h_val   += *sym;
            h_val   += ( *sym >> 4 );
            ++sym;
        }
#if 0
#ifdef SRVOPT
        if( srvopt_fctrtn() )
            printf( "RTN:hash_sym:%d\n",h_val%DICT_TAB_SZ);
#endif
#endif
        return( h_val % DICT_TAB_SZ );
}


/* ================================================================= */
/*  Function:    sym_in_table ()                                     */
/* ================================================================= */

Static
Sym_e_t *sym_in_table ( Char *sym ) {
        Sym_e_t *sp = dict_table[ hash_sym( sym ) ];

#if 0
#ifdef SRVOPT
        if( srvopt_function() )
            printf( "FCT:sym_in_table(\"%s\")\n",sym);
#endif
#endif
        while( sp ) {
            if( strcmp( sp->sym, sym ) == 0 )
                break;

            sp      = sp->nxt;
        }
#if 0
#ifdef SRVOPT
        if( srvopt_fctrtn() )
            printf( "RTN:sym_in_table:%s\n",srv_sym(sp));
#endif
#endif
        return( sp );
}


/* ================================================================= */
/*  Function:    add_sym ()                                          */
/* ================================================================= */

Static
Void    add_sym ( Sym_e_t *sp ) {
        Int16   h_val = hash_sym( sp->sym );
        Sym_e_t *np;

#if 0
#ifdef SRVOPT
        if( srvopt_function() )
            printf( "FCT:add_sym(%s)\n",srv_sym(sp));
#endif
#endif
        if( dict_table[ h_val ] == NULL )
            dict_table[ h_val ]     = sp;
        else {
            np = dict_table[ h_val ];
            while( np->nxt )
                np      = np->nxt;
            np->nxt = sp;
        }
}


/* ================================================================= */
/*  Function:    delete_sym ()                                       */
/* ================================================================= */

Boolean delete_sym ( Char *sym ) {
        Int16   h_val = hash_sym( sym );
        Sym_e_t *pp;
        Sym_e_t *np;

#if 0
#ifdef SRVOPT
        if( srvopt_function() )
            printf( "FCT:delete_sym(\"%s\")\n",sym);
#endif
#endif
        if( ( pp = dict_table[ h_val ] ) == NULL )
            return( FALSE );

        if( strcmp( pp->sym, sym ) == 0 ) {
            dict_table[ h_val ] = pp->nxt;
            free( pp );
            return( TRUE );
        }

        while( pp->nxt ) {
            np      = pp->nxt;
            if( strcmp( np->sym, sym ) == 0 ) {
                pp->nxt = np->nxt;
                free( np );
                return( TRUE );
            }
            pp      = np;
        }
        return( FALSE );
}


/* ================================================================= */
/*  Function:    fill_sym_entry (0                                   */
/* ================================================================= */

Static
Sym_e_t *fill_sym_entry ( Char *sym, Char *txt ) {
        Sym_e_t *sp;
        Int16   i1      = strlen( sym ) + 1;
        Int16   i2      = strlen( txt ) + 1;

#if 0
#ifdef SRVOPT
        if( srvopt_function() )
            printf( "FCT:fill_sym_entry(\"%s\",\"%s\")\n",sym,txt);
#endif
#endif
        sp      = (Sym_e_t *)alloc_buf( sizeof( Sym_e_t ) + i1 + i2,
                                "fill_sym_entry()" );
        sp->nxt = NULL;
        sp->sym = (Char *)sp + sizeof( Sym_e_t );
        strcpy( sp->sym, sym );
        sp->txt = sp->sym + i1;
        strcpy( sp->txt, txt );
#if 0
#ifdef SRVOPT
        if( srvopt_fctrtn() )
            printf( "RTN:fill_sym_entry:%s\n",srv_sym(sp));
#endif
#endif
        return( sp );
}


/* ================================================================= */
/*  Function:    put_sym ()                                          */
/* ================================================================= */

Boolean put_sym ( Char *sym, Char *txt, Boolean perm ) {
        Sym_e_t *sp;

#ifdef SRVOPT
        if( srvopt_function() )
            printf( "FCT:put_sym(\"%s\",\"%s\",%d)\n",sym,txt,perm);
#endif
        /* if already in table, can't put it in symbol table */
        if( sym_in_table( sym ) )
            return( FALSE );

        if( ( sp = fill_sym_entry( sym, txt ) ) != NULL ) {
            sp->perm        = perm;
            add_sym( sp );
        }

        return( sp != NULL );
}


/* ================================================================= */
/*  Function:    update_sym ()                                       */
/* ================================================================= */

Boolean update_sym ( Char *sym, Char *txt, Boolean perm ) {
        Sym_e_t *sp;

#ifdef SRVOPT
        if( srvopt_function() )
            printf( "FCT:update_sym(\"%s\",\"%s\",%d)\n",sym,txt,perm);
#endif
        /* if already in table, delete it from symbol table first */
        if( ( sp = sym_in_table( sym ) ) != NULL ) {
            if( ! perm && sp->perm )
                return( FALSE );
            delete_sym ( sym );
        }

        return( put_sym( sym, txt, perm ) );
}


/* ================================================================= */
/*  Function:    in_macro_list ()                                    */
/* ================================================================= */

Static  struct  pd_macros {
        Char    *match;
        Char    *rtn;
} macros_tmpl[] = {
        { "**",         "$**"           },
        { "*",          "$*"            },
        { "*L",         "$(*L)"         },
        { "*F",         "$(*F)"         },
        { "*M",         "$(*M)"         },
        { "*T",         "$(*T)"         },
        { "*S",         "$(*S)"         },
        { "@",          "$@"            },
        { "@L",         "$(@L)"         },
        { "@F",         "$(@F)"         },
        { "@M",         "$(@M)"         },
        { "@T",         "$(@T)"         },
        { "@S",         "$(@S)"         },
        { "<",          "$<"            },
        { "<L",         "$(<L)"         },
        { "<F",         "$(<F)"         },
        { "<M",         "$(<M)"         },
        { "<T",         "$(<T)"         },
        { "<S",         "$(<S)"         },
        { "?",          "$?"            },
        { "%",          "$%"            },
        { NULL,         NULL            }
};


/* ================================================================= */
/*  Function:    get_sym ()                                          */
/* ================================================================= */

Char    *get_sym ( Char *sym ) {
        struct  pd_macros *pdm;
        Sym_e_t *sp;

#ifdef SRVOPT
        if( srvopt_function() )
            printf( "FCT:get_sym(\"%s\")\n",sym);
#endif
        pdm = macros_tmpl;
        while( pdm->match ) {
            if( strcmp( sym, pdm->match ) == 0 )
                break;
            ++pdm;
        }

        if( making == FALSE ) {
            /* in parse mode: check if symbol is one of the           */
            /* pre-defined ones which needs to be expanded during     */
            /* processing time not at makefile parse time             */
            if( pdm->match != NULL ) {
                return( pdm->rtn );
            }
        }
        else {
            /* in make mode: only return sym that is of builtin type  */
            if( pdm->match == NULL ) {
                reset_buf( &t2 );
                append_buf( &t2, "$" );
                append_buf( &t2, sym );
                return( t2.bp );
            }
        }

        sp      = sym_in_table( sym );

#ifdef SRVOPT
        if( srvopt_fctrtn() )
            printf( "RTN:get_sym:%s\n",srv_sym(sp));
#endif

        return( sp != NULL ? sp->txt : NULL );
}


/* ================================================================= */
/*  Function:    output_translate ()                                 */
/* ================================================================= */

Static
Char    *output_translate ( Char *rp, Char *from, Char *to,
                            Char *end, Int16 line ) {
        Char  *sp;
        Char  *ep;
        Int32 fromsz;
        Char  save_char;

#ifdef SRVOPT
        if( srvopt_function() )
            printf( "FCT:output_translate(\"%s\",\"%s\","
                    "\"%s\",\"%s\",%d)\n",
                    rp!=NULL?rp:"NULL",from,to,end,line);
#endif
        if( rp == NULL ¦¦ *rp == 0 ) {
            return( rp );
        }
        if( *from == 0 ) {
            return( rp );
        }
        /* null terminate the from and to string                      */
        *(to - 1)  = 0;
        *end       = 0;
        fromsz     = strlen( from );
        sp         = rp;
        ep         = strstr( sp, from );
        if( ep != NULL ) {
            reset_buf( &t1 );
            while( ep != NULL ) {
                save_char = *ep;
                *ep = 0;
                append_buf( &t1, sp );
                *ep = save_char;
                append_buf( &t1, to );
                sp = ep + fromsz;
                ep = strstr( sp, from );
            }
            append_buf( &t1, sp );
            rp  = t1.bp;
        }
        *(to - 1)  = '=';
        *end       = ')';
#ifdef SRVOPT
        if( srvopt_fctrtn() )
            printf( "FCT:output_translate:\"%s\"\n",rp);
#endif
        return( rp );
}


/* ================================================================= */
/*  Function:    expand_macro_txt ()                                 */
/* ================================================================= */
/***********************************************************************
      Input:    txt                  - start position to scan
                line                 - input text line no.
      Output:   txt                  - end position for next scan
                msz                  - size of macro expanded
                expand_nested_macro  - ptr to macro expanded
                                       = NULL if msz = 0;
      expand    1. nested macro
                2. output translation
                3.
***********************************************************************/

Static
Char    *expand_macro_txt ( Char **txt, Int32 *msz, Int16 line ) {
        Char   *mp  = *txt;
        Char   *sp;
        Char   *rp;
        Int32  cnt;
        Char   save_char;

#ifdef SRVOPT
        if( srvopt_function() )
            printf( "FCT:expand_macro_txt(\"%s\")\n",*txt);
#endif
        if( *mp == '(' ) {
            ++mp;
            if( *mp == '$' ¦¦ *mp == '&' ) { /* $($.....) expansion   */
                Int32 nmsz;

                /* process nested macro                               */
                ++mp;
                sp   = expand_macro_txt( &mp, &nmsz, line );
                if( *mp != ')' ) {
                    /* macro ()'s mismatch                            */
                    log_error( MISMATCH_PARENTH, NULL, line );
                    exit( TMK_EXIT_FAILURE );
                }
                ++mp;
                if( nmsz != 0 ) {
                    rp        = get_sym( sp );
                    *msz      = ( rp != NULL ) ? strlen( rp ) : 0;
                }
                else {
                    rp        = NULL;
                    *msz      = 0;
                }
                *txt      = mp;
            }
            else {
                /* process output translation and/or regular macros   */
                sp        = skip_macro_sym( mp );
                cnt       = sp - mp;
                if( cnt != 0 ) {   /* $(macro......) expansion        */
                    save_char = *sp;    /* save overwritten char      */
                    *sp       = 0;      /* null terminated macro name */
                    rp        = get_sym( mp );
                    *sp       = save_char; /* restore overwritten char*/
                    mp        = sp;
                    if( *mp == ':' ) { /* $(macro:....) expansion     */
                        Char  *from;
                        Char  *to;
                        Char  *end;

                        /* perform output translation                 */
                        ++mp;
                        from = mp;
                        to   = strchr( mp, '=' );
                        if( to == NULL ) {
                            /* macro ()'s mismatch                    */
                            log_error( INV_OUTPUT_MACRO, NULL, line );
                            exit( TMK_EXIT_FAILURE );
                        }
                        ++to;
                        end  = strchr( to, ')' );
                        if( end == NULL ) {
                            /* macro ()'s mismatch                    */
                            log_error( MISMATCH_PARENTH, NULL, line );
                            exit( TMK_EXIT_FAILURE );
                        }
                        rp = output_translate( rp, from, to, end, line );
                        mp = end;
                    }
                    *msz      = ( rp != NULL ) ? strlen( rp ) : 0;
                }
                else {   /*  $() expands to nothing                   */
                    rp        = NULL;
                    *msz      = 0;
                }
                if( *mp != ')' ) {
                    /* macro ()'s mismatch                            */
                    log_error( MISMATCH_PARENTH, NULL, line );
                    exit( TMK_EXIT_FAILURE );
                }
                *txt      = mp + 1;
            }
        } else {
            /* check for $$ substitution                              */
            if( *mp == '$' ¦¦ *mp == '&' ) {
                rp   = mp;
                *msz = 1;
                *txt = mp + 1;
            }
            else {
                /* non-() macro substitution                      */
                sp        = skip_macro_sym( mp );
                cnt       = sp - mp;
                save_char = *sp;        /* save overwritten char      */
                *sp       = 0;          /* null terminated macro name */
                rp        = get_sym( mp );
                *sp       = save_char;  /* restore overwritten char   */
                *msz      = ( rp != NULL ) ? strlen( rp ) : 0;
                *txt      = sp;
            }
        }

#ifdef SRVOPT
        if( srvopt_fctrtn() )
            printf( "RTN:expand_macro_txt:\"%s\",\"%s\",%d\n",
                     rp?rp:"NULL",*txt,msz) ;
#endif
        return( rp );
}


/* ================================================================= */
/*  Function:    text_substitution ()                                */
/* ================================================================= */

Char    *text_substitution ( Char *txt, Int16 line ) {
        Char    *dp     = subtxt;
        Int16   cnt     = 0;

#ifdef SRVOPT
        if( srvopt_function() )
            printf( "FCT:text_substitution(\"%s\")\n",txt);

        if( srvopt_detail() )
            printf( "DTL:before: \"%s\"\n",txt);
#endif
        while( *txt ) {
            if( *txt != '$' && *txt != '&' ) {
                if( *txt == '\\' ) {
                    /* escape character processing, '\' character     */
                    /* gurantee that there is at least one more char  */
                    /* after to be processed, except it is the last   */
                    /* char in text which has already been taken care */
                    /* of by read_line routines                       */
                    ++txt;
                }
                if( ++cnt >= subtxt_sz ) {
                    /* expand subtxt buffer size                      */
                    expand_subtxt( cnt );
                    dp      = subtxt + cnt - 1;
                }
                *dp++   = *txt++;
            } else {
                /* find macro name from input stream                  */
                Char    *mp;
                Int32   msz;

                ++txt;                      /* skip the first $ char  */
                mp   = expand_macro_txt( &txt, &msz, line );
                if( msz != 0 ) {
                    cnt     += msz;
                    if( cnt >= subtxt_sz ) {
                        /* expand subtxt buffer size                  */
                        expand_subtxt( cnt );
                        dp      = subtxt + cnt - 1;
                    }
                    /* transfer expanded macro text to destination    */
                    while( msz-- ) {
                        *dp++ = *mp++;
                    }
                }
            }
        }
        *dp     = 0;

#ifdef SRVOPT
        if( srvopt_detail() )
            printf( "DTL:after:  \"%s\"\n",subtxt);

        if( srvopt_fctrtn() )
            printf( "RTN:text_substitution:\"%s\"\n",subtxt);
#endif
        return( subtxt );
}


/* ================================================================= */
/*  Function:    bi_macro_substitution ()                            */
/* ================================================================= */

Char    *bi_macro_substitution ( Char *txt ) {
        Char    *dp     = subtxt;
        Int16   cnt     = 0;
        Sym_e_t *sp;
        struct  pd_macros *pdm;

#ifdef SRVOPT
        if( srvopt_function() )
            printf( "FCT:bi_macro_substitution(\"%s\")\n",txt);

        if( srvopt_detail() )
            printf( "DTL:before: \"%s\"\n",txt);
#endif
        while( *txt ) {
            if( *txt != '$' && *txt != '&' ) {
                if( ++cnt >= subtxt_sz ) {
                    /* expand subtxt buffer size                      */
                    expand_subtxt( cnt );
                    dp      = subtxt + cnt - 1;
                }
                *dp++   = *txt++;
            } else {
                /* find macro name from input stream                  */
                Char    *mp;
                Int32   msz;

                pdm = macros_tmpl;
                while( pdm->match ) {
                    if( strstr( txt, pdm->rtn ) == txt )
                        break;
                    ++pdm;
                }

                if( pdm->match != NULL ) {
                    sp   = sym_in_table( pdm->match );
                    mp   = sp->txt;
                    msz  = sp ? strlen( mp ) : 0;
                    txt += strlen( pdm->rtn );
                } else {
                    mp   = txt;
                    msz  = 1;
                    ++txt;
                }
                if( ( cnt + msz ) >= subtxt_sz ) {
                    /* expand subtxt buffer size                      */
                    expand_subtxt( cnt + msz );
                    dp      = subtxt + cnt;
                }
                memcpy( dp, mp, msz );
                dp  += msz;
                cnt += msz;
            }
        }
        *dp     = 0;

#ifdef SRVOPT
        if( srvopt_detail() )
            printf( "DTL:after:  \"%s\"\n",subtxt);

        if( srvopt_fctrtn() )
            printf( "RTN:bi_macro_substitution:\"%s\"\n",subtxt);
#endif
        return( subtxt );
}


/* ================================================================= */
/*  Function:    dump_dict ()                                        */
/* ================================================================= */

Void    dump_dict ( Void ) {
        Boolean dbg;
        Int16   cnt;

        dbg     = opt_debug();
        if( ! dbg /* && ! opt_print_macro()*/ )
            return;

        log_dbg( "*****  Macro Dictionary definitions  "
                 "************************************" );

        for( cnt = 0 ; cnt < DICT_TAB_SZ ; ++cnt ) {
            Sym_e_t *sep;

            if( ( sep = dict_table[cnt] ) != NULL ) {
                while( sep ) {
                    sprintf( txtbuf, "(%s) <%s>  = \"%s\"",
                             sep->perm ? "Perm" : "Temp",
                             sep->sym, sep->txt );
                    log_dbg( txtbuf );
                    sep   = sep->nxt;
                }
            }
        }
        log_dbg( "*************************************"
                 "************************************");
}

