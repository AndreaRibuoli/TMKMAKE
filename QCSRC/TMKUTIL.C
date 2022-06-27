#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <signal.h>

#include "tmkbase.h"
#include "tmkutil.h"
#include "tmkdict.h"
#include "tmkfile.h"
#include "tmkparse.h"
#include "tmkoptio.h"
#include "tmkopna.h"
#include "tmkmsghd.h"





Buf_t t1 = { NULL, NULL, 0 };
Buf_t t2 = { NULL, NULL, 0 };

Char txtbuf[ WRKBUF_SZ ];
Static Boolean usrspc_created = FALSE;
Static Char dltusrspc[80];






Char *skip_white_spaces ( Char *txt ) {
        while( *txt && isspace( *txt ) )
            ++txt;
        return( txt );
}






Char *skip_non_white_spaces ( Char *txt ) {
        while( *txt && !isspace( *txt ) )
            ++txt;
        return( txt );
}






Int16 skip_trail_spaces ( Char *txt ) {
        Int16 read_cnt;
        Char *tp;

        read_cnt= strlen( txt );
        tp = txt + read_cnt - 1;
        while( read_cnt && isspace( *tp ) ) {
            --read_cnt;
            *tp-- = 0;
        }
        return( read_cnt );
}






Int16 skip_trail_spaces_sz ( Char *txt, Int16 read_cnt ) {
        Char *tp;

        tp = txt + read_cnt - 1;
        while( read_cnt && isspace( *tp ) ) {
            --read_cnt;
            --tp;
        }
        return( read_cnt );
}





Char *skip_macro_sym ( Char *txt ) {
        while( *txt &&
               ( isalnum( *txt ) || strchr( "_<?@*%", *txt ) != NULL ) )
            ++txt;
        return( txt );
}






Char *skip_alpha ( Char *txt ) {
        while( *txt && isalpha( *txt ) )
            ++txt;
        return( txt );
}






Char *look_for ( Char *txt, Char c ) {
        while( *txt && *txt != c )
            ++txt;
        return( *txt == 0 ? NULL : txt );
}






Char *to_upper_str ( Char *s ) {
        Char *r = s;

        while( *s ) {
            *s = toupper( *s );
            ++s;
        }
        return( r );
}






Char *strstr_no_case ( Char *s, Char *c ) {
        Char *tp;

        reset_buf( &t2 );
        append_buf( &t2, s );
        to_upper_str( t2.bp );
        if( ( tp = strstr( t2.bp, c ) ) == NULL )
            return( NULL );

        return( s + ( tp - t2.bp ) );
}






Void reset_buf ( Buf_t *buf ) {
        if( buf->bp == NULL ) {
            buf->bp = (Char *)alloc_buf( WRKBUF_SZ, "reset_buf()" );
            buf->tsz= WRKBUF_SZ;
        }
        buf->tp = buf->bp;
        buf->tp[0] = 0;
}






Void append_buf ( Buf_t *buf, Char *ap ) {
        Char *np;
        Int16 nsz;
        Int16 osz = buf->tp - buf->bp;

        if( ap == NULL )
            return;

        nsz = strlen( ap );
        if( ( osz + nsz ) >= buf->tsz ) {
            nsz = Max( nsz, WRKBUF_SZ );
            np = (Char *)alloc_buf( buf->tsz + nsz, "append_buf()" );
            buf->tsz += nsz;
            strcpy( np, buf->bp );
            free( buf->bp );
            buf->bp = np;
            buf->tp = buf->bp + osz;
        }
        strcpy( buf->tp, ap );
        buf->tp += nsz;
}






Void *alloc_buf ( Int16 sz, Char *err_txt ) {
        Char *rtn_txt;

        if( ( rtn_txt = malloc( sz ) ) == NULL ) {
            log_error( NO_HEAP, err_txt, MSG_NO_LINE_NO );
            exit( TMK_EXIT_FAILURE );
        }
        return( rtn_txt );
}






Int16 cmp_date( Date_t d1, Date_t d2 ) {
        Int32 diff = d1.val - d2.val;

        return( diff ? ( diff > 0 ? 1 : -1 ) : 0 );
}






Boolean set_date( Date_t *d, Int32 val ) {
        d->val = val;
        return( TRUE );
}






Int32 get_date( Date_t *d ) {
        return( d->val );
}






Char *obj_name_toupper ( Char *obj_name, Int16 sz ) {
        Char *cp = obj_name;

        while( sz-- ) {
            *cp = toupper( *cp );
            ++cp;
        }
        return( obj_name );
}






Int32 conv_as400_date ( Char *date ) {
        Int32 time;

#define Digit(n) (date[n]-((Int32)'0'))

        time = 0;
        time += ((Digit(1) < 7) ? (10+Digit(1)-7) : Digit(1)-7) * 10L
                   + Digit(2);
        time *= 12L;
        time += Digit(3) * 10L + Digit(4);
        time *= 31L;
        time += Digit(5) * 10L + Digit(6);
        time *= 24L;
        time += Digit(7) * 10L + Digit(8);
        time *= 60L;
        time += Digit(9) * 10L + Digit(10);
        time *= 60L;
        time += Digit(11) * 10L + Digit(12);

        return( time );
}







Void usrspc_exist_trap ( int sig ) {
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
        if( memcmp( excinfo.Msg_Id, "CPF9870", 7 ) != 0 ) {
#else
        if( memcmp( data->exmsg->exmsgid, "CPF9870", 7 ) != 0 ) {
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






Void creat_usrspc ( Void ) {
        Void (*old_signal_fct)( int );

        if( ! usrspc_created ) {

            old_signal_fct = signal( SIGALL, &usrspc_exist_trap );
            QUSCRTUS( LST_USRSPC_NM, "MAKEATTR  ", 2048,
                    " ", "*CHANGE   ", "STRMAKE List Work Space" );
            signal( SIGALL, old_signal_fct );

            usrspc_created = TRUE;
        }
}






Void usrspc_delete_trap ( int sig ) {
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
        if( memcmp( _EXCP_MSGID, "CPC2191", 7 ) != 0 ) {
#else
        if( memcmp( data->exmsg->exmsgid, "CPC2191", 7 ) != 0 ) {
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






Void delete_usrspc ( Void ) {
        Void (*old_signal_fct)( int );

        if( usrspc_created ) {

            sprintf( dltusrspc, "DLTUSRSPC USRSPC(%s)", DLT_USRSPC_NM );

            old_signal_fct = signal( SIGALL, &usrspc_delete_trap );
            system( dltusrspc );
            signal( SIGALL, old_signal_fct );
        }
}
