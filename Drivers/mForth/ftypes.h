/* MIT License

Copyright (c) 2020 mnowell129
 
Charles M. Nowell Jr.
The Mouse Works, LLC 
mickeynowell@tmwfl.com 
 
Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all
copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
SOFTWARE.*/
/*
MickeyForth (mForth) forth interpreter.
copyright 1998-2011
*/

#ifndef FTYPES_H
#define FTYPES_H

#include <stdint.h>
#include <stdbool.h>
// to be found elsewhere, but don't want stdio.h included 
// because printf is redirected.
int sprintf(char *restrict s, const char *restrict format, ...);
// User must include this somewhere in the compile path
// see forthconfigsample.h for a template
#include "forthconfig.h"

#ifndef _SETJMP
   #include <setjmp.h>
#endif

typedef float  Float32;
typedef double Float64;
typedef signed long long int   Int64;
typedef signed int Fixed32;
typedef signed long long int Fixed64;
typedef void*  PVOID;
typedef void* (*THREAD_FUNC_PTR)(void*);
typedef int   (*USER_CALLBACK_FUNCPTR) (void*);    
typedef void  (*VOID_FUNC_PTR) (void);
typedef int   (*INT_FUNC_PTR) (void);  
// This is a special type name to make things obvious
typedef char const * const CountedString;

typedef const char *StringConstant;
typedef  char *String;

typedef String *Strings;

typedef CountedString *CountedStrings;

typedef uint8_t Ordinal;

typedef union
{
    int32_t    int32Value;
    Float32    float32Value;
    String     stringValue;
    Fixed32    fixed32Value;
    Fixed64    fixed64Value;
    Float64    float64Value;
    int32_t    *int32Ptr;
    uint32_t   *uint32Ptr;
    uint8_t    *uint8Ptr;
    Float32    *float32Ptr;
    void      *voidPtr;
}Floater;


#define GLOBAL_DOS
#undef  GLOBAL_DOS

#define RELATIVE
//#undef RELATIVE


#define PTR_SIZE 4
#if PTR_SIZE==2
typedef struct UserState *UserStatePtr;
typedef void  (*WordPtr)(UserStatePtr user);
typedef uint16_t *CellPtr;
typedef uint16_t  Cell;
typedef int16_t   SignedCell;
   #define CELL 2
   #define CELL_ALIGN_MASK 0x01
#else
typedef struct UserState *UserStatePtr;
typedef void  (*WordPtr)(UserStatePtr user);
typedef WordPtr CellPtr;
typedef int32_t  SignedCell;
typedef uint32_t Cell;
typedef uint16_t HalfCell;
typedef int16_t  SignedHalfCell;
   #define CELL 4
   #define CELL_ALIGN_MASK 0x03
#endif


/* the definition of a word pointer */
/* i.e. a forth token */
//typedef struct UserState;
//typedef struct UserState *UserStatePtr;

typedef void AssemblyWord(void);
typedef void ForthWord(void);
//typedef  enum{CODE,COLON,VARIABLE,CONSTANT,USER}WordType;
#define  IMMEDIATE    BIT7_OF_8
// note ignore compile only words
// so the user must me cognizant that the word
// is compile only.
// frees up one bit for length so length of strings can change
// from 64 to 128
// #define  COMPILE_ONLY BIT6_OF_8
#define  LENGTH_MASK 0x7f



typedef struct {
   WordPtr   code;        /* pointer to the code for this word */
   char      *text;        /* pointer to the actual 0 terminated text */
   #ifdef USE_HELP
   char      *help;
   #endif
}NameType;

/* for variables */
/* variable space is */
/* a chunk of ram where each holds a pointer to it */
/* user HERE to keep up with it , etc */
/* declaration words need a place to add themselves */

#define CODE_SPACE_SIZE      0x80
#define STACK_SIZE           64 // 2048
#define RETURN_STACK_SIZE    64


typedef uint8_t* (*MallocType)(int16_t sizeRequested);
typedef void  (*FreeType)(uint8_t *blockToFree);
typedef const char* (*ExternalStartup)(UserStatePtr user);
#ifndef PRINTF_TYPE
   #define PRINTF_TYPE
