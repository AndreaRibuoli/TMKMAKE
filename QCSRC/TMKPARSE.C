# 15 "TMKPARSE.C"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <signal.h>
#include <xxfdbk.h>

#include "tmkbase.h"
#include "tmkdict.h"
#include "tmkutil.h"
#include "tmkfile.h"
#include "tmkparse.h"
#include "tmkoptio.h"
#include "tmkbuilt.h"
#include "tmkopna.h"
#include "tmkmsghd.h"





Static Int16 max_mf_lvl= INCLUDE_LVL + 1;
Static Int16 cur_mf_lvl= 0;
Static Incl_t *mf = NULL;
Static Incl_t *mf_base = NULL;

Static Int16 max_cd_lvl= CD_LVL+1;
Static Int16 cur_cd_lvl= 0;
Static Cd_t *cd = NULL;
Static Cd_t *cd_base = NULL;

Static Int16 cur_line;
Static Int16 rd_line;

Static Int16 ibuf1_sz= WRKBUF_SZ;
Static Char *ibuf1;
Static Char *nxt_ip;

Static Int16 ibuf2_sz= 0;
Static Char *ibuf2 = NULL;

Static
struct keyword {
        Char *match_txt;
        Int16 op;
        Char look_char;
} kw[] = {
        { ".IGNORE", OP_IGNORE, '\0' },
        { ".SILENT", OP_SILENT, '\0' },
        { ".SUFFIXES", OP_SUFFIXES, ':' },
        { ".PRECIOUS", OP_PRECIOUS, ':' }
};

#define KW_TBL_SZ (sizeof(kw)/sizeof(struct keyword))

Static Work_t cur_wrk = { NULL, NULL, NULL, -1, -1 };

Static Rules_t *head = NULL;
Static Rules_t *next_rule = NULL;






Static Void free_element ( Element_t *e );
Int32 evaluate_exp ( Char *tp, Int16 line );





Static
Element_t *expand_elem_list ( Char *txt, Int16 line ) {
        Char *np = txt;
        Char *tp;
        Int16 len;

        File_spec_t fs;
        Element_t *hp = NULL;
        Element_t *nep = NULL;
        Element_t *ep;

#ifdef SRVOPT
        if( srvopt_function() )
            printf("FCT:expand_elem_list(\"%s\",%d)\n",
                     txt?txt:"NULL", line );
#endif
        while( np != NULL && *np ) {

            if( ( txt = parse_obj_name( txt, &fs, &len, line ) ) ==
                    NULL ) {
                fs.lib[0] =
                fs.file[0] =
                fs.extmbr[0] =
                fs.type[0] =
                fs.seu_type[0]= 0;
                fs.obj_type = '*';
                fs.is_file = FALSE;
                tp = skip_non_white_spaces( np );
                len = tp - np;
                txt = skip_white_spaces( tp );

            }
            np[len] = 0;

            ep = ( Element_t * )alloc_buf( sizeof( Element_t ) +
                            len , "expand_elem_list()" );
            ep->fs = fs;
            strcpy( ep->name, np );

            if( nep == NULL ) {
                hp = ep;
            }
            else {
                nep->nxt= ep;
            }
            nep = ep;
            ep->nxt = NULL;

            np = txt;
        }
#ifdef SRVOPT
        if( srvopt_fctrtn() )
            printf("RTN:expand_elem_list:%s\n",srv_e(hp));
#endif
        return( hp );
}






Static
Element_t *duplicate_elem_list ( Element_t *d ) {
        Element_t *hp = NULL;
        Element_t *np = NULL;
        Element_t *ep;

#ifdef SRVOPT
        if( srvopt_function() )
            printf("FCT:duplicate_elem_list(Element_t *d)\n" );
#endif
        while( d ) {
            ep = ( Element_t * )alloc_buf( sizeof( Element_t ) +
                            strlen( d->name ), "duplicate_elem_list()" );
            *ep = *d;
            strcpy( ep->name, d->name );

            if( np == NULL ) {
                hp = ep;
            }
            else {
                np->nxt = ep;
            }

            np = ep;
            ep->nxt = NULL;
            d = d->nxt;
        }
#ifdef SRVOPT
        if( srvopt_fctrtn() )
            printf("RTN:duplicate_elem_list:%s\n",srv_e(hp));
#endif
        return( hp );
}






Rules_t *in_rule_list ( Char *name ) {
        Rules_t *rp = head;

#ifdef SRVOPT
        if( srvopt_function() )
            printf("FCT:in_rule_list(\"%s\")\n",name);
#endif
        while( rp ) {
            if( strcmp( rp->target->name, name ) == 0 )
                break;
            rp = rp->nxt;
        }
#ifdef SRVOPT
        if( srvopt_fctrtn() )
            printf("RTN:in_rule_list:%s\n",srv_rule(rp));
#endif
        return( rp );
}






Static
Rules_t *rules_add_tail ( Element_t *t, Element_t *d ) {
        Rules_t *rp;

#ifdef SRVOPT
        if( srvopt_function() )
            printf("FCT:rules_add_tail(Element_t &t, Element_t *d)\n" );
#endif
        rp = ( Rules_t * )alloc_buf( sizeof( Rules_t ),
                                          "rules_add_tail()" );
        rp->target = t;
        rp->dependent = d;
        rp->cmd = cur_wrk.cmd;
        rp->op = cur_wrk.op;
        rp->line = cur_wrk.line;
        rp->recursive = FALSE;
        rp->implicit_rule= FALSE;

        if( head == NULL ) {
            head = rp;
        } else {
            Rules_t *np = head;
            while( np->nxt )
                np = np->nxt;
            np->nxt = rp;
        }
        rp->nxt = NULL;

#ifdef SRVOPT
        if( srvopt_fctrtn() )
            printf("RTN:rule_add_tail:%s\n",srv_rule(rp));
#endif
        return( rp );
}






