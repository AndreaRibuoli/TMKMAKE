# 19 "TMKFILE.C"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <signal.h>
#include <recio.h>

#include "tmkopna.h"
#include "tmkbase.h"
#include "tmkutil.h"
#include "tmkfile.h"
#include "tmkparse.h"
#include "tmkdict.h"
#include "tmkmaker.h"
#include "tmkoptio.h"
#include "tmkmsghd.h"
# 46 "TMKFILE.C"
Static
Char *valid_name ( Char *txt, Int16 *sz ) {
        Char *tp = txt;
        Char *rp;
        Int16 len = 0;

#ifdef SRVOPT
        if( srvopt_function() )
            printf("FCT:valid_name(\"%s\",Int16 *sz)\n", txt );
#endif


        if( *tp == 0 ¦¦ !( isalpha( *tp ) ¦¦ look_for( "$@*", *tp ) ) ) {
            rp = NULL;
        }
        else {
            ++tp;



            while( *tp ) {
                if( !( isalnum( *tp ) ¦¦ look_for( "$@_", *tp ) ) ) {
                    Char *mp = has_dyn_macro( tp - 1 );



                    if( mp == NULL ¦¦ mp != ( tp - 1 ) ) {
                        break;
                    }

                    tp += ( *tp == '@' ) ? 0 : 3;
                }
                ++tp;
            }

            len = tp - txt;
            rp = ( len == 0 ¦¦ len > 10 ) ? NULL : tp;
        }
        *sz = len;

#ifdef SRVOPT
        if( srvopt_fctrtn() )
            printf("RTN:valid_name:\"%s\",%d\n", rp?rp:"NULL", len );
#endif
        return( rp );
}






