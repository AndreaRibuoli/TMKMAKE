# 16 "TMKMAKER.C"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <signal.h>
#include <ctype.h>
#ifdef __ILEC400__
  #include <milib.h>
#endif

#include "tmkbase.h"
#include "tmkmaker.h"
#include "tmkutil.h"
#include "tmkfile.h"
#include "tmkparse.h"
#include "tmkdict.h"
#include "tmkbuilt.h"
#include "tmkoptio.h"
#include "tmkopna.h"
#include "tmkmsghd.h"



#ifdef __ILEC400__
  #define __rtvlurc() (_LU_WORK_AREA->LU_RC)
#else
  short __rtvlurc( void );
#endif





Static Boolean cmd_is_make;





Static
void set_proc_date ( File_spec_t *tfs, File_spec_t *dfs )
{
    if ( cmp_date ( tfs->proc_update, dfs->last_update ) < 0 ) {
         tfs->proc_update.val = dfs->last_update.val;
    }

    if ( cmp_date ( tfs->proc_update, dfs->proc_update ) < 0 ) {
         tfs->proc_update.val = dfs->proc_update.val;
    }
}
# 75 "TMKMAKER.C"
Static
Boolean target_older ( Element_t *t, Element_t *d, Int16 line,
                       Date_t last_update ) {
        Element_t *ep;
        Boolean rc = FALSE;

#ifdef SRVOPT
        if( srvopt_function() ) {
            sprintf(srv_cat,"FCT:target_older(%s),",srv_e(t));
            printf("%s%s)\n",srv_cat,srv_e(d));
        }
#endif


        if( t->fs.last_update.val == 0 ) {
#ifdef SRVOPT
            if( srvopt_fctrtn() )
                printf("RTN:target_older:1\n");
#endif
            return( TRUE );
        }



        if( d == NULL ) {
#ifdef SRVOPT
            if( srvopt_fctrtn() )
                printf("RTN:target_older:0\n");
#endif
            return( FALSE );
        }


        while( d ) {
            update_file_date( &d->fs, line );
            ep = d;
            d = d->nxt;

            if (( cmp_date( (last_update.val == 0 ? t->fs.last_update :
                           last_update), ep->fs.last_update ) < 0 ) ||
               ( cmp_date( (last_update.val == 0 ? t->fs.last_update :
                           last_update), ep->fs.proc_update ) < 0 )) {
                set_proc_date ( &t->fs, &ep->fs );
                rc = TRUE;
            }



            if( strcmp( t->fs.type, FS_T_PGMSET ) == 0 &&
                strcmp( ep->fs.type, FS_T_PGM ) == 0 &&
                strcmp( t->fs.lib, ep->fs.lib ) == 0 &&
                strcmp( t->fs.file, ep->fs.file ) == 0 ) {

                if( cmp_date( (last_update.val == 0 ? t->fs.last_update :
                               last_update), t->fs.create_date )
                             == 0 ) {
                    rc = TRUE;
                }
            }
        }
#if 0

        while( d ) {
            update_file_date( &d->fs, line );
            d = d->nxt;
        }
#endif
#ifdef SRVOPT
        if( srvopt_fctrtn() )
            printf("RTN:target_older:%d\n",rc);
#endif
        return( rc );
}






Static
Boolean make_in_command ( Char *cmd ) {
        Char *cp;
        Char *s;
        Char save;
        Boolean rc;

#ifdef SRVOPT
        if( srvopt_function() )
            printf("FCT:make_in_command(\"%s\")\n",cmd);
#endif

        cp = skip_non_white_spaces( cmd );
        save = *cp;
        *cp = 0;

        s = strchr( cmd, '/' );
        rc = (strcmp((s == NULL) ? cmd : s+1, "TMKMAKE") == 0);

        *cp = save;

#ifdef SRVOPT
        if( srvopt_fctrtn() )
            printf("RTN:make_in_command:%d\n",rc);
#endif
        return( rc );
}






Static
Char *create_nested_make_cmd ( Char *cmd, Buf_t *b ) {
        Char *tp;
        Char save;

#ifdef SRVOPT
        if( srvopt_function() )
            printf("FCT:create_nested_make_cmd(\"%s\",Buf_t *b)\n",cmd);
#endif
        reset_buf( b );



        if( ( tp = strstr_no_case( cmd, "OPTION(" ) ) == NULL ) {


            append_buf( b, cmd );
            append_buf( b, " OPTION(*NOEXEC)" );
        } else {


            save = tp[7];
            tp[7] = 0;
            append_buf( b, cmd );
            append_buf( b, "*NOEXEC " );
            tp[7] = save;
            append_buf( b, tp + 7 );
        }
#ifdef SRVOPT
        if( srvopt_fctrtn() )
            printf("RTN:create_nested_make_cmd:\"%s\"\n",b->bp);
#endif
        return( b->bp );
}






