 TMKTCHMB:   PGM        PARM(&OBJN &OBJL &OBJM)
             DCL        VAR(&OBJN) TYPE(*CHAR) LEN(10)
             DCL        VAR(&OBJL) TYPE(*CHAR) LEN(10)
             DCL        VAR(&OBJM) TYPE(*CHAR) LEN(10)
             DCL        VAR(&OBJX) TYPE(*CHAR) LEN(50)
             DCL        VAR(&OBJA) TYPE(*CHAR) LEN(3)

             CHKOBJ     OBJ(&OBJL/&OBJN)  OBJTYPE(*FILE) MBR(&OBJM)
             MONMSG     MSGID(CPF9815)   EXEC(GOTO CMDLBL(RTN))
             RTVMBRD    FILE(&OBJL/&OBJN) MBR(&OBJM) FILEATR(&OBJA) +
                          TEXT(&OBJX)
             IF         COND(&OBJA *EQ '*PF') THEN(CHGPFM +
                          FILE(&OBJL/&OBJN) MBR(&OBJM) TEXT(&OBJX))
             ELSE       CMD(CHGLFM FILE(&OBJL/&OBJN) MBR(&OBJM) +
                          TEXT(&OBJX))
RTN:         RETURN
             ENDPGM
