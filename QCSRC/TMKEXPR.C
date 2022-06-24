#include        <stdio.h>
#include        <stdlib.h>
#include        <string.h>
#include        <ctype.h>

#include        "tmkbase.h"
#include        "tmkmsghd.h"
#include        "tmkutil.h"
#include        "tmkdict.h"
#include        "tmkfile.h"
#include        "tmkparse.h"
#include        "tmkoptio.h"

int     yyparse( Void );
Int32   yylex( Void );

# define TOKEN_BASE 257
# define EXIT 258
# define NUMBER 259
# define CMPL 260
# define NOT 261
# define ADD 262
# define SUB 263
# define MULT 264
# define DIV 265
# define MOD 266
# define SHFLT 267
# define SHFRT 268
# define AND 269
# define OR 270
# define EQ 271
# define NEQ 272
# define GE 273
# define GT 274
# define LE 275
# define LT 276
# define BOR 277
# define BAND 278
# define BXOR 279
# define LEFTP 280
# define RIGHTP 281
# define ENDINPUT 282
# define UMINUS 283
# define UPLUS 284
# define UCMPL 285
# define UNOT 286
#define yyclearin yychar = -1
#define yyerrok yyerrflag = 0
extern int yychar;
extern int yyerrflag;
#ifndef YYMAXDEPTH
#define YYMAXDEPTH 150
#endif
#ifndef YYSTYPE
#define YYSTYPE int
#endif
YYSTYPE yylval, yyval;
typedef int yytabelem;
# define YYERRCODE 256


Static  Char    *bp;
Static  Int32   bc;
Static  Int16   line_no;

Int32   evaluate_exp ( Char *s, Int16 line )
{
  Int32   rc;
  
#ifdef  SRVOPT
  if( srvopt_function() )
    printf("FCT:evaluate_exp(\"%s\",%d)\n",s,line);
#endif
  bp              = s;
  bc              = strlen( s );
  line_no         = line;
  
  rc      = yyparse();
  
#ifdef  SRVOPT
  if( srvopt_fctrtn() )
    printf("RTN:evaluate_exp:%d\n",rc);
#endif
  return( rc );
}

#if     YYDEBUG

char    b[512];

main()
{
  while( fgets( b, 511, stdin ) != NULL ) {
    switch( evaluate_exp( b, strlen( b ) ) ) {
      case -1             : exit( 0 );
      case 0              :
        printf("%d\n", return_value);
        break;
      case ERR_INV_SYNTAX :
        printf("yyparse() : ERR_INV_SYNTAX\n"); break;
      case ERR_INV_TOKEN  :
        printf("yyparse() : ERR_INV_TOKEN\n"); break;
      case ERR_INV_OCT    :
        printf("yyparse() : ERR_INV_OCT\n"); break;
      case ERR_INV_HEX    :
        printf("yyparse() : ERR_INV_HEX\n"); break;
      case ERR_TOO_COMPLEX        :
        printf("yyparse() : ERR_TOO_COMPLEX\n"); break;
      case ERR_INTERNAL_ERR:
        printf("yyparse() : ERR_INTERNAL_ERR\n"); break;
      default                     :
        printf("Error return from yyparse() = %d\n", return_value );
    }
  }
}

#endif  /* YYDEBUG */

yyerror(char *s)
{
#ifdef  SRVOPT
  if( srvopt_function() )
    printf("FCT:yyerror(\"%s\")\n",s);
#endif
  log_error( memcmp( s, "yacc stack overflow", 19 ) == 0 ?
            EXPR_TOO_COMPLICATE : EXPR_SYNTAX_ERROR, NULL, line_no );
  exit( TMK_EXIT_FAILURE );
}

yywrap( Void )
{
#ifdef  SRVOPT
  if( srvopt_function() )
    printf("FCT:yywrap()\n");
#endif
  log_error( INTERNAL_EXPR_ERROR, NULL, line_no );
  exit( TMK_EXIT_FAILURE );
}

