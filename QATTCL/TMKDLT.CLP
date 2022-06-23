/*********************************************************************/
/*                                                                   */
/* NAME: TMKDLT  - DELETION OF MAKE UTILITY SOURCE AND LIBRARY       */
/*                 DELETE THE MAKE UTILITY SOURCE IF SPECIFIED AND   */
/*                 DELETE THE USER'S MAKE UTILITY LIBRARY            */
/*                                                                   */
/* PARAMETER:                                                        */
/*       OBJL      USER-SPECIFED LIBRARY NAME                        */
/*                 IF USER'S LIBRARY IS SPECIFIED, THE LIBRARY WILL  */
/*                 BE DELETED.   IF *NONE IS SPECIFIED, NO LIBRARY   */
/*                 WILL BE DELETED.                                  */
/*                                                                   */
/*       DLTSRC    IF *YES IS SPECIFIED, ALL THE SOURCE PROGRAMS FOR */
/*                 MAKE UTILITY WILL BE DELETED FROM QUSRTOOL LIBRARY*/
/*                 OTHERWISE, NO SOURCE WILL BE DELETED.             */
/*                                                                   */
/* INVOCATION:     CALL TMKINST DLTSRC OBJL                          */
/*                                                                   */
/*********************************************************************/

 TMKDLT:     PGM        PARM(&DLTSRC &OBJL)
             DCL        VAR(&DLTSRC) TYPE(*CHAR) LEN(4)
             DCL        VAR(&OBJL) TYPE(*CHAR) LEN(10)

             IF         COND(&DLTSRC *EQ '*YES') THEN(DO)
                          RMVM FILE(QUSRTOOL/QATTSYSC) MBR(TMK*)
                          RMVM FILE(QUSRTOOL/QATTCL)  MBR(TMK*)
                          RMVM FILE(QUSRTOOL/QATTCMD)  MBR(TMKMAKE)
                        ENDDO
             IF         COND(&OBJL *NE '*NONE') THEN(DLTLIB +
                           LIB(&OBJL))
             ENDPGM
