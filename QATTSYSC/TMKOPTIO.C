/* ================================================================= */
/*                                                                   */
/*  MODULE:    TMKOPTIO                                              */
/*                                                                   */
/*                                                                   */
/*  MODULE-TYPE: C Library                                           */
/*                                                                   */
/*  Processor:  C                                                    */
/*                                                                   */
/*  Purpose:    Command option processing                            */
/*                                                                   */
/* ================================================================= */


#include        <stdio.h>
#include        <string.h>
#include        <stdlib.h>
#include        <signal.h>

#include        "tmkhbase.qattsysc"
#include        "tmkhdict.qattsysc"
#include        "tmkhutil.qattsysc"
#include        "tmkhfile.qattsysc"
#include        "tmkhpars.qattsysc"
#include        "tmkhopti.qattsysc"
#include        "tmkhmsgh.qattsysc"
#include        "tmkhopna.qattsysc"

#if 0
#include        "fdebug.h"
#endif

/***********************************************************************
        Variables definitions
***********************************************************************/

Static  Char        makefile_name[ PATH_SZ ]; /* makefile base name   */
Static  Char        *makefile_mbr;        /* makefile_name mbr insert */
Static  Mbr_list_t  *mlist  = NULL;       /* makefile mbr list        */

Static  Int16       options = OPT_DEFAULT;/* STRMAKE options flags    */
Static  Targ_list_t *tlist  = NULL;       /* target list              */

Static  Int16       src_left_margin;      /* source file left margin  */
Static  Int16       src_right_margin;     /* source file right margin */

Static  Int16       rtncde_handling = RTN_EXCEPTION;/* rtncde handling*/
Static  Int16       rtncde_severity = RTN_DEF_SEV; /* default severity*/

Static  Char        *macro_argv;          /* command macros list      */


#ifdef SRVOPT
Static  Int16       srv_option = SRVOPT_NONE; /* service option flags */
Static  Char        srv_buf[WRKBUF_SZ];
        Char        srv_cat[WRKBUF_SZ];

Boolean srvopt_function    ( Void ) {
    return srv_option & SRVOPT_FUNCTION;
}
Boolean srvopt_fctrtn      ( Void ) {
    return srv_option & SRVOPT_FCTRTN;
}
Boolean srvopt_detail      ( Void ) {
    return srv_option & SRVOPT_DETAIL;
}
Boolean srvopt_dump_macro  ( Void ) {
    return srv_option & SRVOPT_DUMP_MACRO;
}
Boolean srvopt_dump_bi     ( Void ) {
    return srv_option & SRVOPT_DUMP_BI;
}
Boolean srvopt_dump_rules  ( Void ) {
    return srv_option & SRVOPT_DUMP_RULES;
}
Boolean srvopt_dump_suffix ( Void ) {
    return srv_option & SRVOPT_DUMP_SUFFIX;
}
Boolean srvopt_dump_defcmd ( Void ) {
    return srv_option & SRVOPT_DUMP_DEFCMD;
}
Boolean srvopt_dump_io     ( Void ) {
    return srv_option & SRVOPT_DUMP_IO;
}
Char    *srv_fs( File_spec_t *fs ) {
        if( fs == NULL )
            return( "NULL" );

        sprintf(srv_buf,"{%s/%s(%s)<%s%s%s>,%d,%d,%d}", fs->lib,fs->file,
             fs->extmbr,fs->type,
             fs->seu_type!=NULL&&*fs->seu_type!=0?",":"",fs->seu_type,
             fs->is_file,fs->last_update,fs->create_date);

        return( srv_buf );
}
Char    *srv_e( Element_t *e ) {
        Char  fsbuf[256];
        if( e == NULL )
            return( "NULL" );

        sprintf( fsbuf, srv_fs( &e->fs ) );
        sprintf( srv_buf, "{,%d,%s,\"%s\"}", e->maked,fsbuf,
                 (e->name!=NULL)?e->name:"");
        return( srv_buf );
}
Char    *srv_rule( Rules_t *r ) {
        Char    target[100];
        Char    depend[100];
        Char    cmd   [100];
        Char    *cp;

        if( r == NULL )
            return( "NULL" );

        strcpy( target, srv_e(r->target));
        strcpy( depend, srv_e(r->dependent));
        cp = srv_cmd( r->cmd );
        strncpy( cmd, cp, Min( strlen(cp),99) + 1 );
        sprintf( srv_buf, "{,%s,%s,\n     %s,\n     %d,%d,%d,%d}",
                 target,depend,cmd,
                 r->op,r->line,r->recursive,r->implicit_rule);
        return( srv_buf );
}
Char    *srv_cmd( Cmd_t *c ) {
        if( c == NULL )
            return( "NULL" );

        sprintf( srv_buf, "{,%d,\"%s\"}", c->line,c->cmd_txt);
        return( srv_buf );
}
Char    *srv_sym( Sym_e_t *s ) {
        if( s == NULL )
            return( "NULL" );

        sprintf( srv_buf, "{,\"%s\",\"%s\",%d}",
                 s->sym,s->txt,s->perm );
        return( srv_buf );
}