Static
Int32   isodigit ( Int32 c )
{
  Int32   rc;
#ifdef  SRVOPT
  if( srvopt_function() )
    printf("FCT:isodigit(%c)\n",c);
#endif
  rc      = '0' <= c && c <= '7';
#ifdef  SRVOPT
  if( srvopt_fctrtn() )
    printf("RTN:isodigit:%d\n",rc);
#endif
  return( rc );
}

Int32   yylex( Void )
{
  Int32   c;
  
#ifdef  SRVOPT
  if( srvopt_function() )
    printf("FCT:yylex()\n");
#endif
  while( bc && *bp == ' ' ) {
    --bc; ++bp;
  }
  
  if( bc == 0 )
    return ENDINPUT;
  
  c       = *bp;
  --bc; ++bp;
  
#if 0
#if     YYDEBUG
  switch( c ) {
    case 'E':       return( EXIT );         /* exit exp main pgm  */
    case 'D':       return( DEBUG );        /* toggle debug info  */
  }
#endif  /* YYDEBUG */
#endif
  if( isdigit(c) ) {
    if( yylval = ( c - '0' ) ) {
      while( bc && isdigit( c = *bp ) ) {
        yylval      *= 10;
        yylval      += c - '0';
        ++bp;       --bc;
      }
    } else {
      if( bc ) {
        c   = *bp;
        if( c == 'x' || c == 'X' ) {
          if( isxdigit( *( bp+1 ) ) ) {
            ++bp;       --bc;
            while( bc && isxdigit( c = *bp ) ) {
              yylval  *= 16;
              yylval  += isdigit(c) ? c - '0'
              : toupper(c) - 'A' + 10;
              ++bp;   --bc;
            }
            if( bc && !isspace( *bp ) ) {
              log_error( INV_HEX_NO, NULL, line_no );
              exit( TMK_EXIT_FAILURE );
            }
          } else {
            log_error( INV_HEX_NO, NULL, line_no );
            exit( TMK_EXIT_FAILURE );
          }
        } else {
          while( bc && isodigit( c = *bp ) ) {
            yylval      *= 8;
            yylval      += c - '0';
            ++bp;       --bc;
          }
          if( bc && isdigit( c ) ) {
            log_error( INV_OCTAL_NO, NULL, line_no );
            exit( TMK_EXIT_FAILURE );
          }
        }
      }
    }
    return( NUMBER );
  }
  
  switch( c ) {
    case '\n'       :       return ENDINPUT;
    case '('        :       return LEFTP;
    case ')'        :       return RIGHTP;
    case '+'        :       return ADD;
    case '-'        :       return SUB;
    case '*'        :       return MULT;
    case '/'        :       return DIV;
    case '%'        :       return MOD;
    case '~'        :       return CMPL;
    case '^'      :       return BXOR;
    case '&'        :
      if( bc && *bp == '&' ) {
        --bc; ++bp;
        return AND;
      }
      return BAND;
    case '!'        :
      if( bc && *bp == '=' ) {
        --bc; ++bp;
        return NEQ;
      }
      return NOT;
    case '='        :
      if( bc && *bp == '=' ) {
        --bc; ++bp;
        return EQ;
      }
      break;
    case '<'        :
      if( bc && *bp == '=' ) {
        --bc; ++bp;
        return LE;
      }
      if( bc && *bp == '<' ) {
        --bc; ++bp;
        return SHFLT;
      }
      return LT;
    case '>'        :
      if( bc && *bp == '=' ) {
        --bc; ++bp;
        return GE;
      }
      if( bc && *bp == '>' ) {
        --bc; ++bp;
        return SHFRT;
      }
      return GT;
    case '|'      :       /* vertical bar processing */
    check_more_or:
      if( bc && ( *bp == '|' ) ) {
        --bc; ++bp;
        return OR;
      } else {
        if( bc >= 3 && *bp == '?'
           && *(bp+1) == '?' && *(bp+2) == '!' ) {
          bc      -= 3;
          bp      += 3;
          return OR;
        }
      }
      return BOR;
    case '?'        :       /* looking for tri-graphs */
      if( bc >= 2 && *bp == '?' ) {
        int     found = 1;      /* found tr-graph flag */
        
        switch( *(bp+1) ) {
          case '!' :      bc      -= 2;
            bp      += 2;
            goto    check_more_or;
            
          case '=' :      c       = '#';        break;
          case '(' :      c       = '[';        break;
          case ')' :      c       = ']';        break;
          case '/' :      c       = '\\';         break;
          case '\'' :     c       = BXOR;         break;
          case '<' :      c       = '{';        break;
          case '>' :      c       = '}';        break;
          case '-' :      c       = CMPL;         break;
          default :       found   = 0;
        }
        if( found ) {
          bc  -= 2;
          bp  += 2;
        }
      }
  }
  if( c <= TOKEN_BASE ) {
    log_error( EXPR_SYNTAX_ERROR, NULL, line_no );
    exit( TMK_EXIT_FAILURE );
  }
  
#ifdef  SRVOPT
  if( srvopt_fctrtn() )
    printf("RTN:yylex:%d\n",c );
#endif
  return( c );
}
yytabelem yyexca[] ={
  -1, 1,
  0, -1,
  -2, 0,
};
# define YYNPROD 27
# define YYLAST 191
yytabelem yyact[]={
  
  14,    15,    11,    12,    13,    16,    17,    27,    28,    22,
  23,    21,    20,    19,    18,    25,    24,    26,     1,     0,
  10,    14,    15,    11,    12,    13,    16,    17,    27,    28,
  22,    23,    21,    20,    19,    18,    25,    24,    26,     0,
  52,    14,    15,    11,    12,    13,    16,    17,    27,     0,
  22,    23,    21,    20,    19,    18,    25,    24,    26,    14,
  15,    11,    12,    13,    16,    17,     0,     0,    22,    23,
  21,    20,    19,    18,    25,    24,    26,    14,    15,    11,
  12,    13,    16,    17,     0,     0,    22,    23,    21,    20,
  19,    18,     0,    24,    26,    14,    15,    11,    12,    13,
  16,    17,     0,     0,    22,    23,    21,    20,    19,    18,
  0,    24,    14,    15,    11,    12,    13,    16,    17,     0,
  0,    22,    23,    21,    20,    19,    18,    14,    15,    11,
  12,    13,    16,    17,    11,    12,    13,     0,    21,    20,
  19,    18,     9,     8,     4,     5,     6,     7,    14,    15,
  11,    12,    13,    16,    17,    14,    15,    11,    12,    13,
  0,     2,     0,     0,     3,    29,    30,    31,    32,    33,
  0,     0,     0,    34,    35,    36,    37,    38,    39,    40,
  41,    42,    43,    44,    45,    46,    47,    48,    49,    50,
  51 };