Static
Char *namncpy ( Char *d, Char *s, Int16 sz ) {
#ifdef SRVOPT
        if( srvopt_function() )
            printf("FCT:namncpy(Char *d,\"%s\",%d)\n", s,sz );
#endif
        if( sz <= FILE_NM_SZ ) {
            strncpy( d, s, sz );
            d[ sz ] = 0;
        }
        else {
            *d = 0;
        }
#ifdef SRVOPT
        if( srvopt_fctrtn() )
            printf("RTN:namncpy:\"%s\"\n",d);
#endif
        return( d );
}
# 171 "TMKFILE.C"
Char *parse_obj_name ( Char *f, File_spec_t *fs,
                          Int16 *len, Int16 line ) {
        Char *orgp = f;
        Char *tp;
        Char *rp;
        Int16 sz;
        Int16 type_id;

#define OBJ_DB 0
#define OBJ_LIBFILE 1
#define OBJ_PGM 2
#define OBJ_INVALID 3

#ifdef SRVOPT
        if( srvopt_function() )
            printf("FCT:parse_obj_name(\"%s\",File_spec_t *fs,"
                   "Int16 *len,%d)\n",f,line );
#endif
        fs->lib[0] =
        fs->file[0] =
        fs->extmbr[0] =
        fs->type[0] =
        fs->seu_type[0] = 0;
        fs->obj_type = '*';
        fs->is_file = FALSE;

        if( ( tp = valid_name( f, &sz ) ) == NULL ) {

            return( NULL );
        }


        if( *tp == '/' ) {
            namncpy( fs->lib, f, sz );
            f = tp + 1;
            if( ( tp = valid_name( f, &sz ) ) == NULL ) {
                log_error( INV_OBJ_SPEC, NULL, line );
                exit( TMK_EXIT_FAILURE );
            }
        }

        switch( *tp ) {
        case '.' :

            namncpy( fs->extmbr, f, sz );
            f = tp + 1;
            if( ( tp = valid_name( f, &sz ) ) == NULL ) {
                log_error( INV_OBJ_SPEC, NULL, line );
                exit( TMK_EXIT_FAILURE );
            }
            namncpy( fs->file, f, sz );
            strcpy( fs->type, FS_T_FILE );
            fs->is_file = TRUE;
            type_id = OBJ_DB;
            break;

        case '(' :

            namncpy( fs->file, f, sz );
            f = tp + 1;
            if( ( tp = valid_name( f, &sz ) ) == NULL ¦¦
                *tp != ')' ) {

                log_error( INV_OBJ_SPEC, NULL, line );
                exit( TMK_EXIT_FAILURE );
            }
            namncpy( fs->extmbr, f, sz );
            strcpy( fs->type, FS_T_LIBFILE );
            type_id = OBJ_LIBFILE;
            ++tp;
            break;
        default :
            namncpy( fs->file, f, sz );
            strcpy( fs->type, FS_T_PGM );
            type_id = OBJ_PGM;
        }


        if( *tp == '<' ) {
            f = ++tp;
            if( ( tp = valid_name( f, &sz ) ) == NULL ¦¦

                ( *f == '*' ) ) {

                log_error( INV_OBJ_SPEC, NULL, line );
                exit( TMK_EXIT_FAILURE );
            }


            if( memcmp( f, FS_T_FILE, sizeof( FS_T_FILE ) - 1 ) == 0 ) {
                if( *tp != ',' && *tp != '>' ) {

                    type_id = OBJ_INVALID;
                }
                switch( type_id ) {
                case OBJ_PGM :

                    strcpy( fs->type, FS_T_FILE );
                    fs->is_file = TRUE;

                    *fs->extmbr = 0;
                    break;

                case OBJ_DB :
                    if( *tp == ',' ) {

                        f = tp + 1;
                        tp = valid_name( f, &sz );
                        if( tp == NULL ¦¦ sz > SEU_TYP_SZ ¦¦
                              *tp != '>' ) {
                            log_error( INV_OBJ_SPEC, NULL, line );
                            exit( TMK_EXIT_FAILURE );
                        }

                        namncpy( fs->seu_type, f, sz );
                    }
                    break;
                case OBJ_LIBFILE :
                default :
                    log_error( INV_OBJ_SPEC, NULL, line );
                    exit( TMK_EXIT_FAILURE );
                }
            }
            else {

                if( *tp != '>' && *tp != ',' ) {
                    log_error( INV_OBJ_SPEC, NULL, line );
                    exit( TMK_EXIT_FAILURE );
                }
                memcpy( fs->type, f, sz );
                fs->type[sz] = 0;
            }
            ++tp;
        }



        if( *tp != 0 && ! isspace( *tp ) ) {
            return( NULL );
        }

        *len = tp - orgp;
        rp = skip_white_spaces( tp );

#ifdef SRVOPT
        if( srvopt_fctrtn() )
            printf("RTN:parse_obj_name:\"%s\"\n  %s,%d\n",
                   rp, srv_fs(fs), *len);
#endif
        return( rp );
}
# 368 "TMKFILE.C"
Char *parse_file_ext ( Char *f, File_spec_t *fs ) {
        Char *tp;
        Int16 sz;

#ifdef SRVOPT
        if( srvopt_function() )
            printf("FCT:parse_file_ext(\"%s\",File_spec_t *fs"
                   ")\n",f );
#endif
        fs->lib[0] =
        fs->file[0] =
        fs->extmbr[0] =
        fs->type[0] =
        fs->seu_type[0] = 0;
        fs->obj_type = '*';
        fs->is_file = FALSE;

        if( ( tp = valid_name( f, &sz ) ) != NULL ) {

            namncpy( fs->file, f, sz );
            strcpy( fs->type, FS_T_FILE );
            fs->is_file = TRUE;

            if( *tp == '<' ) {

                ++tp;
                sz = sizeof( FS_T_FILE ) - 1;
                if( memcmp( tp, FS_T_FILE, sz ) == 0 ) {
                    tp += sz;
                    switch( *tp ) {
                    case ',' :

                        f = tp + 1;
                        tp = valid_name( f, &sz );
                        if( tp == NULL ¦¦ sz > SEU_TYP_SZ ¦¦
                              *tp != '>' ) {
                            tp = NULL;
                        }
                        else {

                            namncpy( fs->seu_type, f, sz );
                            ++tp;
                        }
                        break;
                    case '>' :
                        ++tp;
                        break;
                    default :
                        tp = NULL;
                    }
                }
                else {

                    tp = NULL;
                }
            }
        } else {

            if( *f == '<' ) {
                ++f;
                if( ( tp = valid_name( f, &sz ) ) == NULL ¦¦

                      ( *f == '*' ) ¦¦

                      ( *tp != '>' ) ) {

                    tp = NULL;
                }
                else {
                    namncpy( fs->type, f, sz );
                    ++tp;
                }
            } else
                tp = NULL;
        }
#ifdef SRVOPT
        if( srvopt_fctrtn() )
            printf("RTN:parse_file_ext:\"%s\"\n     %s\n",tp?tp:"NULL",
                     srv_fs(fs));
#endif
        return( tp );
}