Static  set_srvopt( void *opt ) {
    Int16    cnt  = *(Int16 *)opt;
    Char     *op  = ( (Char *)opt ) + 2;

    while( cnt-- ) {
        switch( *op ) {
        case 'N'  :  srv_option  = SRVOPT_NONE                 ; break;
        case 'F'  :  srv_option ¦= SRVOPT_FUNCTION             ; break;
        case 'G'  :  srv_option ¦= SRVOPT_FCTRTN               ; break;
        case 'D'  :  srv_option ¦= SRVOPT_DETAIL               ; break;
        case 'M'  :  srv_option ¦= SRVOPT_DUMP_MACRO           ; break;
        case 'B'  :  srv_option ¦= SRVOPT_DUMP_BI              ; break;
        case 'R'  :  srv_option ¦= SRVOPT_DUMP_RULES           ; break;
        case 'S'  :  srv_option ¦= SRVOPT_DUMP_SUFFIX          ; break;
        case 'C'  :  srv_option ¦= SRVOPT_DUMP_DEFCMD          ; break;
        case 'I'  :  srv_option ¦= SRVOPT_DUMP_IO              ; break;
        case 'A'  :  srv_option ¦=
                           SRVOPT_FUNCTION    ¦ SRVOPT_FCTRTN      ¦
                           SRVOPT_DETAIL      ¦
                           SRVOPT_DUMP_MACRO  ¦ SRVOPT_DUMP_BI     ¦
                           SRVOPT_DUMP_RULES  ¦ SRVOPT_DUMP_SUFFIX ¦
                           SRVOPT_DUMP_DEFCMD ¦ SRVOPT_DUMP_IO ; break;
        }
        ++op;
    }
}

#endif /* ifdef SRVOPT */


/* ================================================================= */
/*  Function:    no_of_makefile_mbr ()                               */
/* ================================================================= */

Int32   no_of_makefile_mbr ( Void ) {
        Mbr_list_t  *mlp = mlist;
        Int32       cnt = 0;

#ifdef SRVOPT
        if( srvopt_function() )
            printf("FCT:no_of_makefile_mbr(Void)\n");
#endif
        /* loop through all members and return count                  */
        while( mlp ) {
            ++cnt;
            mlp  = mlp->nxt;
        }
#ifdef SRVOPT
        if( srvopt_fctrtn() )
            printf("RTN:no_of_makefile_mbr:%d\n",cnt);
#endif
        return( cnt );
}


/* ================================================================= */
/*  Function:    next_makefile_mbr ()                                */
/* ================================================================= */

Char    *next_makefile_mbr ( Void ) {
        Mbr_list_t  *mlp = mlist;

#ifdef SRVOPT
        if( srvopt_function() )
            printf("FCT:next_makefile_mbr(Void)\n");
#endif
        /* use the same mlist var to maintain next member to process  */
        sprintf( makefile_mbr, "(%s)", mlist->txt );
        mlist = mlist->nxt;

#ifdef SRVOPT
        if( srvopt_fctrtn() )
            printf("RTN:next_makefile_mbr:\"%s\"\n",makefile_name);
#endif
        return( makefile_name );
}


/* ================================================================= */
/*  Function:    is_macro ()                                         */
/* ================================================================= */

Static
Char    *is_macro ( Char *txt ) {
        Char   *rp;

#ifdef SRVOPT
        if( srvopt_function() )
            printf("FCT:is_macro(\"%20.20s\")\n", txt );
#endif
        rp = strchr( txt, '=' );

#ifdef SRVOPT
        if( srvopt_fctrtn() )
            printf("RTN:is_macro:\"%s\"\n",rp!=NULL?rp:"NULL");
#endif
        return( rp );
}


/* ================================================================= */
/*  Function:    setup_command_macro ()                              */
/* ================================================================= */