typedef void  (*PrintfType)(char *format,...);
#endif
typedef void  (*PutSType)(const char *string);
typedef char* (*GetsType)(char *buffer);
typedef char* (*GetsVector)(UserStatePtr user);
typedef void  (*PutchType)(char key);
typedef int   (*GetchType)(void);
typedef uint8_t  (*KeyHitType)(void);
typedef void  (*SetEchoType)(uint8_t);


typedef enum {
   FORTH_IP,
   RP,
   SP,
   SP0,
   RP0,
   IPTEMP0,
   IPTEMP1,
   IPTEMP2,
   IPTEMP3,
   PRINTF,
   PUTS,
   PUTCH,
   GETCH,
   KEYHIT,
   GETS,
   GETS_VECTOR,
   SET_ECHO_FUNCTION,
   MALLOC,
   FREE,
   // end of named variables
   STATE,
   CSP,
   CURRENT,
   HANDLER,
   TICKEVAL,
   BASE,
   PTIB,
   PPAD,
   TO_IN,
   HLD,
   QUIET,
   /* SEARCHABLE,*/ /* this is a bit mask that determines whether or not
                  a wordlist is searchable when compiling
                  maybe a future feature */
   WORDLISTS,
   DEFINES, // this must be here so that the cfasearch routine can back over it
   WORDLIST0, // this for core words
   WORDLIST1, // this for the few compiled core words
   WORDLIST2, // this for user's specific words
   WORDLIST3,
   WORDLIST4,
   WORDLIST5,
   USER_ID,  // identifies which user this is Root (serial), mib, host, hdp
   START_STRING,
   //      FILEGETS,
//      FILEHANDLE,
//      QUEUE,
//      QUEUEARRAY0,
//      QUEUEARRAY1,
//      QUEUEARRAY2,
//      QUEUEARRAY3,
   SEMAPHORE,    // semaphore that our thread can wait on, cast to pointer of local type
   SEMACALLBACK,  // pointer to function that meets local prototype to call back to signal the semaphore
   SUBSCRIBEPTR,
   OUTPUTPTR,     // pointer to a function that outputs variables on this interface
//      RECORD,       // hold our current file record number
//      LASTRECORD,   // last record to read if we don't hit EOF first
   OLD_GETS0,
   OLD_GETS1,
   OLD_GETS2,
   ECHO,
   ECHOZERO,
//      READY_FOR_INPUT,  /* a pointer to a function, that if not null, is called in dotPrompt */
   /* indicates that the last query input has been processed */
   /* used by block devices so that at the end of a bunch of output the block device */
   /* can send the accumulated output */
   ARRAY_INDEX,
   TIB_SIZE,
   PAD_SIZE,
   ABORT,
   BUFFERHEAD,
   BUFFERTAIL,
   STRING_TEMP  // a buffer for run time strings storage, temporary

}NamedUserVariables;

#define MAX_WORDLISTS  (WORDLIST5-WORDLIST0+1)
//     SP0,RP0,UP0,TICKKEY,TICKEMIT,TICKEXPECT,TICKTAP,
//   TICKECHO,TICKPROMPT,BASE,TMP,SPAN,TO_IN,PTIB,
//   TIB,CSP,TICKEVAL,TICKNUMBER,HLD,HANDLER,CP,NP,LAST}UserVars;




