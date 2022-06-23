 TMKTCHOB:   PGM        PARM(&OBJN &OBJL &OBJT)
             DCL        VAR(&OBJN) TYPE(*CHAR) LEN(10)
             DCL        VAR(&OBJL) TYPE(*CHAR) LEN(10)
             DCL        VAR(&OBJT) TYPE(*CHAR) LEN(11)
             DCL        VAR(&OBJX) TYPE(*CHAR) LEN(50)

             CHKOBJ     OBJ(&OBJL/&OBJN) OBJTYPE(&OBJT)
             MONMSG     MSGID(CPF9801 CPF9811) EXEC(GOTO CMDLBL(RTN))
             RTVOBJD    OBJ(&OBJL/&OBJN) OBJTYPE(&OBJT) TEXT(&OBJX)
             CHGOBJD    OBJ(&OBJL/&OBJN) OBJTYPE(&OBJT) TEXT(&OBJX)
RTN:         RETURN
             ENDPGM