Void    setup_command_macro ( Void ) {
        Int16           cnt;
        Char            *cp;
        Int32           i;
        Int32           txtsz;

#ifdef SRVOPT
        if( srvopt_function() )
            printf("FCT:setup_command_macro(Void)\n");
#endif

        /* process macro definitions                                  */
        cnt = *(Int16 *)macro_argv;
        cp  = macro_argv + 2;
        for( i = 0 ; i < cnt ; ++i ) {
            Char    *tp;

            if( ( tp = is_macro( cp ) ) != NULL ) {
                *tp++  = 0;
                skip_trail_spaces( cp );
                tp     = skip_white_spaces( tp );
                txtsz  = skip_trail_spaces_sz( tp, CMDF_MACRO - (tp-cp));
                memcpy( txtbuf, tp, txtsz );
                txtbuf[ txtsz ] = 0;
                update_sym( cp, txtbuf, TRUE );
#ifdef SRVOPT
        if( srvopt_dump_macro() )
            printf("MACRO: \"%s\" = \"%s\"\n", cp, txtbuf );
#endif
            } else {
                txtsz  = skip_trail_spaces_sz( cp, CMDF_MACRO );
                sprintf( txtbuf, "\"%*s\"", txtsz, cp );
                log_error( INV_MACRO_DEF, txtbuf, MSG_NO_LINE_NO );
            }
            cp      += CMDF_MACRO;
        }
}


/* ================================================================= */
/*  Function:    process_options ()                                  */
/* ================================================================= */