Static short system_cmd_sev;

Void system_cmd_trap ( int sig ) {
#ifdef __ILEC400__
        _INTRPT_Hndlr_Parms_T excinfo;
        error_rtn errcode;
#else
        sigdata_t *data;
        sigact_t *act;
#endif


#ifdef __ILEC400__
        _GetExcData (&excinfo);
#else
        data = sigdata();
#endif

#ifdef __ILEC400__
        system_cmd_sev = excinfo.Severity;
#else
        system_cmd_sev = data->exmsg->exmsgsev;
#endif
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
        act->xrtntosgnler=
        act->xremovemsg = 0;
#endif
}
# 289 "TMKMAKER.C"
Static
Void convert_to_pack ( Char *buf, Int16 rm, Int16 num ) {
        Char pc;

        while( num-- ) {
            pc = rm % 10;
            rm /= 10;
            pc |= ( rm % 10 ) * 16;
            rm /= 10;
            buf[num] = pc;
        }
}






#pragma linkage(QCMDEXC,OS)
void QCMDEXC( Char *, void * );

Static
Int16 exec_command ( Char *cmd, Int16 line ) {
        Int16 rc = 0;
        Int16 rm;
        static _Packed struct qcmdexc {
                Char integral[5];
                Char zeros[2];
                Char sign;
        } qcmd = { "\0\0\0\0\0", "\0\0", 0x0f };
        Void (*old_signal_fct)( int );

#ifdef SRVOPT
        if( srvopt_function() )
            printf("FCT:exec_command(\"%s\")\n",cmd);
#endif
        if( !opt_no_execute() ) {


            rm = strlen( cmd );
            if( rm > QCMDEXC_MAX_SZ ) {
                log_error( CMD_EXCEEDED_3000, NULL, line );
                exit( TMK_EXIT_FAILURE );
            }

            convert_to_pack( (char *)&qcmd, rm, 5 );
            system_cmd_sev = 0;
            old_signal_fct = signal( SIGALL, &system_cmd_trap );
            QCMDEXC( cmd, (void *)&qcmd );
            signal( SIGALL, old_signal_fct );

            rm = opt_get_rtncde_methods();
            if( rm != RTN_LURC ) {

                rc = system_cmd_sev;
            }
            if( system_cmd_sev == 0 && rm != RTN_EXCEPTION ) {

                rc = __rtvlurc();
            }
        }
        else {

            if( cmd_is_make ) {

                cmd = create_nested_make_cmd( cmd, &t1 );

                rm = strlen( cmd );
                if( rm > QCMDEXC_MAX_SZ ) {
                    log_error( CMD_EXCEEDED_3000, NULL, line );
                    exit( TMK_EXIT_FAILURE );
                }

                convert_to_pack( (char *)&qcmd, rm, 5 );
                system_cmd_sev = 0;
                old_signal_fct = signal( SIGALL, &system_cmd_trap );
                QCMDEXC( cmd, (void *)&qcmd );
                signal( SIGALL, old_signal_fct );

                if( system_cmd_sev == 0 )
                    rc == __rtvlurc();
            }
        }
#ifdef SRVOPT
        if( srvopt_fctrtn() )
            printf("RTN:exec_command:%d\n",rc);
#endif
        return( rc );
}
# 391 "TMKMAKER.C"
Static
Void set_pre_defined_macros ( Rules_t *rp ) {
        Element_t *ep;
        Date_t target_date;
        Char *cp;
        Boolean is_file = rp->target->fs.is_file;

#ifdef SRVOPT
        if( srvopt_function() )
            printf("FCT:set_pre_defined_macro(%s)\n",srv_rule(rp));
#endif

        update_sym( "@", rp->target->name, FALSE );
        update_sym( "@L", rp->target->fs.lib, FALSE );
        update_sym( "@F", rp->target->fs.file, FALSE );
        update_sym( "@M", rp->target->fs.extmbr, FALSE );
        update_sym( "@T", rp->target->fs.type, FALSE );
        update_sym( "@S", rp->target->fs.seu_type, FALSE );


        update_sym( "%",
              ( ( strcmp( rp->target->fs.type, FS_T_LIBFILE ) == 0 ||
                  strcmp( rp->target->fs.type, FS_T_TXTLIB ) == 0 ||
                  strcmp( rp->target->fs.type, FS_T_BNDDIR ) == 0 ) &&
                  *rp->target->fs.extmbr != 0 )
              ? rp->target->fs.extmbr : "",
              FALSE );



        ep = rp->dependent;
        target_date = rp->target->fs.last_update;
        reset_buf( &t1 );
        reset_buf( &t2 );


        while( ep ) {
            Char *np;

            append_buf( &t2, ep->name );
            append_buf( &t2, " " );

            if( cmp_date( target_date, ep->fs.last_update ) < 0 ) {
                append_buf( &t1, ep->name );
                append_buf( &t1, " " );
            }
            ep = ep->nxt;
        }

        if( t1.bp != t1.tp ) { --t1.tp; *t1.tp = 0; }
        if( t2.bp != t2.tp ) { --t2.tp; *t2.tp = 0; }

        update_sym( "?", t1.bp, FALSE );
        update_sym( "**", t2.bp, FALSE );


        if( rp->implicit_rule ) {

            reset_buf( &t1 );
            append_buf( &t1, cp = rp->target->fs.lib );
            append_buf( &t1, *cp ? "/" : "" );
            append_buf( &t1, is_file ? rp->target->fs.extmbr
                                     : rp->target->fs.file );
            update_sym( "*", t1.bp, FALSE );

            update_sym("*L", rp->target->fs.lib, FALSE );
            update_sym("*F", rp->target->fs.file, FALSE );
            update_sym("*M", rp->target->fs.extmbr, FALSE );
            update_sym("*T", rp->target->fs.type, FALSE );
            update_sym("*S", rp->target->fs.seu_type, FALSE );


            if( rp->dependent ) {
                update_sym("<", rp->dependent->name, FALSE );
                update_sym("<L", rp->dependent->fs.lib, FALSE );
                update_sym("<F", rp->dependent->fs.file, FALSE );
                update_sym("<M", rp->dependent->fs.extmbr, FALSE );
                update_sym("<T", rp->dependent->fs.type, FALSE );
                update_sym("<S", rp->dependent->fs.seu_type, FALSE );
            } else {
                update_sym("<", "", FALSE );
                update_sym("<L", "", FALSE );
                update_sym("<F", "", FALSE );
                update_sym("<M", "", FALSE );
                update_sym("<T", "", FALSE );
                update_sym("<S", "", FALSE );
            }
        }
        else {

            update_sym("*", "", FALSE );
            update_sym("*L","", FALSE );
            update_sym("*F","", FALSE );
            update_sym("*M","", FALSE );
            update_sym("*T","", FALSE );
            update_sym("*S","", FALSE );

            update_sym("<", "", FALSE );
            update_sym("<L","", FALSE );
            update_sym("<F","", FALSE );
            update_sym("<M","", FALSE );
            update_sym("<T","", FALSE );
            update_sym("<S","", FALSE );
        }
}