Void add_inf_rule_to_rule_tail ( Element_t *tep, Element_t *dep,
                                    Cmd_t *cmd ) {
        Rules_t *rp;

#ifdef SRVOPT
        if( srvopt_function() )
            printf("FCT:add_inf_rule_to_rule_tail(Element_t &t,"
                       "Element_t *d,Cmd_t *cmd)\n" );
#endif
        if( dep ) {
            dep->nxt = NULL;
        }
        cur_wrk.cmd = cmd;
        cur_wrk.op = OP_RULES;
        cur_wrk.line = -1;
        rp = rules_add_tail( tep, dep );
        rp->implicit_rule = TRUE;
}






Static
Void rules_add_dependents ( Rules_t *rp, Element_t *t, Element_t *d ){
        Element_t *rep = rp->dependent;

#ifdef SRVOPT
        if( srvopt_function() )
            printf("FCT:rules_add_dependents(Rules_t *rp,Element_t &t,"
                       "Element_t *d)\n" );
#endif
        if ( rep == NULL ) {
            rp->dependent = d;
        }
        else {
            while( rep->nxt ) {
                rep = rep->nxt;
            }
            rep->nxt = d;
        }
}






Static
Void cmd_add_tail ( Cmd_t **hd, Char *cmd, Int16 line ) {
        Int16 len = sizeof( Cmd_t ) + strlen( cmd );
        Cmd_t *cp;

#ifdef SRVOPT
        if( srvopt_function() )
            printf("FCT:cmd_add_tail(Cmd_t **hd,\"%s\",%d)\n",
                    cmd?cmd:"NULL",line );
#endif
        cp = ( Cmd_t * )alloc_buf( len, "cmd_add_tail()" );

        strcpy( cp->cmd_txt, cmd );
        cp->line = line;

        if( *hd == NULL ) {
            *hd = cp;
        } else {
            Cmd_t *np = *hd;
            while( np->nxt ) {
                np = np->nxt;
            }
            np->nxt = cp;
        }
        cp->nxt = NULL;
}






Void free_cmd ( Cmd_t *cp ) {
        Cmd_t *ncp;

#ifdef SRVOPT
        if( srvopt_function() )
            printf("FCT:free_cmd(\"%s\")\n",
                    cp ? cp->cmd_txt : "" );
#endif
        while( cp ) {
            ncp = cp->nxt;
            free( cp );
            cp = ncp;
        }
}






Char *has_dyn_macro ( Char *txt ) {
        Char *rp;

#ifdef SRVOPT
        if( srvopt_function() )
            printf("FCT:has_dyn_macro(\"%s\")\n",txt);
#endif
        ( ( rp = strstr( txt, "$@" ) ) != NULL ) ||
        ( ( rp = strstr( txt, "$(@L)" ) ) != NULL ) ||
        ( ( rp = strstr( txt, "$(@F)" ) ) != NULL ) ||
        ( ( rp = strstr( txt, "$(@M)" ) ) != NULL ) ||
        ( ( rp = strstr( txt, "$(@T)" ) ) != NULL ) ||
        ( ( rp = strstr( txt, "$(@S)" ) ) != NULL );

#ifdef SRVOPT
        if( srvopt_fctrtn() )
            printf("RTN:has_dyn_macro:\"%s\"\n",rp?rp:"NULL");
#endif
        return( rp );
}






Static Buf_t dyn_buf;

Static
Element_t *expand_depend_dyn_name ( Element_t *target,
                                Element_t **depend, Int16 line ) {
        Element_t *dp = *depend;
        Element_t *pdp = NULL;
        Element_t *ndp;
        Char *txtp;
        Char *strtp;
        Int16 len;

#ifdef SRVOPT
        if( srvopt_function() ) {
            sprintf(srv_cat,"FCT:expand_depend_dyn_name(%s,\n     ",
                            srv_e(target) );
            strcat(srv_cat,srv_e(*depend));
            printf( "%s,%d)\n",srv_cat,line);
        }
#endif


        while( dp ) {
            reset_buf( &dyn_buf );
            strtp = dp->name;


            if( ( txtp = has_dyn_macro( strtp ) ) != NULL ) {
                Char *cp;
                Int16 adj;



                do {
                    if( txtp[1] == '@' ) {
                        cp = target->name;
                        adj = 2;
                    }
                    else {
                        switch( txtp[3] ) {
                        case 'L' : cp = target->fs.lib; break;
                        case 'M' : cp = target->fs.extmbr; break;
                        case 'F' : cp = target->fs.file; break;
                        case 'T' : cp = target->fs.type; break;
                        case 'S' : cp = target->fs.seu_type; break;
                        }
                        adj = 5;
                    }

                    *txtp = 0;
                    append_buf( &dyn_buf, strtp );

                    append_buf( &dyn_buf, cp );
                    strtp = txtp + adj;
                    *txtp = '$';
                } while( ( txtp = has_dyn_macro( strtp ) ) != NULL );


                append_buf( &dyn_buf, strtp );


                ndp = ( Element_t * )alloc_buf( sizeof( Element_t ) +
                      dyn_buf.tp-dyn_buf.bp,
                      "expand_depend_dyn_name" );
                ndp->nxt = dp->nxt;
                strcpy( ndp->name, dyn_buf.bp );
                if( pdp == NULL ) {
                    *depend = ndp;
                }
                else {
                    pdp->nxt = ndp;
                }
                free( dp );
                dp = ndp;

                parse_obj_name( dyn_buf.bp, &dp->fs, &len, line );
            }
            pdp = dp;
            dp = dp->nxt;
        }



        dp = *depend;
        pdp = NULL;
        while( dp ) {
            if( *dp->name == 0 ) {
                if( pdp != NULL ) {
                    pdp->nxt = dp->nxt;
                    free( dp );
                    dp = pdp->nxt;
                }
                else {
                    *depend = dp->nxt;
                    free( dp );
                    dp = *depend;
                }
            }
            else {
                pdp = dp;
                dp = dp->nxt;
            }
        }


        if( *depend != NULL && target->nxt != NULL )
            dp = duplicate_elem_list( *depend );
        else
            dp = *depend;

#ifdef SRVOPT
        if( srvopt_fctrtn() ) {
            printf("RTN:expand_depend_dyn_name:%s\n",srv_e(dp) );
        }
#endif
        return( dp );
}