Void    process_options ( int argc, Char **argv ) {
        Int16           cnt;
        Int16           i;
        int             txtsz;     /* int to match printf("%*") type */
        Char            *cp;
        Void            (*old_signal_fct)( int );
        Void            *np = NULL;

#ifdef SRVOPT
        set_srvopt( argv[ARGV_SRVOPT] );

        if( srvopt_function() )
            printf("FCT:process_options(%d,Char *argv)\n", argc );
#endif
        /* process options                                            */
        cnt = *(Int16 *)argv[ARGV_OPTIONS];
        cp  = argv[ARGV_OPTIONS] + 2;
        for( i = 0 ; i < cnt ; ++i ) {
            switch( *cp ) {
            case 'J'        :  /* NOIGNORE */
                    options &= ~OPT_IGNORE;
                    break;
            case 'I'        :  /* IGNORE   */
                    options ¦= OPT_IGNORE;
                    break;
            case 'V'        :  /* NOSILENT */
                    options &= ~OPT_SILENT;
                    break;
            case 'S'        :  /* SILENT */
                    options ¦= OPT_SILENT;
                    break;
            case 'C'        :  /* NOBIRULES */
                    options ¦= OPT_NOBIRULES;
                    break;
            case 'B'        :  /* BIRULES */
                    options &= ~OPT_NOBIRULES;
                    break;
            case 'F'        :  /* NOEXEC */
                    options ¦= OPT_NOEXEC;
                    break;
            case 'E'        :  /* EXEC */
                    options &= ~OPT_NOEXEC;
                    break;
            case 'U'        :  /* NOTOUCH */
                    options &= ~OPT_TOUCH;
                    break;
            case 'T'        :  /* TOUCH */
                    options ¦= OPT_TOUCH;
                    break;
            case 'X'        :  /* NODEBUG */
                    options &= ~OPT_DEBUG;
                    break;
            case 'D'        :  /* DEBUG */
                    options ¦= OPT_DEBUG;
                    break;
            }
            ++cp;
        }
#ifdef SRVOPT
        if( srvopt_detail() )
            printf("DTL:process_option:option flag = %04.4x\n",
                      options );
#endif
        /* process target field                                      */
        cnt = *(Int16 *)argv[ARGV_TARGET];
        cp  = argv[ARGV_TARGET] + 2;

#ifdef SRVOPT
        if( srvopt_detail() )
            printf("DTL:process_option:target count = %d\n", cnt );
#endif
        for( i = 0 ; i < cnt ; ++i ) {
            Targ_list_t     *tlp;

            if( strncmp( cp, CMDF_TARGET_DEFAULT,
                        sizeof(CMDF_TARGET_DEFAULT) - 1 ) == 0 ) {
                /* skip *FIRST target name for default */
                continue;
            }

            txtsz   = skip_trail_spaces_sz ( cp, CMDF_TARGET );
            tlp     = (Targ_list_t *)alloc_buf(
                            sizeof( Targ_list_t ) + txtsz + 1,
                            "process_options()" );
            memcpy( tlp->txt, cp, txtsz );
            tlp->txt[ txtsz ]       = 0;

            /* put the new target at end of target list */
            tlp->nxt        = NULL;
            if( tlist == NULL ) {
                np      = tlp;
                tlist   = tlp;
            }
            else {
                ((Targ_list_t *)np)->nxt  = tlp;
                np       = tlp;
            }

            if( opt_debug() ) {
                sprintf( txtbuf, "Target:<%d> \"%s\"", txtsz, tlp->txt);
                log_dbg( txtbuf );
            }
            cp += CMDF_TARGET;
        }

        /* process makefile definition                               */
        cp      = argv[ARGV_SRCFILE] + CMDF_LIBFILE / 2;
        txtsz   = skip_trail_spaces_sz ( cp, CMDF_LIBFILE / 2 );
        memcpy( makefile_name, cp, txtsz );
        cp      = makefile_name + txtsz;
        *cp++   = '/';
        txtsz   = skip_trail_spaces_sz( argv[2], CMDF_LIBFILE / 2 );
        memcpy( cp, argv[ARGV_SRCFILE], txtsz );
        cp      += txtsz;
        *cp     = 0;
        makefile_mbr = cp;         /* remember member insertion point*/

        /* build the makefile member list from command input         */
        if( strncmp( argv[ARGV_SRCMBR], "*ALL", 4 ) == 0 ) {
            Mbr_list_t    *mlp;      /* makefile mbr list tmp ptr    */
            mbrl0100      *mli;      /* System API member output list*/
            header_struct *list_header;
            Int32         mbr_count;

            creat_usrspc();
            obj_not_exist_flag      = LSTOBJ_FOUND;
            old_signal_fct = signal( SIGABRT, &obj_not_exist_trap );
            QUSLMBR( LST_USRSPC_NM, "MBRL0100",
                argv[ARGV_SRCFILE], argv[ARGV_SRCMBR], "1" );
            signal( SIGABRT, old_signal_fct );
            if( obj_not_exist_flag == LSTOBJ_NOT_FOUND ) {
                log_error( CANT_OPEN_MAKEFILE, makefile_name,
                           MSG_NO_LINE_NO );
                exit( TMK_EXIT_FAILURE );
            }

            QUSPTRUS( LST_USRSPC_NM, &list_header );
            mli       = (mbrl0100 *)(((char *)list_header) +
                          list_header->list_section_offset );
            mbr_count = list_header->number_of_entries;

            if( obj_not_exist_flag == LSTOBJ_EXCEED_USRSPC ) {
                sprintf( makefile_mbr+1, "%5.5d%s",
                         mbr_count, makefile_name );
                log_error( TOO_MANY_MBR, makefile_mbr + 1,
                           MSG_NO_LINE_NO );
            }


#ifdef SRVOPT
            if( srvopt_detail() )
                printf("DTL:process_option:member list, count = %d\n",
                       mbr_count );
/* DBG_DP((char *)mli, 40 ); */
#endif
            if( mbr_count == 0 ) {
                log_error( ALL_NO_MBR, makefile_name,
                           MSG_NO_LINE_NO );
                exit( TMK_EXIT_FAILURE );
            }

            while( mbr_count-- ) {
                txtsz = skip_trail_spaces_sz(mli->member_name,CMDF_MBR);
                mlp   = (Mbr_list_t *)alloc_buf(
                                sizeof( Mbr_list_t ) + txtsz + 1,
                                "process_options()" );
                memcpy( mlp->txt, mli->member_name, txtsz );
                mlp->txt[ txtsz ]       = 0;
                mlp->nxt        = NULL;

                /* put the new target at end of target list */
                if( mlist == NULL ) {
                    np      = mlp;
                    mlist   = mlp;
                }
                else {
                    ((Mbr_list_t *)np)->nxt = mlp;
                    np      = mlp;
                }

                if( opt_debug() ) {
                    sprintf( txtbuf, "Makefile member:\"%s\"",
                             ((Mbr_list_t *)np)->txt);
                    log_dbg( txtbuf );
                }
                mli++;
            }
        }
        else {
            txtsz = skip_trail_spaces_sz( argv[ARGV_SRCMBR], CMDF_MBR );
            mlist   = (Mbr_list_t *)alloc_buf(
                            sizeof( Mbr_list_t ) + txtsz + 1,
                            "process_options()" );
            memcpy( mlist->txt, argv[ARGV_SRCMBR], txtsz );
            mlist->txt[ txtsz ] = 0;
            mlist->nxt          = NULL;
        }

        if( opt_debug() ) {
            sprintf( txtbuf, "MAKEFILE = \"%s\"", makefile_name );
            log_dbg( txtbuf );
        }

        /* remember the marco definition from command for later       */
        /*  processing                                                */
        macro_argv = argv[ ARGV_MACRO ];

        /* process Margin definitions                                 */
        src_left_margin  = *( ( (Int16 *)argv[ARGV_MARGINS] ) + 1 );
        src_right_margin = *( ( (Int16 *)argv[ARGV_MARGINS] ) + 2 );

#ifdef SRVOPT
        if( srvopt_detail() )
            printf("DTL:process_option:"
                   "Source Margin: left = %d,  right = %d\n",
                   src_left_margin, src_right_margin );

        /* DBG_DP((char *)argv[ARGV_MARGINS], 16 );                */
#endif

        if( src_left_margin >= src_right_margin ) {
            log_error( INV_MARGIN_SPEC, NULL, MSG_NO_LINE_NO );
            src_left_margin = 0;
        }

        /* process Return Code Handling definitions                   */
        rtncde_handling = *( ( (Int16 *)argv[ARGV_RTNCDE] ) + 1 );
        rtncde_severity = *( ( (Int16 *)argv[ARGV_RTNCDE] ) + 2 );

#ifdef SRVOPT
        if( srvopt_detail() )
            printf("DTL:process_option:"
                   "rtncde handling: method = %d, severity = %d\n",
                   rtncde_handling, rtncde_severity );

        /* DBG_DP((char *)argv[ARGV_RTNCDE], 16 ); */
#endif

        /* process User Message Logging definitions                   */
        if( argv[ARGV_USRMSG][0] == 'S' ) {
            set_usrmsg_to_session();
        }
}