yytabelem yypact[]={
  
  -116, -1000,  -262,  -116,  -116,  -116,  -116,  -116, -1000, -1000,
  -1000,  -116,  -116,  -116,  -116,  -116,  -116,  -116,  -116,  -116,
  -116,  -116,  -116,  -116,  -116,  -116,  -116,  -116,  -116,  -241,
  -1000, -1000, -1000, -1000, -1000, -1000, -1000,  -130,  -130,  -107,
  -107,  -114,  -114,  -114,  -114,  -135,  -135,  -150,  -185,  -167,
  -203,  -221, -1000 };
yytabelem yypgo[]={
  
  0,    18,   161 };
yytabelem yyr1[]={
  
  0,     1,     2,     2,     2,     2,     2,     2,     2,     2,
  2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
  2,     2,     2,     2,     2,     2,     2 };
yytabelem yyr2[]={
  
  0,     5,     7,     7,     7,     7,     7,     7,     7,     7,
  7,     7,     7,     7,     7,     7,     7,     7,     7,     7,
  7,     5,     5,     5,     5,     2,     3 };
yytabelem yychk[]={
  
  -1000,    -1,    -2,   280,   260,   261,   262,   263,   259,   258,
  282,   264,   265,   266,   262,   263,   267,   268,   276,   275,
  274,   273,   271,   272,   278,   277,   279,   269,   270,    -2,
  -2,    -2,    -2,    -2,    -2,    -2,    -2,    -2,    -2,    -2,
  -2,    -2,    -2,    -2,    -2,    -2,    -2,    -2,    -2,    -2,
  -2,    -2,   281 };
