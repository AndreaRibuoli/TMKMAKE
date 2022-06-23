/* ================================================================= */
/*                                                                   */
/*  MODULE:    TMKHBASE                                              */
/*                                                                   */
/*                                                                   */
/*  MODULE-TYPE: C Library                                           */
/*                                                                   */
/*  Processor:  C                                                    */
/*                                                                   */
/*  Purpose:    Base data type definition - header file              */
/*                                                                   */
/* ================================================================= */

#ifndef _TMKHBASE_H
#define _TMKHBASE_H

/***********************************************************************
        Base type definitions for portability
***********************************************************************/
typedef unsigned char   Byte;         /* unsigned byte                */
typedef short           Boolean;      /* Boolean type                 */

typedef char            Char;         /* signed char                  */
typedef unsigned char   Uchar;        /* unsigned char                */

typedef char            Int8;         /* signed 8 bit integer         */
typedef unsigned char   Uint8;        /* unsigned 8 bit integer       */

typedef short           Int16;        /* signed 16 bit integer        */
typedef unsigned short  Uint16;       /* unsigned 16 bit integer      */

typedef long            Int32;        /* signed 32 bit integer        */
typedef unsigned long   Uint32;       /* unsigned 32 bit integer      */

typedef float           Float32;      /* 32 bit floating point        */
typedef double          Float64;      /* 64 bit floating point        */

#define Void            void
#define Static
#define Register        register


/***********************************************************************
        Base constant definitions
***********************************************************************/
#ifndef FALSE                         /* FALSE constant definitions   */
#define FALSE           0
#endif
#ifndef TRUE                          /* TRUE constant definitions    */
#define TRUE            (!FALSE)
#endif

#define TMK_EXIT_SUCCESS 0            /* success return code          */
#define TMK_EXIT_FAILURE 40           /* failure return code          */

#define PATH_SZ         33            /* path/file spec buffer size   */
#define WRKBUF_SZ       3010          /* working buffer size increment*/
#define INCLUDE_LVL     32            /* max level of nested include  */
#define CD_LVL          32            /* max level of nested directive*/

#define Max(a,b)        (((a)>(b))?(a):(b))
#define Min(a,b)        (((a)<(b))?(a):(b))

#endif  /* _TMKHBASE_H */