typedef struct NamedVariables {
   WordPtr              volatile *ip;  /* instruction pointer */
   WordPtr              **volatile rp;  /* return pointer */
   Cell                 *volatile stackPtr;
   Cell                 *stackStart;
   WordPtr              **rpStart; //  = returnStack;
   WordPtr              ipTemp[4];  /* word to ip to point to initially */
   PrintfType           userPrintf;
   PutSType             userPuts;
   PutchType            userPutch;
   GetchType            userGetch;
   KeyHitType           userKeyHit;
   GetsType             userGets;
   GetsVector           getsVector;
   SetEchoType          setEcho;
   MallocType           forthMallocPtr;
   FreeType             forthFreePtr;
   Cell               state;
   Cell               csp;
   Cell               current;
   Cell               handler;
   Cell               tickeval;
   Cell               base;
   Cell               ptib;
   Cell               ppad;
   Cell               to_in;
   Cell               hld;
   Cell               quiet;
   Cell               wordlists;
   Cell               defines;
   Cell               wordlist0;
   Cell               wordlist1;
   Cell               wordlist2;
   Cell               wordlist3;
   Cell               wordlist4;
   Cell               wordlist5;
   Cell               user_id;
   Cell               start_string;
   // Cell               filehandle;
   // Cell               queue;
   // Cell               queuearray0;
   // Cell               queuearray1;
   // Cell               queuearray2;
   // Cell               queuearray3;
   Cell               semaphore;
   Cell               semaCallback;
   Cell               subscribePtr;
   Cell               outputPtr;
   // Cell               record;
   // Cell               lastrecord;
   Cell               old_gets0;
   Cell               old_gets1;
   Cell               old_gets2;
   Cell               echo;
   Cell               echoZero;
//    Cell               readyForInput;
   Cell               arrayIndex;
   Cell               tibSize;
   Cell               padSize;
   Cell               abort;
   Cell               bufferHead; // linked list of buffers for this context
   Cell               bufferTail;
   Cell               string_temp[1];
}NamedVariables;

// The tib is very large to allow for a full
// 256 byte message from the acoustic
// That may be one big strings
// modified to 128 to save space.
// Pad is just used to format numbers
// also 80 is unreasonable
// reduce to 40
// Make the temp buffer bigger to allow for interpreted arrays
// Perhaps later fold the pad and tempstring together
// to save space
#define TIB_SIZE (1024+80)
//#define PAD_SIZE 81
// #define PAD_SIZE (1024/4)  // cells = 128 bytes
//#define TEMP_SIZE 512  // room for 160 characters or 40 int32's

typedef struct UserState {
   jmp_buf     jumper;
   // char        tibBuffer[TIB_SIZE];
   char        *tibBuffer;
   // make the padbuffer do double duty
   // as both pad, tempstring and arrays
   // Cell        padBuffer[PAD_SIZE];
   Cell        *padBuffer;
   //    Cell      tempString[TEMP_SIZE];
   Cell        stack[STACK_SIZE+RETURN_STACK_SIZE];
   WordPtr     doColon;
   WordPtr     doVariable;
   WordPtr     doConstant;
   //    WordPtr doUser;
   //    WordPtr doDoes; /* maybe */
   //    WordPtr doDefine; /* maybe */
   WordPtr  dollarCompile;
   WordPtr  dollarInterpret;
   WordPtr  dollarArray;
   union {
      NamedVariables   namedVariables;
      Cell           indexedVariables[STRING_TEMP+1]; // just needs to be bigger than the
      // number of named variables
   }userVariables;
}UserState;

//typedef void  (*WordPtr)(UserStatePtr user);


typedef struct {
   UserState           userState;
   WordPtr             initFunction;
   ExternalStartup     externalStartup;
}Context;


//#define NUM_USERS 4 // some small value right now

//extern Context   users[NUM_USERS];
#define ROOT_USER  0 // Uart
#define USER_ONE   1 // MIB
#define USER_TWO   2 // Host
#define USER_THREE 3 // UDP

// NOTE : u MUST be a scalar pointer variable to userState
#define USER_VAR(u,v)   (u->userVariables.indexedVariables[v])

#define PUSH(u,x)   push((u),(Cell)(x))
#define POP(u)      pop((u))

#define UPUSH(x)      push(user,(Cell)(x))
#define UPOP()        pop(user)
#define UDEPTH()      depth(user)

// define some error codes for
// longjump to send with stack problems
//
#define STACK_EMPTY          1
#define RETURN_STACK_EMPTY   2
#define BASE_0               3
#define PACKDOLLAR           4
#define DOLLARCOMMA          5

uint8_t addWord(UserStatePtr user,WordPtr function,char *name);


#define FCONTEXT  UserStatePtr user


#define LONG_STRING_COUNT
//#undef LONG_STRING_COUNT


#ifdef M3FIX
// fixup for cortex m3
   #define FUNCTION(a)  ((uint8_t *)(((int)a) & (-2)))
#else
   #define FUNCTION(a)  ((uint8_t *)a)
#endif


#define ROM_END   0x0003FFFF
#define PERIPHERAL_START 0x2009C000

#endif