yytabelem yydef[]={
  
  0,    -2,     0,     0,     0,     0,     0,     0,    25,    26,
  1,     0,     0,     0,     0,     0,     0,     0,     0,     0,
  0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
  21,    22,    23,    24,     3,     4,     5,     6,     7,     8,
  9,    10,    11,    12,    13,    14,    15,    16,    17,    18,
  19,    20,     2 };
typedef struct { char *t_name; int t_val; } yytoktype;
#ifndef YYDEBUG
#       define YYDEBUG  0       /* don't allow debugging */
#endif

#if YYDEBUG

yytoktype yytoks[] =
{
  "TOKEN_BASE",   257,
  "EXIT", 258,
  "NUMBER",       259,
  "CMPL", 260,
  "NOT",  261,
  "ADD",  262,
  "SUB",  263,
  "MULT", 264,
  "DIV",  265,
  "MOD",  266,
  "SHFLT",        267,
  "SHFRT",        268,
  "AND",  269,
  "OR",   270,
  "EQ",   271,
  "NEQ",  272,
  "GE",   273,
  "GT",   274,
  "LE",   275,
  "LT",   276,
  "BOR",  277,
  "BAND", 278,
  "BXOR", 279,
  "LEFTP",        280,
  "RIGHTP",       281,
  "ENDINPUT",     282,
  "UMINUS",       283,
  "UPLUS",        284,
  "UCMPL",        285,
  "UNOT", 286,
  "-unknown-",    -1      /* ends search */
};

char * yyreds[] =
{
  "-no such reduction-",
  "stat : expr ENDINPUT",
  "expr : LEFTP expr RIGHTP",
  "expr : expr MULT expr",
  "expr : expr DIV expr",
  "expr : expr MOD expr",
  "expr : expr ADD expr",
  "expr : expr SUB expr",
  "expr : expr SHFLT expr",
  "expr : expr SHFRT expr",
  "expr : expr LT expr",
  "expr : expr LE expr",
  "expr : expr GT expr",
  "expr : expr GE expr",
  "expr : expr EQ expr",
  "expr : expr NEQ expr",
  "expr : expr BAND expr",
  "expr : expr BOR expr",
  "expr : expr BXOR expr",
  "expr : expr AND expr",
  "expr : expr OR expr",
  "expr : CMPL expr",
  "expr : NOT expr",
  "expr : ADD expr",
  "expr : SUB expr",
  "expr : NUMBER",
  "expr : EXIT",
};
#endif /* YYDEBUG */
/* @(#)yaccpar  1.3  com/cmd/lang/yacc,3.1,9021 9/7/89 18:46:37 */
/*
 ** Skeleton parser driver for yacc output
 */

/*
 ** yacc user known macros and defines
 */
#ifdef YYSPLIT
#   define YYERROR      return(-2)
#else
#   define YYERROR      goto yyerrlab
#endif

#define YYACCEPT        return(0)
#define YYABORT         return(1)
#define YYBACKUP( newtoken, newvalue )\
{\
if ( yychar >= 0 || ( yyr2[ yytmp ] >> 1 ) != 1 )\
{\
yyerror( "syntax error - cannot backup" );\
goto yyerrlab;\
}\
yychar = newtoken;\
yystate = *yyps;\
yylval = newvalue;\
goto yynewstate;\
}
#define YYRECOVERING()  (!!yyerrflag)
#ifndef YYDEBUG
#       define YYDEBUG  1       /* make debugging available */
#endif

int yydebug;                    /* set to 1 to get debugging */

