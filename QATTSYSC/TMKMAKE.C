/*******************************************************************/
/*                                                                 */
/* MODULE NAME: TMKMAKE                                            */
/*                                                                 */
/* DESCRIPTIVE NAME: COMMAND SOURCE FOR TMKMAKE COMMAND            */
/*                                                                 */
/* COPYRIGHT:                                                      */
/*                                                                 */
/* FUNCTION:                                                       */
/*     SOURCE FOR THE COMMAND TMKMAKE                              */
/*                                                                 */
/* MODULE TYPE:                                                    */
/*     PROCESSOR: CL                                               */
/*     MODULE SIZE:  ///  STATEMENTS                               */
/*     ATTRIBUTES; CDO                                             */
/*                                                                 */
/* ENTRY POINT: COMMAND TMKMAKE                                    */
/*     LINKAGE: COMMAND ANALYZER                                   */
/*                                                                 */
/* INPUT:                                                          */
/*   COMMAND INPUT DEVICE                                          */
/*                                                                 */
/* OUTPUT:                                                         */
/*   FROM COMMAND ANALYZER                                         */
/*                                                                 */
/* EXIT NORMAL: CALL MKMAKE                                        */
/*                                                                 */
/* EXIT ERROR: RETURN                                              */
/*                                                                 */
/* EXCEPTIONS: N/A                                                 */
/*                                                                 */
/*******************************************************************/
 TMKMAKE: CMD PROMPT(TMK0001)

 /*                                             */
 /*  TARGET LABEL - TARGET(*FIRST)              */
 /*                                             */
 /*                                             */
         PARM       KWD(MAKETARG)                                 +
                      TYPE(*CHAR) LEN(54)                         +
                      MIN(0) MAX(30)                              +
                      DFT(*FIRST)                                 +
                      SPCVAL((*FIRST '*FIRST'))                   +
                      PROMPT(TMK0002)

 /*                                             */
 /*  SOURCE FILE - SRCFILE(LIB/FILE)            */
 /*                                             */
 /*                                             */
         PARM       KWD(SRCFILE)                                  +
                      TYPE(Q1)                                    +
                      FILE(*IN)                                   +
                      PROMPT(TMK0003)

 /*                                             */
 /*  SOURCE MEMBER - SRCMBR(MBR)                */
 /*                                             */
 /*                                             */
         PARM       KWD(SRCMBR)                                   +
                      TYPE(*NAME) LEN(10)                         +
                      DFT(*FIRST)                                 +
                      SPCVAL((*FIRST '*FIRST')                    +
                             (*ALL   '*ALL'  )                    +
                            )                                     +
                      PROMPT(TMK0005)

 /*                                             */
 /*  MACRO DEFINITIONS - MACRO(MARCO)           */
 /*                                             */
 /*                                             */
         PARM       KWD(MACRO)                                    +
                      TYPE(*CHAR)                                 +
                      MIN(0) MAX(50)                              +
                      PROMPT(TMK0006)

 /*                                             */
 /*  OPTIONS - OPTION(OPTIONS)                  */
 /*                                             */
 /*                                             */
         PARM       KWD(OPTION)                                   +
                      TYPE(*CHAR)                                 +
                      LEN(1)                                      +
                      RSTD(*YES)                                  +
                      SPCVAL((*NOIGNORE       J)                  +
                             (*IGNORE         I)                  +
                             (*NOSILENT       V)                  +
                             (*SILENT         S)                  +
                             (*NOEXEC         F)                  +
                             (*EXEC           E)                  +
                             (*NOBIRULES      C)                  +
                             (*BIRULES        B)                  +
                             (*NOTOUCH        U)                  +
                             (*TOUCH          T)                  +
                             (*NODEBUG        X)                  +
                             (*DEBUG          D)                  +
                            )                                     +
                      PROMPT(TMK0007)                             +
                      CHOICE(*NONE)                               +
                      MIN(0) MAX(10)

 /*                                             */
 /* MARGINS - Source margins to use to extract  */
 /*           data from the file.               */
         PARM       KWD(MARGINS)                                  +
                      PMTCTL(*PMTRQS)                             +
                      TYPE(Q2)                                    +
                      PROMPT(TMK0008)

 /*                                             */
 /* RETURN CODE HANDLING - RTNCDE(*EXCEPTION)   */
 /*                                             */
         PARM       KWD(RTNCDE)                                   +
                      PMTCTL(*PMTRQS)                             +
                      TYPE(Q3)                                    +
                      PROMPT(TMK0011)

 /*                                             */
 /*  MESSAGE LOGGIN METHODS - MSGLOG(*JOBLOG)   */
 /*                                             */
 /*                                             */
         PARM       KWD(MSGLOG)                                   +
                      PMTCTL(*PMTRQS)                             +
                      TYPE(*CHAR)                                 +
                      LEN(1)                                      +
                      RSTD(*YES)                                  +
                      DFT(*JOBLOG)                                +
                      SPCVAL((*JOBLOG        J)                   +
                             (*SESSION       S)                   +
                            )                                     +
                      PROMPT(TMK0013)

 /*                                             */
 /*  Service option (internal) for debugging    */
 /*                                             */
 /*                                             */
         PARM       KWD(SRVOPT)                                   +
                      PMTCTL(*PMTRQS)                             +
                      TYPE(*CHAR)                                 +
                      LEN(1)                                      +
                      RSTD(*YES)                                  +
                      DFT(*NONE)                                  +
                      SPCVAL((*NONE          N)                   +
                             (*FUNCTION      F)                   +
                             (*FCTRTN        G)                   +
                             (*DETAIL        D)                   +
                             (*DUMPMACRO     M)                   +
                             (*DUMPBI        B)                   +
                             (*DUMPRULES     R)                   +
                             (*DUMPSUFFIX    S)                   +
                             (*DUMPDEFCMD    C)                   +
                             (*DUMPIO        I)                   +
                             (*ALL           A)                   +
                            )                                     +
                      MIN(0) MAX(9)                               +
                      PROMPT('Internal service options')

 /*                                             */
 /* SOURCE FILE INPUT WITH QUALIFIER            */
 /*                                             */
 /*                                             */
 Q1:     QUAL       TYPE(*NAME) LEN(10)                           +
                      RSTD(*NO)                                   +
                      DFT(QMAKSRC)                                 +

         QUAL       TYPE(*NAME) LEN(10)                           +
                      RSTD(*NO)                                   +
                      DFT(*LIBL)                                  +
                      SPCVAL((*LIBL '*LIBL') (*CURLIB '*CURLIB')) +
                      PROMPT(TMK0004)

 /*                                              */
 /* SOURCE MARGINS                               */
 /*                                              */
 Q2:     ELEM       TYPE(*INT2)                                   +
                      DFT(*SRCFILE)                               +
                      RANGE(1 32754)                              +
                      SPCVAL((*SRCFILE 0))                        +
                      PROMPT(TMK0009)

         ELEM       TYPE(*INT2)                                   +
                      DFT(256)                                    +
                      RANGE(1 32754)                              +
                      PROMPT(TMK0010)

 /*                                             */
 /* FLAGGING LEVEL - Indicates level of messages*/
 /*                  to be displayed.           */
 /*                                             */
 Q3:     ELEM       TYPE(*INT2)                                   +
                      RSTD(*YES)                                  +
                      DFT(*EXCEPTION)                             +
                      SPCVAL((*EXCEPTION  1)                      +
                             (*LURC       2)                      +
                             (*BOTH       3)                      +
                            )                                     +
                      PROMPT(TMK0014)

         ELEM       TYPE(*INT2)                                   +
                      DFT(30)                                     +
                      RANGE(0 40)                                 +
                      PROMPT(TMK0012)