/* ================================================================= */
/*  Function:    get_first_requested_target ()                       */
/* ================================================================= */

Static  Targ_list_t     *cur_tlist      = NULL;

Char    *get_first_requested_target ( Void ) {
        Char  * rp;
#ifdef SRVOPT
        if( srvopt_function() )
            printf("FCT:get_first_requested_target(Void)\n");
#endif
        rp = ( cur_tlist = tlist ) != NULL ? tlist->txt : NULL;

#ifdef SRVOPT
        if( srvopt_fctrtn() )
            printf("RTN:get_first_requested_target"
                   "\"%s\"\n",rp!=NULL?rp:"NULL");
#endif
        return( rp );
}


/* ================================================================= */
/*  Function:    get_next_requested_target ()                        */
/* ================================================================= */

Char    *get_next_requested_target ( Void ) {
        Char  *rp = NULL;
#ifdef SRVOPT
        if( srvopt_function() )
            printf("FCT:get_next_requested_target(Void)\n");
#endif
        if( cur_tlist != NULL && cur_tlist->nxt != NULL ) {
            cur_tlist       = cur_tlist->nxt;
            rp = cur_tlist->txt;
        }

#ifdef SRVOPT
        if( srvopt_fctrtn() )
            printf("RTN:get_next_requested_target:"
                   "\"%s\"\n",rp!=NULL?rp:"NULL");
#endif
        return( rp );
}


/***********************************************************************
        option get margin functions
***********************************************************************/

/* ================================================================= */
/*  Function:    opt_get_left_margin ()                              */
/* ================================================================= */

Int16   opt_get_left_margin( Void ) {
        return( src_left_margin );
}


Int16   opt_get_right_margin( Void ) {
        return( src_right_margin );
}


/* ================================================================= */
/*  Function:    opt_get_rtncde_methods ()                           */
/* ================================================================= */
/***********************************************************************
        option get return code handling parameters
***********************************************************************/

Int16   opt_get_rtncde_methods ( Void ) {
        return( rtncde_handling );
}

Int16   opt_get_rtncde_sev ( Void ) {
        return( rtncde_severity );
}


/* ================================================================= */
/*  Function:    opt_set_ingore ()                                   */
/* ================================================================= */
/***********************************************************************
        option set indicator functions
***********************************************************************/

Void    opt_set_ingore ( Void ) {
#ifdef SRVOPT
        if( srvopt_function() )
            printf("FCT:opt_set_ingore(Void)\n");
#endif
        options ¦= OPT_IGNORE;
}

Void    opt_set_silent ( Void ) {
#ifdef SRVOPT
        if( srvopt_function() )
            printf("FCT:opt_set_silent(Void)\n");
#endif
        options ¦= OPT_SILENT;
}


/***********************************************************************
        option indicator functions
***********************************************************************/

Boolean opt_ignore_err_code ( void ) {
        return( options & OPT_IGNORE );
}

Boolean opt_silent_mode ( void ) {
        return( options & OPT_SILENT );
}

Boolean opt_no_built_in_rule ( void ) {
        return( options & OPT_NOBIRULES );
}

Boolean opt_no_execute ( void ) {
        return( options & OPT_NOEXEC );
}

Boolean opt_touch_target ( void ) {
        return( options & OPT_TOUCH );
}

Boolean opt_debug ( void ) {
        return( options & OPT_DEBUG );
}