#define YYFLAG          (-1000)

#ifdef YYSPLIT
#   define YYSCODE { \
extern int (*yyf[])(); \
register int yyret; \
if (yyf[yytmp]) \
if ((yyret=(*yyf[yytmp])()) == -2) \
goto yyerrlab; \
else if (yyret>=0) return(yyret); \
}
#endif

YYSTYPE yyv[ YYMAXDEPTH ];      /* value stack */
int yys[ YYMAXDEPTH ];          /* state stack */

YYSTYPE *yypv;                  /* top of value stack */
YYSTYPE *yypvt;                 /* top of value stack for $vars */
int *yyps;                      /* top of state stack */

int yystate;                    /* current state */
int yytmp;                      /* extra var (lasts between blocks) */

int yynerrs;                    /* number of errors */
int yyerrflag;                  /* error recovery flag */
int yychar;                     /* current input token number */


int
yyparse()
{
  yypv = &yyv[-1];
  yyps = &yys[-1];
  yystate = 0;
  yytmp = 0;
  yynerrs = 0;
  yyerrflag = 0;
  yychar = -1;
  
  goto yystack;
  {
    register YYSTYPE *yy_pv;        /* top of value stack */
    register int *yy_ps;            /* top of state stack */
    register int yy_state;          /* current state */
    register int  yy_n;             /* internal state number info */
    
  yynewstate:
    yy_pv = yypv;
    yy_ps = yyps;
    yy_state = yystate;
    goto yy_newstate;
    
  yystack:
    yy_pv = yypv;
    yy_ps = yyps;
    yy_state = yystate;
    
  yy_stack:
#if YYDEBUG
    if ( yydebug )
    {
      register int yy_i;
      
      printf( "State %d, token ", yy_state );
      if ( yychar == 0 )
        printf( "end-of-file\n" );
      else if ( yychar < 0 )
        printf( "-none-\n" );
      else
      {
        for ( yy_i = 0; yytoks[yy_i].t_val >= 0;
             yy_i++ )
        {
          if ( yytoks[yy_i].t_val == yychar )
            break;
        }
        printf( "%s\n", yytoks[yy_i].t_name );
      }
    }
#endif /* YYDEBUG */
    if ( ++yy_ps >= &yys[ YYMAXDEPTH ] )    /* room on stack? */
    {
      yyerror( "yacc stack overflow" );
      YYABORT;
    }
    *yy_ps = yy_state;
    *++yy_pv = yyval;
    
  yy_newstate:
    if ( ( yy_n = yypact[ yy_state ] ) <= YYFLAG )
      goto yydefault;         /* simple state */
#if YYDEBUG
    yytmp = yychar < 0;
#endif
    if ( ( yychar < 0 ) && ( ( yychar = yylex() ) < 0 ) )
      yychar = 0;             /* reached EOF */
#if YYDEBUG
    if ( yydebug && yytmp )
    {
      register int yy_i;
      
      printf( "Received token " );
      if ( yychar == 0 )
        printf( "end-of-file\n" );
      else if ( yychar < 0 )
        printf( "-none-\n" );
      else
      {
        for ( yy_i = 0; yytoks[yy_i].t_val >= 0;
             yy_i++ )
        {
          if ( yytoks[yy_i].t_val == yychar )
            break;
        }
        printf( "%s\n", yytoks[yy_i].t_name );
      }
    }
#endif /* YYDEBUG */
    if ( ( ( yy_n += yychar ) < 0 ) || ( yy_n >= YYLAST ) )
      goto yydefault;
    if ( yychk[ yy_n = yyact[ yy_n ] ] == yychar )  /*valid shift*/
    {
      yychar = -1;
      yyval = yylval;
      yy_state = yy_n;
      if ( yyerrflag > 0 )
        yyerrflag--;
      goto yy_stack;
    }
    
  yydefault:
    if ( ( yy_n = yydef[ yy_state ] ) == -2 )
    {
#if YYDEBUG
      yytmp = yychar < 0;
#endif
      if ( ( yychar < 0 ) && ( ( yychar = yylex() ) < 0 ) )
        yychar = 0;             /* reached EOF */
#if YYDEBUG
      if ( yydebug && yytmp )
      {
        register int yy_i;
        
        printf( "Received token " );
        if ( yychar == 0 )
          printf( "end-of-file\n" );
        else if ( yychar < 0 )
          printf( "-none-\n" );
        else
        {
          for ( yy_i = 0;
               yytoks[yy_i].t_val >= 0;
               yy_i++ )
          {
            if ( yytoks[yy_i].t_val
                == yychar )
            {
              break;
            }
          }
          printf( "%s\n", yytoks[yy_i].t_name );
        }
      }
#endif /* YYDEBUG */
      {
        register int *yyxi = yyexca;
        
        while ( ( *yyxi != -1 ) ||
               ( yyxi[1] != yy_state ) )
        {
          yyxi += 2;
        }
        while ( ( *(yyxi += 2) >= 0 ) &&
               ( *yyxi != yychar ) )
          ;
        if ( ( yy_n = yyxi[1] ) < 0 )
          YYACCEPT;
      }
    }
    
    if ( yy_n == 0 )        /* have an error */
    {
      switch ( yyerrflag )
      {
        case 0:         /* new error */
          yyerror( "syntax error" );
          goto skip_init;
        yyerrlab:
          yy_pv = yypv;
          yy_ps = yyps;
          yy_state = yystate;
          yynerrs++;
        skip_init:
        case 1:
        case 2:
          yyerrflag = 3;
          while ( yy_ps >= yys )
          {
            yy_n = yypact[ *yy_ps ] + YYERRCODE;
            if ( yy_n >= 0 && yy_n < YYLAST &&
                yychk[yyact[yy_n]] == YYERRCODE)
            {
              yy_state = yyact[ yy_n ];
              goto yy_stack;
            }
#if YYDEBUG
#       define _POP_ "Error recovery pops state %d, uncovers state %d\n"
            if ( yydebug )
              printf( _POP_, *yy_ps,
                     yy_ps[-1] );
#       undef _POP_
#endif
            yy_ps--;
            yy_pv--;
          }
          YYABORT;
        case 3:         /* no shift yet; eat a token */
#if YYDEBUG
          if ( yydebug )
          {
            register int yy_i;
            
            printf( "Error recovery discards " );
            if ( yychar == 0 )
              printf( "token end-of-file\n" );
            else if ( yychar < 0 )
              printf( "token -none-\n" );
            else
            {
              for ( yy_i = 0;
                   yytoks[yy_i].t_val >= 0;
                   yy_i++ )
              {
                if ( yytoks[yy_i].t_val
                    == yychar )
                {
                  break;
                }
              }
              printf( "token %s\n",
                     yytoks[yy_i].t_name );
            }
          }
#endif /* YYDEBUG */
          if ( yychar == 0 )      /* reached EOF. quit */
            YYABORT;
          yychar = -1;
          goto yy_newstate;
      }
    }/* end if ( yy_n == 0 ) */
#if YYDEBUG
    if ( yydebug )
      printf( "Reduce by (%d) \"%s\"\n",
             yy_n, yyreds[ yy_n ] );
#endif
    yytmp = yy_n;                   /* value to switch over */
    yypvt = yy_pv;                  /* $vars top of value stack */
    {
      register int yy_len = yyr2[ yy_n ];
      
      if ( !( yy_len & 01 ) )
      {
        yy_len >>= 1;
        yyval = ( yy_pv -= yy_len )[1]; /* $$ = $1 */
        yy_state = yypgo[ yy_n = yyr1[ yy_n ] ] +
        *( yy_ps -= yy_len ) + 1;
        if ( yy_state >= YYLAST ||
            yychk[ yy_state =
                   yyact[ yy_state ] ] != -yy_n )
        {
          yy_state = yyact[ yypgo[ yy_n ] ];
        }
        goto yy_stack;
      }
      yy_len >>= 1;
      yyval = ( yy_pv -= yy_len )[1]; /* $$ = $1 */
      yy_state = yypgo[ yy_n = yyr1[ yy_n ] ] +
      *( yy_ps -= yy_len ) + 1;
      if ( yy_state >= YYLAST ||
          yychk[ yy_state = yyact[ yy_state ] ] != -yy_n )
      {
        yy_state = yyact[ yypgo[ yy_n ] ];
      }
    }
    /* save until reenter driver code */
    yystate = yy_state;
    yyps = yy_ps;
    yypv = yy_pv;
  }
  
  switch(yytmp){
      
    case 1:{        return( yypvt[-1] );            } /*NOTREACHED*/ break;
    case 2:{        yyval   = yypvt[-1];            } /*NOTREACHED*/ break;
    case 3:{        yyval   = yypvt[-2] * yypvt[-0];        } /*NOTREACHED*/ break;
    case 4:{
      if( yypvt[-0] == 0 ) {
        log_error( DIVIDED_BY_ZERO, NULL, line_no );
        exit( TMK_EXIT_FAILURE );
      }
      yyval   = yypvt[-2] / yypvt[-0];
    } /*NOTREACHED*/ break;
    case 5:{
      if( yypvt[-0] == 0 ) {
        log_error( DIVIDED_BY_ZERO, NULL, line_no );
        exit( TMK_EXIT_FAILURE );
      }
      yyval   = yypvt[-2] % yypvt[-0];
    } /*NOTREACHED*/ break;
    case 6:{        yyval   = yypvt[-2] + yypvt[-0];        } /*NOTREACHED*/ break;
    case 7:{        yyval   = yypvt[-2] - yypvt[-0];        } /*NOTREACHED*/ break;
    case 8:{
      yyval   = ( yypvt[-0] != 0 )
      ? yypvt[-2] << yypvt[-0]
      : yypvt[-2];
    } /*NOTREACHED*/ break;
    case 9:{
      yyval   = ( yypvt[-0] != 0 )
      ? yypvt[-2] >> yypvt[-0]
      : yypvt[-2];
    } /*NOTREACHED*/ break;
    case 10:{       yyval   = yypvt[-2] < yypvt[-0];        } /*NOTREACHED*/ break;
    case 11:{       yyval   = yypvt[-2] <= yypvt[-0];       } /*NOTREACHED*/ break;
    case 12:{       yyval   = yypvt[-2] > yypvt[-0];        } /*NOTREACHED*/ break;
    case 13:{       yyval   = yypvt[-2] >= yypvt[-0];       } /*NOTREACHED*/ break;
    case 14:{       yyval   = yypvt[-2] == yypvt[-0];       } /*NOTREACHED*/ break;
    case 15:{       yyval   = yypvt[-2] != yypvt[-0];       } /*NOTREACHED*/ break;
    case 16:{       yyval   = yypvt[-2] & yypvt[-0];        } /*NOTREACHED*/ break;
    case 17:{       yyval   = yypvt[-2] | yypvt[-0];        } /*NOTREACHED*/ break;
    case 18:{       yyval   = yypvt[-2] ^ yypvt[-0];      } /*NOTREACHED*/ break;
    case 19:{       yyval   = yypvt[-2] && yypvt[-0];       } /*NOTREACHED*/ break;
    case 20:{       yyval   = yypvt[-2] || yypvt[-0];       } /*NOTREACHED*/ break;
    case 21:{       yyval   = ~ yypvt[-0];          } /*NOTREACHED*/ break;
    case 22:{       yyval   = ! yypvt[-0];          } /*NOTREACHED*/ break;
    case 23:{       yyval   = yypvt[-0];            } /*NOTREACHED*/ break;
    case 24:{       yyval   = - yypvt[-0];          } /*NOTREACHED*/ break;
    case 26:{       return( -1 );           } /*NOTREACHED*/ break;
  }
  
  goto yystack;           /* reset registers in driver code */
}