Void CPF4102_handler ( int sig ) {
#ifdef __ILEC400__
        _INTRPT_Hndlr_Parms_T excinfo;
        error_rtn errcode;
#else
        sigdata_t *data;
        sigact_t *act;
#endif
        Int32 cpfmsg;


#ifdef __ILEC400__
        _GetExcData (&excinfo);
#else
        data = sigdata();
#endif

#ifdef __ILEC400__
        if( memcmp( excinfo.Msg_Id, "CPF4102", 7 )) {
#else
        if( memcmp( data->exmsg->exmsgid, "CPF4102", 7 )
            || memcmp( data->exmsg->exmsglib, "*CURLIB", 7 )) {
#endif


                return;
        }

#ifdef __ILEC400__
        errcode.rtn_area_sz = 0;
        QMHCHGEM (&(excinfo.Target),
        0,
        excinfo.Msg_Ref_Key,
#if DEBUG
                opt_debug() ? MOD_HANDLE : MOD_RMVLOG,
#else
        MOD_RMVLOG,
#endif
        "",
        0,
        &errcode);
#else


        act = data->sigact;
        act->xhalt =
        act->xpmsg =
        act->xumsg =
        act->xdebug =
        act->xdecerr =
        act->xresigprior=
        act->xresigouter=
        act->xrtntosgnler= 0;
        act->xremovemsg =
#endif
#if DEBUG
                opt_debug() ? 0 : 1;
#else
                1;
#endif
}






Static
Boolean open_source_file ( Incl_t *mf ) {
#ifdef __ILEC400__
        _XXOPFB_T *ofb;
#else
        XXOPFB_T *ofb;
#endif
        Int16 margin;

#ifdef SRVOPT
        if( srvopt_function() )
            printf("FCT:open_source_file( \"%s\"\n", mf->name );
#endif

        signal (SIGIO, &CPF4102_handler );
        if( ( mf->f = _Ropen( mf->name, "rr" ) ) == NULL ) {
#ifdef SRVOPT
        if( srvopt_fctrtn() )
            printf("RTN:open_source_file:FALSE\n" );
#endif
            return( FALSE );
        }


        ofb = _Ropnfbk( mf->f );
        mf->rec_len = ofb->pgm_record_len;
        if( ( margin = opt_get_left_margin() ) == 0 ||
              !memcmp( strchr( mf->name, '(' ), "(BUILTIN)", 9 ) ) {
            mf->left_margin = ofb->src_file_indic == 'Y' ? 12 : 0;
            mf->right_margin = ofb->pgm_record_len - 1;
        }
        else {
            mf->left_margin = margin - 1;
            mf->right_margin = opt_get_right_margin() - 1;
        }


        if( ( mf->rec_len + 1 ) > ibuf1_sz ) {
            ibuf1_sz = (mf->rec_len / WRKBUF_SZ) * WRKBUF_SZ + WRKBUF_SZ;
            free( ibuf1 );
            ibuf1 = (Char *)alloc_buf( ibuf1_sz, "open_source()" );
        }
        mf->eof = FALSE;
#ifdef SRVOPT
        if( srvopt_detail() ) {
            printf("DTL:open_source_file:left/right margin = %d/%d\n",
                        mf->left_margin, mf->right_margin );
            printf("DTL:open_source_file:record len        = %d\n",
                        mf->rec_len );
            printf("DTL:open_source_file:"
                     "file = %x(%c)  pgm_record_len = %d(%x) "
                         "max_rcd = %d(%x)\n",
                     ofb->src_file_indic, ofb->src_file_indic,
                     ofb->pgm_record_len, ofb->pgm_record_len,
                     ofb->max_rcd_length, ofb->max_rcd_length );
        }
#endif

#ifdef SRVOPT
        if( srvopt_fctrtn() )
            printf("RTN:open_source_file:TRUE\n" );
#endif
        return( TRUE );
}






Static
Char *read_source( Incl_t *mf, Int32 *read_cnt ) {
        _RIOFB_T *iofb;
        Char *rp;
        Int32 cnt;

#ifdef SRVOPT
        if( srvopt_function() )
            printf("FCT:read_source( \"%s\")\n", mf->name );
#endif
        iofb = _Rreadn( mf->f, ibuf1, mf->rec_len, __DFT );
        if( iofb->num_bytes == EOF ) {
            mf->eof = TRUE;
#ifdef SRVOPT
        if( srvopt_fctrtn() )
            printf("RTN:read_source:NULL\n");
#endif
            return( NULL );
        }
#ifdef SRVOPT
        if( srvopt_detail() )
            printf("DTL:read_source:"
                "iofb->num_bytes=%d left/right=%d/%d\n",
                iofb->num_bytes, mf->left_margin, mf->right_margin );
#endif
        if( iofb->num_bytes != mf->rec_len ) {
        }



        rp = ibuf1 +
             ((mf->rec_len > mf->right_margin) ? mf->right_margin :
              mf->rec_len );
        cnt = mf->rec_len - mf->left_margin;
        while( cnt && isspace( *rp ) ) {
            --rp;
            --cnt;
        }
        *(rp+1) = '\0';
        *read_cnt = cnt;

#ifdef SRVOPT
        if( srvopt_fctrtn() )
            printf("RTN:read_source:\"%s\"\n",ibuf1+mf->left_margin);
#endif

        return( ibuf1 + mf->left_margin );
}
# 699 "TMKPARSE.C"
Static
Void register_rules ( Void ) {
        Element_t *target;
        Element_t *depend;
        Char *sp;
        Char *tp;

#ifdef SRVOPT
        if( srvopt_function() )
            printf("FCT:register_rules(Void)\n");
        if( srvopt_detail() )
            printf("DTL:register_rules:cur_wk.t1=\"%s\"\n"
                   "    cur_wk.t2=\"%s\"\n"
                   "    cur_wk.cmd=\"%s\"\n"
                   "    cur_wk.op/line=%d/%d\n",
              cur_wrk.t1?cur_wrk.t1:"NULL",
              cur_wrk.t2?cur_wrk.t2:"NULL",
              cur_wrk.cmd?cur_wrk.cmd->cmd_txt:"NULL",
              cur_wrk.op,cur_wrk.line );
#endif
        if( cur_wrk.t1 ) {

            target = expand_elem_list( cur_wrk.t1, cur_wrk.line );

            depend = expand_elem_list( cur_wrk.t2, cur_wrk.line );

            while( target ) {
                Rules_t *rp;
                Element_t *tp;


                tp = expand_depend_dyn_name( target, &depend, cur_line );

                if( cur_wrk.op == OP_INFER_RULES ) {
                    update_inference_rules( cur_wrk.t1,
                            cur_wrk.cmd, cur_wrk.line );
                } else
                if( ( rp = in_rule_list( target->name ) ) == NULL ) {
                    rules_add_tail( target, tp );
                } else
                if( cur_wrk.op == OP_RULES && cur_wrk.cmd == NULL ) {
                    rules_add_dependents( rp, target, tp );
                } else
                if( cur_wrk.op == OP_RULES &&
                        rp->op == OP_RULES && rp->cmd == NULL ) {
                    rules_add_dependents( rp, target, tp );
                    rp->cmd = cur_wrk.cmd;
                } else
                if( cur_wrk.op == OP_MULTI_RULES &&
                        rp->op == OP_MULTI_RULES ) {
                    rules_add_tail( target, tp );
                }
                else {
                    log_error( MULT_RULE_SAME_TARGET, NULL, rp->line );
                    exit( TMK_EXIT_FAILURE );
                }

                tp = target;
                target = target->nxt;
                tp->nxt = NULL;
            }



            free( cur_wrk.t1 );
            cur_wrk.cmd = NULL;
            cur_wrk.t1 =
            cur_wrk.t2 = NULL;
        }
}






Static
Int32 append_rd_buf ( Int16 append_cnt, Char *from_buf,
                        Int16 *to_buf_sz, Char **to_buf ) {
        Char *lp;
        Int16 sz;
        Int32 total_sz;

#ifdef SRVOPT
        if( srvopt_function() )
            printf("FCT:append_rd_buf(%d,\"%s\",,)\n",append_cnt,
                    from_buf );
#endif
        sz = ( *to_buf == NULL ) ? 0 : strlen( *to_buf );
        total_sz = sz + append_cnt + 1;
        if( total_sz >= *to_buf_sz ) {
            lp = *to_buf;
            *to_buf_sz = (( total_sz / WRKBUF_SZ ) * WRKBUF_SZ ) +
                            WRKBUF_SZ;
            *to_buf = (Char *)alloc_buf( *to_buf_sz, "append_rd_buf()" );
            memcpy( *to_buf, lp, sz );
            if( lp != NULL ) {
                free( lp );
            }
        }
        memcpy( (*to_buf) + sz, from_buf, append_cnt );
        *((*to_buf) + sz + append_cnt ) = 0;
#ifdef SRVOPT
        if( srvopt_fctrtn() )
            printf("RTN:append_rd_buf:%d,%d,\"%s\"\n",sz+append_cnt,
                    *to_buf_sz,*to_buf );
#endif
        return( sz + append_cnt );
}






Static
Char *include_keyword_found ( Char *rtn_txt ) {
        Char *ep;

        if( *rtn_txt == '!' ) {
            rtn_txt = skip_white_spaces( rtn_txt + 1 );
        }
        ep = skip_alpha( rtn_txt );
        if( ( ( ep - rtn_txt ) == INCLUDE_SZ ) &&
            isspace( *ep ) &&
            strncmp( rtn_txt, INCLUDE_TXT, INCLUDE_SZ ) == 0 ) {
            ep = skip_white_spaces( ep );
        }
        else {
            ep = NULL;
        }
        return( ep );
}
# 850 "TMKPARSE.C"
Static
Char *read_next_line ( Int16 *line ) {
        Boolean cont_line;
        Boolean read_more;
        Int32 read_cnt;
        Int32 tot_cnt;
        Char *hp;
        Char *tp;
        Char *fn;
        Char *rtn_txt;

#ifdef SRVOPT
        if( srvopt_function() )
            printf("FCT:read_next_line(Int16 *line)\n");
#endif

        ibuf2[0] = 0;

try_again:
        read_more = TRUE;
        cont_line = FALSE;
        tot_cnt = 0;
        cur_line = rd_line + 1;



        while( read_more ) {
            read_more = FALSE;
            if( ( hp = read_source( mf, &read_cnt ) ) != NULL ) {
                ++rd_line;
                tp = hp + read_cnt - 1;


                if( read_cnt == 0 ) {
                    if( ibuf2[0] == 0 ) {



                        ++cur_line;
                        read_more = TRUE;
                        continue;
                    }
                    else {


                        break;
                    }
                }

                if( cont_line ) {

                     while( *hp && isspace( *hp ) ) {
                         --read_cnt;
                         ++hp;
                     }
                }



                if( *tp == '\\' || *tp == '+' ) {
                    cont_line =
                    read_more = TRUE;


                    *tp = ' ';


                    read_cnt = skip_trail_spaces( hp );


                    if( read_cnt == 0 )
                        continue;



                    tp = hp + read_cnt;
                    *tp++ = ' ';
                    *tp = 0;
                    ++read_cnt;
                }
                tot_cnt = append_rd_buf( read_cnt, hp,
                                         &ibuf2_sz, &ibuf2 );
            }
        }


        if( *ibuf2 == '#' ) {
            *ibuf2 = 0;
            rtn_txt = ibuf2;
        }
        else {

            tp = ibuf2 + tot_cnt;
            while( tot_cnt-- > 0 ) {
                if( *(--tp) == '#' ) {
                    *tp = 0;
                }
            }


            skip_trail_spaces( ibuf2 );


            rtn_txt = text_substitution( ibuf2, cur_line );





            if( cd->active &&
                ( fn = include_keyword_found( rtn_txt ) ) != NULL ) {

                Char *tp = fn;
                FILE *fp;

                if( ++cur_mf_lvl >= max_mf_lvl ) {

                    log_error( NEST_INCL_EXCEED_MAX, NULL, cur_line );
                    exit( TMK_EXIT_FAILURE );
                }

                tp = skip_non_white_spaces( tp );
                *tp = 0;






                if( ( tp = strchr( fn, '.' ) ) != NULL ) {
                    Char *slashp = strchr( fn, '/' );

                    *tp = 0;
                    reset_buf( &t1 );
                    if( slashp != NULL ) {
                        *slashp = 0;
                        sprintf( t1.bp, "%s/%s(%s)", fn,
                                 tp + 1, slashp + 1 );
                    }
                    else {
                        sprintf( t1.tp, "%s(%s)", tp + 1, fn );
                    }
                    fn = t1.bp;
                }

                if( opt_debug() ) {
                    sprintf( txtbuf, "Include file opened (%d) = \"%s\"",
                             cur_mf_lvl, fn );
                    log_dbg( txtbuf );
                }

                mf->line_no = rd_line;
                ++mf;
                mf->name = fn;

                if( open_source_file( mf ) ) {
                    rd_line = 0;
                    ibuf2[0] = 0;
                    goto try_again;
                } else {
                    log_error( CANT_OPEN_INCLUDE, fn, cur_line );
                    exit( TMK_EXIT_FAILURE );
                }
            }
        }


        if( *rtn_txt == 0 ) {
            if( ! mf->eof )
                goto try_again;

            if( cur_mf_lvl ) {
                _Rclose( mf->f );
                --mf;
                --cur_mf_lvl;
                rd_line = mf->line_no;
                ibuf2[0] = 0;
                goto try_again;
            }
        }

        *line = cur_line;

#ifdef SRVOPT
        if( srvopt_fctrtn() )
            printf("RTN:read_next_line:\"%s\"\n",rtn_txt);
#endif
        return( rtn_txt );
}






Static
Int16 look_for_directive ( Char *rd_ln, Int16 sz ) {
        Static struct dir_keyword {
            Char *txt;
            Int16 sz;
            Int16 rv;
        } dir_kw[] = { { "if", 2, CD_IF },
                       { "else", 4, CD_ELSE },
                       { "elif", 4, CD_ELIF },
                       { "endif", 5, CD_ENDIF },
                       { "ifdef", 5, CD_IFDEF },
                       { "ifndef", 6, CD_IFNDEF },
                       { "error", 5, CD_ERROR },
                       { "undef", 5, CD_UNDEF },
                       { NULL, 0, CD_UNKNOWN }
                     };
        struct dir_keyword *kp = dir_kw;

#ifdef SRVOPT
        if( srvopt_function() )
            printf("FCT:look_for_directive(\"%s\",%d)\n",rd_ln,sz);
#endif
        while( kp->txt ) {
            if( kp->sz == sz &&
                memcmp( kp->txt, rd_ln, sz ) == 0 ) {

                break;
            }
            ++kp;
        }
#ifdef SRVOPT
        if( srvopt_fctrtn() )
            printf("RTN:look_for_directive:%d\n",kp->rv );
#endif
        return( kp->rv );
}






Static
Int16 scan_directives ( Char **rd_ln, Int32 *exp_value, Int16 line ) {
        Char *tp = *rd_ln;
        Char *ep;
        Int16 rc = CD_NONE;

#ifdef SRVOPT
        if( srvopt_function() )
            printf("FCT:scan_directives(\"%s\",*exp_value,%d)\n",
                       *rd_ln,line);
#endif
        if( *tp == '!' ) {
            tp = skip_white_spaces( tp + 1 );
            ep = skip_alpha( tp );
            rc = look_for_directive( tp, ep - tp );
            tp = skip_white_spaces( ep );

            switch( rc ) {
            case CD_IF :
            case CD_ELIF :
                if( *tp == 0 ) {
                    log_error( NO_EXPR_IN_IF_ELIF, NULL, line );
                    exit( TMK_EXIT_FAILURE );
                }
                if( tp == ep ) {
                    log_error( EXPR_SYNTAX_ERROR, NULL, line );
                    exit( TMK_EXIT_FAILURE );
                }

                *exp_value = evaluate_exp( tp, line );

                break;
            case CD_IFDEF :
            case CD_IFNDEF :
                if( *tp == 0 ) {
                    log_error( NO_EXPR_IN_IF_ELIF, NULL, line );
                    exit( TMK_EXIT_FAILURE );
                }
                if( tp == ep ) {
                    log_error( EXPR_SYNTAX_ERROR, NULL, line );
                    exit( TMK_EXIT_FAILURE );
                }

                if( cd->active ) {
                    ep = skip_macro_sym( tp );
                    *ep = 0;
                    *exp_value = get_sym( tp ) != NULL;
                }
                else {
                    *exp_value = FALSE;
                }

                break;
            case CD_ERROR :
            case CD_UNDEF :
                *rd_ln = tp;
                break;
            case CD_ELSE :
            case CD_ENDIF :
            case CD_UNKNOWN :
                break;
            }
        }
#ifdef SRVOPT
        if( srvopt_fctrtn() )
            printf("RTN:scan_directives(,%d):%d\n", *exp_value, rc );
#endif
        return( rc );
}






Static
Void cd_push ( Int16 op, Int16 line, Int16 expected,
                  Boolean act, Boolean act_bef ) {
#ifdef SRVOPT
        if( srvopt_function() )
            printf("FCT:cd_push(%d,%d,%x,%d,%d):cur_lvl=%d\n",
            op,line,expected,act,act_bef,cur_cd_lvl+1);
#endif
        if( ++cur_cd_lvl >= max_cd_lvl ) {
            log_error( NEST_DIRECTIVE_EXCEED_MAX, NULL, MSG_NO_LINE_NO );
            exit( TMK_EXIT_FAILURE );
        }
        ++cd;
        cd->op = op;
        cd->line = line;
        cd->cd_expected = expected;
        cd->active = act;
        cd->ifelse_before = act_bef;
}






Static
Void cd_pop ( Void ) {
#ifdef SRVOPT
        if( srvopt_function() )
            printf("FCT:cd_pop\n");
#endif
        if( --cur_cd_lvl < 0 ) {
fprintf(stderr,"cd_pop underflow\n");
        }
        --cd;
#ifdef SRVOPT
        if( srvopt_detail() )
            printf("DTL:cd_pop,cur_cd_lvl(%d),%d/%d/%x/%d/%d\n",
               cur_cd_lvl,cd->op,cd->line,cd->cd_expected,
               cd->active,cd->ifelse_before);
#endif
}
# 1214 "TMKPARSE.C"
Static
Char *read_line ( Int16 *line ) {
        Char *rd_ln;
        Char *tp;
        Int16 cd_op;
        Int32 exp_value;
        Boolean prev_active;

#ifdef SRVOPT
        if( srvopt_function() )
            printf("FCT:read_line\n");
#endif
        while( TRUE ) {
            rd_ln = read_next_line( line );
            if( mf->eof ) {

                if( cur_cd_lvl != 0 ) {

                    log_error( UNEXPECT_EOF, rd_ln, MSG_NO_LINE_NO );
                    exit( TMK_EXIT_FAILURE );
                }
                break;
            }



            if( ( cd_op = scan_directives( &rd_ln, &exp_value, *line ) )
                        == CD_NONE ) {
                if( cd->active ) {

                    break;
                }
                else {

                    continue;
                }
            }

            switch( cd_op ) {
            case CD_ELIF :
                if( ( cd_op & cd->cd_expected ) == 0 ) {
                    log_error( MISPLACE_ELIF, NULL, *line );
                    exit( TMK_EXIT_FAILURE );
                }
                prev_active = cd->ifelse_before;
                cd_pop();
                cd_push( cd_op, *line,
                         CD_IF|CD_ELSE|CD_ELIF|CD_ENDIF|CD_IFDEF|
                         CD_IFNDEF|CD_ERROR|CD_UNDEF,
                         cd->active &&
                              ( prev_active ? FALSE : (exp_value != 0) ),
                         prev_active || exp_value != 0 );
                break;
            case CD_IF :
                cd_push( cd_op, *line,
                         CD_IF|CD_ELSE|CD_ELIF|CD_ENDIF|CD_IFDEF|
                         CD_IFNDEF|CD_ERROR|CD_UNDEF,
                         cd->active ? ( exp_value != 0 ) : FALSE,
                                                  exp_value != 0 );
                break;
            case CD_ELSE :
                if( ( cd_op & cd->cd_expected ) == 0 ) {
                    log_error( MISPLACE_ELSE, NULL, *line );
                    exit( TMK_EXIT_FAILURE );
                }
                prev_active = cd->ifelse_before;
                cd_pop();
                cd_push( cd_op, *line,
                         CD_IF|CD_ENDIF|CD_IFDEF|CD_IFNDEF|CD_ERROR|
                         CD_UNDEF, cd->active && !prev_active, TRUE );
                break;
            case CD_ENDIF :
                if( ( cd_op & cd->cd_expected ) == 0 ) {
                    log_error( MISPLACE_ENDIF, NULL, *line );
                    exit( TMK_EXIT_FAILURE );
                }
                cd_pop();
                break;
            case CD_IFNDEF :
                exp_value = ! exp_value;



            case CD_IFDEF :
                cd_push( cd_op, *line,
                         CD_IF|CD_ENDIF|CD_IFDEF|
                         CD_IFNDEF|CD_ERROR|CD_UNDEF,
                         exp_value ? cd->active : FALSE, FALSE );
                break;
            case CD_UNDEF :
                tp = skip_macro_sym( rd_ln );
                *tp = 0;
                if( ! delete_sym( rd_ln ) ) {
                    log_error( UNDEF_MACRO_NOT_FOUND, rd_ln, *line );
                }
                break;
            case CD_ERROR :
                if( get_usrmsg_to_session() ) {
                    fprintf( stdout, "Error directive: %s\n", rd_ln );
                } else {
                    log_error( ERROR_DIRECTIVE, rd_ln, MSG_NO_LINE_NO );
                }
                exit( TMK_EXIT_FAILURE );
            case CD_UNKNOWN :
                log_error( UNKNOWN_DIRECTIVE, rd_ln, *line );
                exit( TMK_EXIT_FAILURE );
            }
        }
#ifdef SRVOPT
        if( srvopt_fctrtn() )
            printf("RTN:read_line:%s\n",rd_ln);
#endif
        return( rd_ln );
}






Static
Int16 check_keyword ( Char *txt, Char **rtn_txt ) {
        Char *tp;
        struct keyword *kp = kw;
        Int16 cnt = KW_TBL_SZ;

#ifdef SRVOPT
        if( srvopt_function() )
            printf("FCT:check_keyword(\"%s\", Char **rtn_txt )\n",txt);
#endif
        if( *txt != '.' ) {
            return( OP_NO_KEYWORD );
        }

        while( cnt-- ) {
            Int16 txt_sz = strlen( kp->match_txt );

            if( strncmp( txt, kp->match_txt, txt_sz ) == 0 ) {
                if( kp->look_char == 0 ) {
                    return( kp->op );
                }

                if( ( tp = look_for( txt + txt_sz, kp->look_char ) )
                        == NULL ) {
                    log_error( INV_KEYWORD_FORMAT, kp->match_txt,
                               cur_line );
                    exit( TMK_EXIT_FAILURE );
                }
                *tp++ = 0;
                skip_trail_spaces( txt );
                *rtn_txt= skip_white_spaces( tp );

                return( kp->op );
            }

            ++kp;
        }
        return( OP_NO_KEYWORD );
}






Int16 parse_rules ( Char *txt, Char **rtn_txt ) {
        Char *tp;
        Char *up;
        Int16 op;
        Int16 rc;

#ifdef SRVOPT
        if( srvopt_function() )
            printf("FCT:parse_rules(\"%s\", Char **rtn_txt )\n",txt);
#endif

        if( isspace( *txt ) ) {

            *rtn_txt= skip_white_spaces( txt );
            return( OP_COMMAND );
        }


        if( ( rc = check_keyword ( txt, rtn_txt ) ) > 0 ) {
            return( rc );
        }



        tp = skip_macro_sym( txt );
        if( *tp == 0 ) {
            return( OP_ERROR );
        }
        up = skip_white_spaces( tp );
        if( *up == '=' ) {

            *tp = 0;
            tp = skip_white_spaces( ++up );
            *rtn_txt= tp;
            return( OP_MACRO );
        }


        up = txt;
        while( TRUE ) {


            Char *op = up;

            tp = skip_obj_name( up );
            up = skip_white_spaces( tp );
            if( *up == 0 || tp == op ) {
                log_error( ( *up == 0 ) ? INV_RULE_COLON_MISSING
                                        : INV_RULE_NO_TARGET,
                            NULL, cur_line );
                return( OP_ERROR );
            }
            if( *up == ':' ) {
                *up++ = 0;
                break;
            }
        }
        op = OP_RULES;
        if( *up == ':' ) {
            op = OP_MULTI_RULES;
            ++up;
        }
        *rtn_txt = skip_white_spaces( up );


        if( op == OP_RULES && *txt == '.' ) {
            op = OP_INFER_RULES;
        }

        return( op );
}






Void setup_parser ( Void ) {
#ifdef SRVOPT
        if( srvopt_function() )
            printf("FCT:setup_parser(Void)\n");
#endif

        ibuf2 = (Char *)alloc_buf( WRKBUF_SZ, "setup_parser()" );
        ibuf2_sz= WRKBUF_SZ;
        ibuf2[0]= 0;

        ibuf1 = (Char *)alloc_buf( ibuf1_sz, "setup_parser()" );

        mf_base = (Incl_t *)alloc_buf( sizeof(Incl_t) * (max_mf_lvl + 1),
                          "setup_parser()" );

        cd_base = (Cd_t *)alloc_buf( sizeof(Cd_t) * (max_cd_lvl + 1),
                          "setup_parser()" );
}






Void setup_parser_structures ( Void ) {
        Rules_t *rp = head;
        Rules_t *nrp;
        Cmd_t *cp;
        Cmd_t *ncp;

#ifdef SRVOPT
        if( srvopt_function() )
            printf("FCT:setup_parser_structures(Void)\n");
#endif

        while( rp ) {

            free_element( rp->target );
            free_element( rp->dependent );


            free_cmd( rp->cmd );

            nrp = rp->nxt;
            if ( !(rp->implicit_rule))
                free( rp );
            rp = nrp;
        }
        head = NULL;


        mf = mf_base;
        cur_mf_lvl = 0;

        cd = cd_base;
        cur_cd_lvl = 0;
        cd->op = 0;
        cd->line = -1;
        cd->cd_expected = CD_IF|CD_IFDEF|CD_IFNDEF|CD_ERROR|CD_UNDEF;
        cd->active = TRUE;
        cd->ifelse_before = FALSE;
}






Int16 parse_makefile ( Char *makef, Boolean parse_makef ) {
        Char *lp;
        Char *sp;
        Char *tp;
        Int16 line;
        Int16 len;
        Int16 op;
        File_spec_t fs;

#ifdef SRVOPT
        if( srvopt_function() )
            printf("FCT:parse_makefile(\"%s\",%d)\n",makef, parse_makef);
#endif
        set_parse_state();

        cur_line = 0;
        rd_line = 0;


        mf->name = makef;
        if( ! open_source_file( mf ) ) {
            if( parse_makef ) {
                log_error( CANT_OPEN_MAKEFILE, makef, MSG_NO_LINE_NO );
                exit( TMK_EXIT_FAILURE );
            }
            else {

                return( FALSE );
            }
        }

        register_rules();

        while( lp = read_line( &line ), strlen( lp ) ) {

            if( opt_debug() ) {
                sprintf( txtbuf, "DBG:line=%3d sz=%2.2d:\"%s\"",
                                line, strlen(lp), lp );
                log_dbg( txtbuf );
            }


            op = parse_rules( lp, &sp );

            if( opt_debug() ) {
                sprintf( txtbuf, "DBG:op:%d op1=%d\"%s\" op2=%d\"%s\"",
                                op, strlen(lp), lp, strlen(sp), sp );
                log_dbg( txtbuf );
            }

            switch( op ) {
            case OP_MACRO :
                update_sym( lp, sp, FALSE );
                break;
            case OP_RULES :
            case OP_MULTI_RULES :


                register_rules();

                cur_wrk.t1 = (Char *)alloc_buf( ( len = strlen( lp ) ) +
                                strlen( sp ) + 2, "parse_makefile()" );
                strcpy( cur_wrk.t1, lp );
                cur_wrk.t2 = cur_wrk.t1 + len + 1;
                strcpy( cur_wrk.t2, sp );
                cur_wrk.cmd = NULL;
                cur_wrk.op = op;
                cur_wrk.line = line;

                break;
            case OP_COMMAND :
                if( cur_wrk.t1 == NULL ) {
                    log_error( INV_RULE_CMD_ONLY, NULL, line );
                    exit( TMK_EXIT_FAILURE );
                }
                cmd_add_tail( &cur_wrk.cmd, sp, line );
                break;
            case OP_IGNORE :
                opt_set_ingore();
                break;
            case OP_SILENT :
                opt_set_silent();
                break;
            case OP_SUFFIXES :
                register_rules();
                update_suffix( sp, line );
                break;
            case OP_PRECIOUS :
                register_rules();
                break;
            case OP_INFER_RULES :
                register_rules();

                if( strlen( sp ) ) {
                    log_error( INV_INF_RULE, NULL, line );
                    exit( TMK_EXIT_FAILURE );
                }

                cur_wrk.t1 = (Char *)alloc_buf( strlen( lp ) + 1,
                        "parse_makefile()" );
                strcpy( cur_wrk.t1, lp );
                cur_wrk.t2 = NULL;
                cur_wrk.cmd = NULL;
                cur_wrk.op = op;
                cur_wrk.line = line;

                break;
            default :
                log_error( INV_RULE, NULL, line );
                exit( TMK_EXIT_FAILURE );
            }
        }
        register_rules();
        return( TRUE );
}
# 1648 "TMKPARSE.C"
Rules_t *get_first_rule ( void ) {
        next_rule = head;

#ifdef SRVOPT
        if( srvopt_function() )
            printf("FCT:get_first_rule(Void)\n");
#endif
        while( next_rule && (next_rule->op != OP_RULES &&
                             next_rule->op != OP_MULTI_RULES ) )
            next_rule = next_rule->nxt;

#ifdef SRVOPT
        if( srvopt_fctrtn() )
            printf("RTN:get_first_rule:%s\n",srv_rule(next_rule));
#endif
        return( next_rule );
}
# 1674 "TMKPARSE.C"
Rules_t *get_next_rule ( void ) {
#ifdef SRVOPT
        if( srvopt_function() )
            printf("FCT:get_next_rule(Void)\n");
#endif
        if( next_rule == NULL )
            return( NULL );

        do {
            next_rule = next_rule->nxt;
        } while( next_rule && ( next_rule->op != OP_RULES &&
                              next_rule->op != OP_MULTI_RULES ) );
#ifdef SRVOPT
        if( srvopt_fctrtn() )
            printf("RTN:get_next_rule:%s\n",srv_rule(next_rule));
#endif
        return( next_rule );
}






Rules_t *get_next_applied_target ( void ) {
        Rules_t *rp = NULL;
        Char *tp;

#ifdef SRVOPT
        if( srvopt_function() )
            printf("FCT:get_next_applied_target(Void)\n");
#endif
        while( ( tp = get_next_requested_target() ) != NULL ) {
            if( ( rp = in_rule_list( tp ) ) != NULL )
                break;
            log_error( TARGET_NOT_FOUND, tp, MSG_NO_LINE_NO );
        }
#ifdef SRVOPT
        if( srvopt_fctrtn() )
            printf("RTN:get_next_applied_target:%s\n",srv_rule(rp));
#endif
        return( rp );
}






Rules_t *get_first_applied_target ( void ) {
        Rules_t *rp;
        Char *tp;

#ifdef SRVOPT
        if( srvopt_function() )
            printf("FCT:get_first_applied_target(Void)\n");
#endif
        if( ( tp = get_first_requested_target() ) == NULL ) {


            rp = head;
            while( rp &&
                  ( rp->op != OP_RULES && rp->op != OP_MULTI_RULES ) ){
                rp = rp->nxt;
            }
        } else {

            if( ( rp = in_rule_list( tp ) ) == NULL ) {
                log_error( TARGET_NOT_FOUND, tp, MSG_NO_LINE_NO );
                rp = get_next_applied_target();
            }
        }
#ifdef SRVOPT
        if( srvopt_fctrtn() )
            printf("RTN:get_first_applied_target:%s\n",srv_rule(rp));
#endif
        return( rp );
}






Static
Void dump_element( Element_t *e ) {
#ifdef SRVOPT
        if( srvopt_function() )
            printf("FCT:dump_element(Element_t *e)\n");
#endif
        while( e ) {
            sprintf( txtbuf, "      \"%s\"", e->name );
            log_dbg( txtbuf );
            e = e->nxt;
        }
}






Void free_element( Element_t *ep ) {
        Element_t *nep;

#ifdef SRVOPT
        if( srvopt_function() )
            printf("FCT:free_element(Element_t *e)\n");
#endif
        while( ep ) {
            nep = ep->nxt;
            free( ep );
            ep = nep;
        }
}






Void dump_rule_list( void ) {
        Int16 cnt = 0;
        Cmd_t *cp;
        Rules_t *rp = head;

        if( ! opt_debug() )
                return;

        log_dbg( "*****  Rules definitions  ************"
                 "***********************************");
        while( rp ) {
            Char *op;
            switch( rp->op ) {
            case OP_RULES :
                    op = "Dependency rule "; break;
            case OP_MULTI_RULES :
                    op = "Multiple rule **"; break;
            case OP_INFER_RULES :
            case OP_MACRO :
            case OP_COMMAND :
            case OP_SUFFIXES :
            case OP_PRECIOUS :
            default :
                    op = "Unknown rule ***"; break;
            }

            sprintf( txtbuf, "** rule %3d / line %3d / %s**********"
                     "**********************", ++cnt, rp->line, op );
            log_dbg( txtbuf );
            log_dbg("Target:"); dump_element( rp->target );
            log_dbg("dependent:"); dump_element( rp->dependent );
            log_dbg("commands:");
            cp = rp->cmd;
            while( cp ) {
                sprintf( txtbuf, "      line %d:\"%s\"",
                         cp->line, cp->cmd_txt);
                log_dbg( txtbuf );
                cp = cp->nxt;
            }

            rp = rp->nxt;
        }
        log_dbg("****************************************"
                "*********************************");
}
