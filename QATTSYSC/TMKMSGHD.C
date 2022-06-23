/* ================================================================= */
/*                                                                   */
/*  MODULE:    TMKMSGHD                                              */
/*                                                                   */
/*                                                                   */
/*  MODULE-TYPE: C Library                                           */
/*                                                                   */
/*  Processor:  C                                                    */
/*                                                                   */
/*  Purpose:    Message handling module                              */
/*                                                                   */
/* ================================================================= */


#include        <stdio.h>
#include        <stdlib.h>
#include        <string.h>

#include        "tmkhbase.qattsysc"
#include        "tmkhmsgh.qattsysc"
#include        "tmkhopna.qattsysc"


/*****************************************************************************
        Variables definitions
*****************************************************************************/

Static  Boolean   usrmsg_to_session = FALSE;
Static  error_rtn errrtn = { 0 };
Static  Char      msg_key[4];
Static  Char      msg_txt[MSG_TXT_SZ+MSG_LINE_NO_SZ];


/* ================================================================= */
/*  Function:    set_usrmsg_to_session ()                            */
/* ================================================================= */

Void    set_usrmsg_to_session( Void ) {
        usrmsg_to_session = TRUE;
}

/* ================================================================= */
/*  Function:    get_usrmsg_to_session ()                            */
/* ================================================================= */

Boolean get_usrmsg_to_session( Void ) {
        return( usrmsg_to_session );
}


/* ================================================================= */
/*  Function:    log_error ()                                        */
/* ================================================================= */

Void    log_error ( Char *msgid, Char *txt, Int32 line_no ) {
        Char   *msg    = msg_txt;
        Int32  msg_sz  = 0;
        Int32  tmp_sz;

        if( line_no > 0 ) {
            /* setup the &1 if the message has line no information   */
            sprintf( msg_txt, "%5d", line_no );
            msg = msg_txt + 5;
            msg_sz = 5;
        }
        tmp_sz = ( txt == NULL ) ? 0 : strlen( txt );
        if( tmp_sz != 0 ) {
            /* setup the rest of &2, &3.... parameters               */
            tmp_sz = Min( MSG_TXT_SZ, tmp_sz );
            memcpy( msg, txt, tmp_sz );
            msg_sz += tmp_sz;
        }

        QMHSNDPM( msgid, MSGF_PATH, msg_txt, msg_sz, MSGT_COMPLETE,
                  "*         ", 0, msg_key, (char *)&errrtn );
}


/* ================================================================= */
/*  Function:    log_usrmsg ()                                       */
/* ================================================================= */

Void    log_usrmsg ( Char *txt ) {
        if( usrmsg_to_session ) {
            fprintf( stdout, "%s\n", txt );
        }
        else {
            log_error( USER_CMD, txt, MSG_NO_LINE_NO );
        }
}


/* ================================================================= */
/*  Function:    log_dbg ()                                          */
/* ================================================================= */

Void    log_dbg ( Char *txt ) {
        fprintf( stdout, "%s\n", txt );
}
/*
int main( void ) {
        log_error ( "TMK9005", "This is more text", 1 );
}
*/