Static
Int32 get_cmd_sev( Char **cmd ) {
        Int32 rc = 0;
        Char *cp = *cmd;

#ifdef SRVOPT
        if( srvopt_function() )
            printf("FCT:get_cmd_sev(%s)\n",*cmd);
#endif

        while( isdigit( *cp ) ) {
            rc *= 10;
            rc += *cp - '0';
            ++cp;
        }
        *cmd = cp;


        rc = isspace( *cp ) ? rc : -1;

#ifdef SRVOPT
        if( srvopt_fctrtn() )
            printf("RTN:get_cmd_sev:%d\n",rc);
#endif
        return( rc );
}






Static
Boolean make_target ( Rules_t *rp ) {
        Element_t *ep;
        Rules_t *drp;
        Boolean rc = FALSE;
        Boolean target_origin_date_set = FALSE;
        Date_t rp_last_update;

#ifdef SRVOPT
        if( srvopt_function() )
            printf("FCT:make_target(%s)\n",srv_rule(rp));
#endif
        if( rp->recursive ) {

            log_error( RECURSIVE_TARGET, rp->target->name, rp->line );
            return( rc );
        }

        rp_last_update.val = 0;


        update_file_date( &rp->target->fs, rp->line );



        while( rp != NULL ) {
            rp->recursive = TRUE;



            ep = rp->dependent;
            while( ep ) {
                ep->maked = FALSE;
                if( ( drp = in_rule_list( ep->name ) ) != NULL ) {


                    ep->maked |= make_target( drp );
                    rp->dependent->fs.proc_update.val =
                    drp->target->fs.proc_update.val;
                }


                if( !ep->maked && !obj_exists( &ep->fs, rp->line ) ) {
                    log_error( CANT_PROC_TARGET, ep->name, rp->line );
                    exit( TMK_EXIT_FAILURE );
                }
                ep = ep->nxt;
            }


            if( opt_touch_target() ) {
                touch_target( rp->target );
            }
            else {
                if( target_older( rp->target, rp->dependent,
                                  rp->line, rp_last_update ) ) {



                    Cmd_t *cur_cmd = rp->cmd;

                    set_pre_defined_macros( rp );
                    if( opt_debug() ) {
                        dump_dict();
                    }


                    while( cur_cmd != NULL ) {
                        Boolean no_echo = FALSE;
                        Boolean no_err_stop = FALSE;
                        Char echo_str[5];
                        Char *cmd = cur_cmd->cmd_txt;
                        Int32 cmd_sev = opt_get_rtncde_sev();

                        while( *cmd == '@' || *cmd == '-' ) {
                            switch( *cmd ) {
                            case '@':
                                no_echo = TRUE;
                                break;
                            case '-':
                                no_err_stop = TRUE;

                                if( isdigit( cmd[1] ) ) {
                                    ++cmd;
                                    cmd_sev = get_cmd_sev( &cmd );
                                    if( cmd_sev < 0 ) {


                                        log_error( INV_PREFIX_RTNSEV,
                                                 NULL, cur_cmd->line );
                                        exit( TMK_EXIT_FAILURE );
                                    }
                                } else cmd_sev =
                                       ~(1<<((sizeof(cmd_sev)*8)-1));
                                break;
                            }
                            ++cmd;
                        }
                        cmd = skip_white_spaces( cmd );

                        cmd_is_make = ( opt_no_execute()
                                    ? make_in_command ( cmd ) : FALSE );



                        cmd = bi_macro_substitution ( cmd );


                        if( ( !no_echo && !opt_silent_mode() ) ||
                            opt_no_execute() ) {
                            log_usrmsg( cmd );
                        }


                        strncpy( echo_str, cmd, 4 );
                        echo_str[4] = 0;
                        to_upper_str( echo_str );
                        if( strncmp( echo_str, "ECHO", 4 ) == 0 ) {

                            if ( !opt_no_execute() ) {
                                cmd = skip_white_spaces( cmd + 4 );
                                log_usrmsg( cmd );
                            }
                        }
                        else {
                            Int16 rc;


                            rc = exec_command( cmd, cur_cmd->line );
                            if( rc > cmd_sev ) {


                                log_error( CMD_FAILED, NULL,
                                                  cur_cmd->line );
                                if( !no_err_stop &&
                                    !opt_ignore_err_code() )
                                    exit( TMK_EXIT_FAILURE );
                            }
                        }
                        cur_cmd = cur_cmd->nxt;
                    }
                    rc = TRUE;
                }
            }

            rp->recursive = FALSE;



            if( rp->op == OP_MULTI_RULES ) {
                Rules_t *crp;

                if ( !target_origin_date_set ) {
                     target_origin_date_set = TRUE;
                     rp_last_update.val = rp->target->fs.last_update.val;
                }


                crp = rp;
                rp = crp->nxt;
                while( rp ) {
                    if( strcmp( rp->target->name,
                                crp->target->name ) == 0 ) {
                        break;
                    }
                    rp = rp->nxt;
                }
            }
            else {
                rp = NULL;
            }
        }
        return( rc );
}






Void process_makefile ( Void ) {
        Rules_t *make_rule;
        Void (*old_signal_fct)( int );

#ifdef SRVOPT
        if( srvopt_function() )
            printf("FCT:process_makefile(Void)\n");
#endif
        set_make_state();
        dump_builtins();
        dump_dict();
        dump_rule_list();


        make_rule = get_first_applied_target();

        while( make_rule ) {

            make_target( make_rule );

            log_error( TARGET_UP_TO_DATE, make_rule->target->name,
                                          make_rule->line );

            make_rule = get_next_applied_target();
        }
}