Boolean parse_inference_rule ( Char *f, File_spec_t *fsd,
                                        File_spec_t *fst ) {
#ifdef SRVOPT
        if( srvopt_function() )
            printf("FCT:parse_inference_rule(\"%s\","
                   "File_spec_t *fsd,File_spec_t *fst)\n",f );
#endif
        if( *f++ != '.' ) {
            return( FALSE );
        }
        if( ( f = parse_file_ext( f, fsd ) ) == NULL ) {
            return( FALSE );
        }
        if( *f++ == '.' ) {
            return( parse_file_ext( f, fst ) != NULL );
        }


        fst->lib[0] =
        fst->file[0] =
        fst->extmbr[0] =
        fst->seu_type[0] = 0;
        fst->obj_type = '*';
        fst->is_file = FALSE;
        strcpy( fst->type, FS_T_PGM );
        return( TRUE );
}






Boolean obj_not_exist_flag;

Static Int32 axtoi( Char *bp ) {
        Int32 ri = 0;
        Int16 cnt = 4;

        while( cnt-- ) {
                ri <<= 4;
                ri += isdigit( *bp ) ? *bp - '0'
                                          : toupper( *bp ) - 'A' + 10;
                ++bp;
        }
        return( ri );
}
# 512 "TMKFILE.C"
Void obj_not_exist_trap ( int sig ) {
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
        if( memcmp( excinfo.Msg_Id, "CPF", 3 ) == 0 ) {
#else
        if( memcmp( data->exmsg->exmsgid, "CPF", 3 ) == 0 ) {
#endif
            cpfmsg = 0x00000;
        }
        else
#ifdef __ILEC400__
        if( memcmp( excinfo.Msg_Id, "CPD", 3 ) == 0 ) {
#else
        if( memcmp( data->exmsg->exmsgid, "CPD", 3 ) == 0 ) {
#endif
            cpfmsg = 0x10000;
        }
        else {

            return;
        }

#ifdef __ILEC400__
        cpfmsg += axtoi( excinfo.Msg_Id + 3 );
#else
        cpfmsg += axtoi( data->exmsg->exmsgid + 3 );
#endif

        if( cpfmsg >= 0x09800 && 0x09826 >= cpfmsg ) {

            obj_not_exist_flag = LSTOBJ_NOT_FOUND;
        }
        else {
            switch( cpfmsg ) {
            case 0x03caa :


                obj_not_exist_flag = LSTOBJ_EXCEED_USRSPC;
            case 0x09999 :
                obj_not_exist_flag = LSTOBJ_NOT_FOUND;
                break;
            case 0x03295 :
                obj_not_exist_flag = LSTOBJ_NOT_FOUND;
                break;
            case 0x03c20 :
                obj_not_exist_flag = LSTOBJ_NOT_FOUND;
                break;
            case 0x03c07 :
            case 0x13c31 :
                obj_not_exist_flag = LSTOBJ_INVALID_TYPE;
                break;
            default :


                return;
            }
        }

#ifdef __ILEC400__
        errcode.rtn_area_sz = 0;
        QMHCHGEM (&(excinfo.Target),
        0,
        excinfo.Msg_Ref_Key,
        MOD_RMVLOG,
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






Boolean update_file_date ( File_spec_t *fs, Int16 line ) {
#ifdef DEBUG
        Boolean last_debug = TRUE;
#endif
        D0100FMT od;
        mbrd0100 md;
        Char *tp;
        Char *cd;
        Char *ud;
        Char *o_type;
        Void (*old_signal_fct)( int );
        header_struct *list_header;
        OBJL0100 *list_obj;
        Int16 obj_count;
        Char obj_name[20];
        Char obj_mbr[10];
        Char obj_type[10];
        Char lib_date[13];
        Boolean rv;
        int error_code;

#ifdef SRVOPT
        if( srvopt_function() )
            printf("FCT:update_file_date(%s)\n",srv_fs(fs));
#endif

        set_date( &fs->last_update, 0L );
        set_date( &fs->create_date, 0L );
        set_date( &fs->proc_update, 0L );




        if( *fs->type == 0 ) {


            return( FALSE );
        }


        memset( md.change_date, '0', 13 );
        memset( md.crt_date, '0', 13 );
        memset( obj_name, ' ', FILE_NM_SZ + FILE_NM_SZ );
        memset( obj_mbr, ' ', FILE_NM_SZ );
        memset( obj_type, ' ', FILE_NM_SZ );


        if( fs->lib[0] == 0 )
            memcpy( obj_name + 10, FS_L_LIBL, sizeof( FS_L_LIBL ) - 1 );
        else
            memcpy( obj_name + 10, fs->lib, strlen( fs->lib ) );
        memcpy( obj_name, fs->file, strlen( fs->file ) );
        obj_name_toupper( obj_name, sizeof( obj_name ) );
        memcpy( obj_mbr, fs->extmbr, strlen( fs->extmbr ) );
        obj_name_toupper( obj_mbr, sizeof( obj_mbr ) );


        o_type = &fs->obj_type;


        obj_not_exist_flag = LSTOBJ_FOUND;
        old_signal_fct = signal( SIGALL, &obj_not_exist_trap );

        if( fs->is_file ) {



            if ( *fs->extmbr == 0 ) {



                memset( od.change_date, '0', 13 );
                memset( od.creation_date, '0', 13 );
                memcpy( obj_type, (const void *)o_type,
                                 strlen( (const char *) o_type ) );

                obj_not_exist_flag = LSTOBJ_FOUND;
                QUSROBJD( (char *)&od, sizeof(od), "OBJD0100",
                          (char *)obj_name, obj_type );

                cd = od.creation_date;
                ud = od.change_date;

                switch( obj_not_exist_flag ) {
                case LSTOBJ_FOUND :
                    set_date( &fs->last_update,
                              conv_as400_date( od.change_date[0] != ' '
                               ? od.change_date : od.creation_date ) );
                    set_date( &fs->create_date,
                              conv_as400_date( od.creation_date ) );
                    break;
                case LSTOBJ_INVALID_TYPE :
                    log_error( INV_OBJ_SPEC, NULL, line );
                    exit( TMK_EXIT_FAILURE );
                }
            }
            else {

                cd = md.crt_date;
                ud = md.change_date;

#if DEBUG
                    last_debug = TRUE;
#endif
                    error_code = 0;

                    obj_not_exist_flag = LSTOBJ_FOUND;
                    signal( SIGALL, &obj_not_exist_trap );
                    QUSRMBRD( (char *)&md, (int)sizeof( md ), "MBRD0100",
                              (char *)obj_name, (char *)obj_mbr, "1",
                              &error_code, "1");
                    if( obj_not_exist_flag == LSTOBJ_FOUND ) {

                        set_date( &fs->last_update,
                              conv_as400_date( md.change_date[0] != ' '
                                  ? md.change_date : md.crt_date ) );
                        set_date( &fs->create_date,
                              conv_as400_date( md.crt_date ) );
                    }
                    else {
                       if( obj_not_exist_flag == LSTOBJ_INVALID_TYPE ) {
                           log_error( INV_OBJ_SPEC, NULL, line );
                           exit( TMK_EXIT_FAILURE );
                       }
                       if( opt_debug() ) {
                           sprintf( txtbuf, "%20.20s/%-10.10s%9.9s/"
                                            "c:m=%13.13s/%13.13s",
                                   obj_name, obj_mbr,
                                   &fs->obj_type, cd, ud ) ;
                           log_dbg( txtbuf );
#if DEBUG
                           last_debug = FALSE;
#endif
                    }
                 }
            }
        }
        else
        if( strcmp( fs->type, FS_T_LIBFILE ) == 0 ) {
            _RFILE *rf;
            _RIOFB_T *iofb;
            struct libf {
                Char entrytype[4];
                Char publicname[100];
                Char pgmname[10];
                Char libname[10];
                Char language[10];
                Char type[6];
                Char init[7];
                Char hash[4];
                Char crtdattim[16];
            } rbuf;



            if( *fs->extmbr == 0 ) {


                memset( md.change_date, '0', 13 );
                memset( md.crt_date, '0', 13 );
                memset( obj_mbr, ' ', FILE_NM_SZ );
                memcpy( obj_mbr, obj_name, FILE_NM_SZ );

                cd = md.crt_date;
                ud = md.change_date;
# 800 "TMKFILE.C"
#if DEBUG
                    last_debug = TRUE;
#endif
                    error_code = 0;
                    obj_not_exist_flag = LSTOBJ_FOUND;
                    signal( SIGALL, &obj_not_exist_trap );
                    QUSRMBRD( (char *)&md, (int)sizeof( md ), "MBRD0100",
                              (char *)obj_name, (char *)obj_mbr, "1",
                              &error_code, "1");
                    if( obj_not_exist_flag == LSTOBJ_FOUND ) {

                        set_date( &fs->last_update,
                              conv_as400_date( md.change_date[0] != ' '
                                  ? md.change_date : md.crt_date ) );
                        set_date( &fs->create_date,
                              conv_as400_date( md.crt_date ) );
                    }
                    else {
                       if( obj_not_exist_flag == LSTOBJ_INVALID_TYPE ) {
                           log_error( INV_OBJ_SPEC, NULL, line );
                           exit( TMK_EXIT_FAILURE );
                       }


                       if( opt_debug() ) {
                           sprintf( txtbuf, "%20.20s/%-10.10s%9.9s/"
                                            "c:m=%13.13s/%13.13s",
                                   obj_name, obj_mbr,
                                   &fs->obj_type, cd, ud ) ;
                           log_dbg( txtbuf );
#if DEBUG
                           last_debug = FALSE;
#endif
                       }
                    }

            }
            else {

                memset( lib_date, '0', sizeof( lib_date ) );
                cd =
                ud = lib_date;



                sprintf( (char *)&rbuf, "%s%s%s", fs->lib,
                         fs->lib[0] != 0 ? "/" : "", fs->file );
#ifdef SRVOPT
            if( srvopt_detail() )
              printf("DTL:update_file_date:Ropen(%s)\n",(char *)&rbuf);
#endif
              if( ( rf = _Ropen( (char *)&rbuf, "rr blkrcd=y" ) )
                                  != NULL ) {
                   iofb = _Rreadk( rf, (char *)&rbuf, sizeof( rbuf ),
                                  __KEY_EQ, "PGM ", 4 );
#ifdef SRVOPT
        if( srvopt_detail() )
            printf("DTL:update_file_date:libfile_rec:%4.4s/%10.10s/"
              "%16.16s/%10.10s/%d/%d\n", rbuf.entrytype, rbuf.publicname,
                   rbuf.crtdattim,obj_mbr,iofb->num_bytes,sizeof(rbuf) );
#endif
                    while( iofb->num_bytes == 167 ) {
#ifdef SRVOPT
        if( srvopt_detail() )
            printf("DTL:update_file_date:libfile_rec:%4.4s/%10.10s/"
               "%16.16s/%10.10s/%d\n", rbuf.entrytype, rbuf.publicname,
                   rbuf.crtdattim,obj_mbr,iofb->num_bytes );
#endif
                        if( memcmp( rbuf.publicname, obj_mbr,
                                    sizeof( obj_mbr ) ) == 0 ) {
                            lib_date[0] = '0';
                            lib_date[1] = rbuf.crtdattim[6];
                            lib_date[2] = rbuf.crtdattim[7];
                            lib_date[3] = rbuf.crtdattim[0];
                            lib_date[4] = rbuf.crtdattim[1];
                            lib_date[5] = rbuf.crtdattim[3];
                            lib_date[6] = rbuf.crtdattim[4];
                            lib_date[7] = rbuf.crtdattim[8];
                            lib_date[8] = rbuf.crtdattim[9];
                            lib_date[9] = rbuf.crtdattim[11];
                            lib_date[10] = rbuf.crtdattim[12];
                            lib_date[11] = rbuf.crtdattim[14];
                            lib_date[12] = rbuf.crtdattim[15];
                            set_date( &fs->last_update,
                                      conv_as400_date( lib_date ) );
                            fs->create_date = fs->last_update;
                            break;
                        }
                        iofb = _Rreadk( rf, (char *)&rbuf, sizeof( rbuf ),
                                        __KEY_NEXTEQ, "PGM ", 4 );
                    }
                    _Rclose( rf );
                }
            }
        }
        else
        if( strcmp( fs->type, FS_T_TXTLIB ) == 0 ) {


            LibHdr_T *lib_header;
            MemList_T *txtmbr;
            Int32 cnt;


            if( *fs->extmbr == 0 ) {

                o_type = "*USRSPC";
                goto gen_get_date;

            }
            memset( lib_date, '0', sizeof( lib_date ) );
            cd =
            ud = lib_date;


            creat_usrspc();
            QUSLOBJ( LST_USRSPC_NM, "OBJL0100", obj_name, "*USRSPC   " );
            QUSPTRUS( LST_USRSPC_NM, &list_header );
            list_obj = (OBJL0100 *)(((char *)list_header) +
                            list_header->list_section_offset );
            obj_count = list_header->number_of_entries;

#ifdef SRVOPT
        if( srvopt_detail() )
            printf("DTL:update_file_date:txtlib:obj_count=%d\n",
                    obj_count );
#endif
            while( obj_count-- ) {
#ifdef DEBUG
                last_debug = TRUE;
#endif
                memcpy( obj_name + 10, list_obj->library_name, 10 );

                obj_not_exist_flag = LSTOBJ_FOUND;
                signal( SIGALL, &obj_not_exist_trap );


                QUSPTRUS( obj_name, &lib_header );
                if( obj_not_exist_flag == LSTOBJ_FOUND) {


                    txtmbr = (MemList_T *)
                       (((char *)lib_header) + lib_header->MemTabStart );
#ifdef SRVOPT
        if( srvopt_detail() )
            printf("DTL:update_file_date:txtlib:mbr_count=%d\n",
                    lib_header->MemberCount );
#endif
                    for( cnt = 0 ; cnt < lib_header->MemberCount ;
                            ++cnt ) {
#ifdef SRVOPT
        if( srvopt_detail() )
          printf("DTL:update_file_date:txtlib:mbr_name=\"%s\"=\"%s\"\n",
                    txtmbr->Name,obj_mbr );
#endif

                        if( memcmp( txtmbr->Name, obj_mbr,
                                    strlen( fs->extmbr ) ) == 0 ) {


                            lib_date[0] = '0';
                            lib_date[1] = txtmbr->Date[0];
                            lib_date[2] = txtmbr->Date[1];
                            lib_date[3] = txtmbr->Date[3];
                            lib_date[4] = txtmbr->Date[4];
                            lib_date[5] = txtmbr->Date[6];
                            lib_date[6] = txtmbr->Date[7];
                            lib_date[7] = txtmbr->Time[0];
                            lib_date[8] = txtmbr->Time[1];
                            lib_date[9] = txtmbr->Time[3];
                            lib_date[10] = txtmbr->Time[4];
                            lib_date[11] = txtmbr->Time[6];
                            lib_date[12] = txtmbr->Time[7];
                            set_date( &fs->last_update,
                                      conv_as400_date( lib_date ) );
                            fs->create_date = fs->last_update;
                            break;
                        }

                        ++txtmbr;
                    }



                    if( cnt < lib_header->MemberCount )
                            break;
                }

                ++list_obj;
                if( opt_debug() ) {
                   sprintf( txtbuf, "%20.20s/%-10.10s%9.9s/"
                                    "c:m=%13.13s/%13.13s",
                           obj_name, obj_mbr,
                           &fs->obj_type, cd, ud );
                   log_dbg( txtbuf );
#ifdef DEBUG
                   last_debug = FALSE;
#endif
                }
            }
        }
        else {
            if( strcmp( fs->type, FS_T_PGMSET ) == 0 ) {

                o_type = "*PGM";
            }
gen_get_date:


            memset( od.change_date, '0', 13 );
            memset( od.creation_date, '0', 13 );
            memcpy( obj_type, (const void *)o_type,
                             strlen( (const char *) o_type ) );

            obj_not_exist_flag = LSTOBJ_FOUND;
            QUSROBJD( (char *)&od, sizeof(od), "OBJD0100",
                      (char *)obj_name, obj_type );

            cd = od.creation_date;
            ud = od.change_date;

            switch( obj_not_exist_flag ) {
            case LSTOBJ_FOUND :
                set_date( &fs->last_update,
                          conv_as400_date( od.change_date[0] != ' '
                             ? od.change_date : od.creation_date ) );
                set_date( &fs->create_date,
                          conv_as400_date( od.creation_date ) );
                break;
            case LSTOBJ_INVALID_TYPE :
                log_error( INV_OBJ_SPEC, NULL, line );
                exit( TMK_EXIT_FAILURE );
            }
        }
        signal( SIGALL, old_signal_fct );

#ifdef DEBUG
        if( opt_debug() && last_debug ) {
            sprintf( txtbuf, "%20.20s/%-10.10s%9.9s/c:m=%13.13s/%13.13s",
                    obj_name, obj_mbr, o_type, cd, ud );
            log_dbg( txtbuf );
        }
#endif

        rv = get_date( &fs->last_update ) != 0L;
#ifdef SRVOPT
        if( srvopt_fctrtn() )
            printf("RTN:update_file_date:%d\n",rv);
#endif
        return( rv );
}






Static Buf_t objn_buf = { NULL, NULL, 0 };

Char *obj_full_name ( File_spec_t *fs ) {

#ifdef SRVOPT
        if( srvopt_function() )
            printf("FCT:obj_full_name(%s)\n",srv_fs(fs));
#endif
        reset_buf( &objn_buf );
        if( fs->lib[0] ) {
                append_buf( &objn_buf, fs->lib );
                append_buf( &objn_buf, "/" );
        }
        if( fs->is_file ) {
            append_buf( &objn_buf, fs->extmbr );
            append_buf( &objn_buf, "." );
            append_buf( &objn_buf, fs->file );
        }
        else {
            append_buf( &objn_buf, fs->file );
            if( *fs->extmbr != 0 ) {
                append_buf( &objn_buf, "(" );
                append_buf( &objn_buf, fs->extmbr );
                append_buf( &objn_buf, ")" );
             }
        }

        if( *fs->seu_type != 0 ¦¦




            ( strcmp( fs->type, FS_T_PGM ) != 0 &&
              strcmp( fs->type, FS_T_FILE ) != 0 &&
              ( strcmp( fs->type, FS_T_LIBFILE ) != 0 ¦¦
                *fs->extmbr == 0 ) ) ) {
            append_buf( &objn_buf, "<" );
            append_buf( &objn_buf, fs->type );
            if( *fs->seu_type != 0 ) {
                append_buf( &objn_buf, "," );
                append_buf( &objn_buf, fs->seu_type );
            }
            append_buf( &objn_buf, ">" );
        }

#ifdef SRVOPT
        if( srvopt_fctrtn() )
            printf("RTN:obj_full_name:\"%s\"\n",objn_buf.bp);
#endif
        return( objn_buf.bp );
}






Boolean obj_exists ( File_spec_t *fs, Int16 line ) {
#ifdef SRVOPT
        if( srvopt_function() )
            printf("FCT:obj_exists(File_spec_t *fsd)\n");
#endif
        return( update_file_date( fs, line ) );
}






Char *skip_obj_name ( Char *txt ) {
#ifdef SRVOPT
        if( srvopt_function() )
            printf("FCT:skip_obj_name(\"%s\")\n",txt);
#endif
        while( *txt && ( isalnum( *txt ) ¦¦
                         look_for( "./(),*<>_", *txt ) != NULL ) ) {
            ++txt;
        }
#ifdef SRVOPT
        if( srvopt_fctrtn() )
            printf("RTN:skip_obj_name:\"%s\"\n",txt);
#endif
        return( txt );
}






#pragma linkage(TMKTCHMB, OS)
#pragma linkage(TMKTCHOB, OS)

extern void TMKTCHMB ( char *, char *, char * );
extern void TMKTCHOB ( char *, char *, char * );

Void touch_target ( Element_t *ep ) {
     File_spec_t *fs;
     char type[11];
     char file[10];
     char lib[10];
     char mbr[10];

#ifdef SRVOPT
        if( srvopt_function() )
            printf("FCT:touch_target(\"%s\")\n",ep->name);
#endif

    fs = &ep->fs;
    if ( *fs->type != 0 )
    {
        if ( fs != NULL && strlen(fs->file) )
        {
            memset ( file, ' ', 10 );
            memcpy ( file, fs->file, strlen(fs->file) );
            if ( !strlen(fs->lib) )
            {
                memcpy ( lib, "*LIBL     ", 10 );
            }
            else
            {
                memset ( lib, ' ', 10 );
                memcpy ( lib, fs->lib, strlen(fs->lib) );
            }
            if ( !strcmp ( fs->type, FS_T_FILE ) &&
                 *fs->extmbr != 0 )
            {
                memset ( mbr, ' ', 10 );
                memcpy ( mbr, fs->extmbr, strlen(fs->extmbr) );
                TMKTCHMB ( file, lib, mbr );
            }
            else
            {
                if ( strcmp ( fs->type, FS_T_PGMSET ) == 0 )
                {
                     memcpy ( type, "*PGM       ", 11 );
                }
                else if ( strcmp ( fs->type, FS_T_LIBFILE ) == 0 )
                {
                     memcpy ( type, "*FILE      ", 11 );
                }
                else if ( strcmp ( fs->type, FS_T_TXTLIB ) == 0 )
                {
                    memcpy ( type, "*USRSPC    ", 11 );
                }
                else
                {
                    memset ( type, ' ', 11 );
                    type[0] = '*';
                    memcpy ( type+1, fs->type, strlen(fs->type) );
                }
                TMKTCHOB ( file, lib, type );
            }
        }
    }
}
# 1226 "TMKFILE.C"
Boolean same_obj_base ( File_spec_t *tfs, File_spec_t *dfs ) {
        Boolean rv = FALSE;
#ifdef SRVOPT
        if( srvopt_function() ) {
            sprintf(srv_cat,"FCT:same_obj_base(%s,\n",srv_fs(tfs));
            printf("%s  %s)\n",srv_cat,srv_fs(dfs));
        }
#endif
        if( strcmp( tfs->lib, dfs->lib ) == 0 ) {




            Int16 tfile = memcmp( tfs->type, FS_T_FILE,
                                    sizeof( FS_T_FILE ) - 1 ) == 0;
            Int16 dfile = memcmp( dfs->type, FS_T_FILE,
                                    sizeof( FS_T_FILE ) - 1 ) == 0;
            rv = strcmp( tfile ? tfs->extmbr : tfs->file,
                            dfile ? dfs->extmbr : dfs->file ) == 0;
        }
#ifdef SRVOPT
        if( srvopt_fctrtn() ) {
            printf("RTN:same_obj_base:%d\n",rv);
        }
#endif
        return( rv );
}
