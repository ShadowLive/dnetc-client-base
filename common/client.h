// Hey, Emacs, this a -*-C++-*- file !

// Copyright distributed.net 1997-1998 - All Rights Reserved
// For use in distributed.net projects only.
// Any other distribution or use of this source violates copyright.
//
// $Log: client.h,v $
// Revision 1.60  1998/07/05 13:44:10  cyruspatel
// Fixed an inadvertent wrap of one of the long single-line revision headers.
//
// Revision 1.59  1998/07/05 13:09:04  cyruspatel
// Created new pathwork.cpp which contains functions for determining/setting
// the "work directory" and pathifying a filename that has no dirspec.
// GetFullPathForFilename() is ideally suited for use in (or just prior to) a
// call to fopen(). This obviates the neccessity to pre-parse filenames or
// maintain separate filename buffers. In addition, each platform has its own
// code section to avoid cross-platform assumptions. More doc in pathwork.cpp
// #define DONT_USE_PATHWORK if you don't want to use these functions.
//
// Revision 1.58  1998/07/05 12:42:37  cyruspatel
// Created cpucheck.h to support makefiles that rely on autodependancy info
// to detect file changes.
//
// Revision 1.57  1998/07/04 21:05:34  silby
// Changes to lurk code; win32 and os/2 code now uses the same variables, and has been integrated into StartLurk and LurkStatus functions so they now act the same.  Additionally, problems with lurkonly clients trying to connect when contestdone was wrong should be fixed.
//
// Revision 1.56  1998/07/02 13:09:28  kbracey
// A couple of RISC OS fixes - printf format specifiers made long.
// Changed a "blocks" to "block%s", n==1?"":"s".
//
// Revision 1.55  1998/07/01 03:29:02  silby
// Added prototype for CheckForcedKeyproxy (used in cliconfig)
//
// Revision 1.54  1998/06/29 08:43:59  jlawson
// More OS_WIN32S/OS_WIN16 differences and long constants added.
//
// Revision 1.53  1998/06/29 07:51:54  ziggyb
// OS/2 lurk and DOD header additions. (platforms\os2cli\dod.h)
//
// Revision 1.52  1998/06/29 06:57:43  jlawson
// added new platform OS_WIN32S to make code handling easier.
//
// Revision 1.51  1998/06/28 23:40:23  silby
// Changes to path handling code so that path validation+adding to filenames will be more reliable (especially on win32).
//
// Revision 1.50  1998/06/25 04:43:32  silby
// Changes to Internalgetfilename for win32 (+ other platforms in the future) to make path handling better (now it won't miss / and : on win32)
//
// Revision 1.49  1998/06/25 03:02:32  blast
// Moved the version #defines from client.h to version.h and added a version
// string called CLIENT_VERSIONSTRING...
//
// Revision 1.48  1998/06/25 02:30:38  jlawson
// put back public Client::connectrequested for use by win32gui
//
// Revision 1.47  1998/06/24 16:24:05  cyruspatel
// Added prototype for GetTimesliceBaseline() (in cpucheck.cpp)
//
// Revision 1.46  1998/06/24 15:54:18  jlawson
// added pragma pack(1) around structures.
//
// Revision 1.45  1998/06/22 19:42:01  daa
// bump to 2.7025.410 this is likely to become 2.7100.411
//
// Revision 1.44  1998/06/22 01:04:56  cyruspatel
// DOS changes. Fixes various compile-time errors: removed extraneous ')' in
// sleepdef.h, resolved htonl()/ntohl() conflict with same def in client.h
// (is now inline asm), added NONETWORK wrapper around Network::Resolve()
//
// Revision 1.43  1998/06/21 17:10:26  cyruspatel
// Fixed some NetWare smp problems. Merged duplicate numcpu validation code
// in ::ReadConfig()/::ValidateConfig() into ::ValidateProcessorCount() and
// spun that off, together with what used to be ::x86id() or ::ArmId(), into
// cpucheck.cpp. Adjusted and cleaned up client.h accordingly.
//
// Revision 1.42  1998/06/15 09:17:57  jlawson
// removed include of pthread.h since threadcd.h knows when to include it
//
// Revision 1.41  1998/06/14 11:22:37  ziggyb
// Fixed the OS/2 headers and added an os2defs.h and adjusted for the
// separate sleep defines header.
//
// Revision 1.40  1998/06/14 08:12:36  friedbait
// 'Log' keywords added to maintain automatic change history
//
//

// For WinNT Service:
//#define WINNTSERVICE "bovrc5nt"
// For Win32 hidden console:
//#define WIN32HIDDEN
// For GUIs and such
//#define NOMAIN

#define NEW_STATS_AND_LOGMSG_STUFF    //if you want a 'better looking' screen
#define NEW_LOGSCREEN_PERCENT_SINGLE  //if the percbar is to stay < 80 chars
//#define PERCBAR_ON_ONE_LINE         //percents for all threads on 1 line


#ifndef CLIENT_H
#define CLIENT_H

// --------------------------------------------------------------------------

// all revision comments moved to the changeLog.txt

#include "version.h"
#include "cputypes.h"
#include "problem.h"
#include "iniread.h"
#include "network.h"
#include "scram.h"
#include "mail.h"
#include "convdes.h"
#include "sleepdef.h"
#include "threadcd.h"
#include "cpucheck.h"

#define OPTION_SECTION "parameters"

#define PACKET_VERSION      0x03

#define MAXCPUS             16
#define BUFFER_RETRY        10
#define FETCH_RETRY         10
#define NETTIMEOUT          60

// --------------------------------------------------------------------------

#if ((CLIENT_OS == OS_AMIGAOS) || (CLIENT_OS == OS_RISCOS))
extern "C" {
#endif

#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <signal.h>
#include <stdarg.h>
#include <string.h>
#include <sys/types.h>

#if ((CLIENT_OS == OS_AMIGAOS) || (CLIENT_OS == OS_RISCOS))
}
#endif

#if (CLIENT_OS == OS_IRIX)
  #include <limits.h>
  #include <sys/types.h>
  #include <sys/prctl.h>
  #include <sys/schedctl.h>
  #include <fcntl.h>
#elif (CLIENT_OS == OS_OS2)
  #include <sys/timeb.h>
  #include <conio.h>
  #include <share.h>
  #include <direct.h>
  #include <fcntl.h>
  #include "platforms/os2cli/os2defs.h"
  #ifndef QSV_NUMPROCESSORS       /* This is only defined in the SMP toolkit */
    #define QSV_NUMPROCESSORS     26
  #endif
#elif (CLIENT_OS == OS_RISCOS)
  extern "C"
  {
    #include <sys/fcntl.h>
    #include <unistd.h>
    #include <stdarg.h>
    #include <machine/endian.h>
    #include <swis.h>
    extern unsigned int ARMident(), IOMDident();
    extern void riscos_clear_screen();
    extern bool riscos_check_taskwindow();
    extern int riscos_find_local_directory(const char *argv0);
    extern char *riscos_localise_filename(const char *filename);
    extern int getch();

    #define fileno(f) ((f)->__file)
    #define isatty(f) ((f) == 0)
  }
  extern s32 guiriscos, guirestart;
  extern bool riscos_in_taskwindow;
#elif (CLIENT_OS == OS_VMS)
  #include <fcntl.h>
  #include <types.h>
  #define unlink remove
#elif (CLIENT_OS == OS_SCO)
  #include <fcntl.h>
  #include <sys/time.h>
#elif (CLIENT_OS == OS_WIN16)
  #include <sys/timeb.h>
  #include <io.h>
  #include <conio.h>
  #include <dos.h>
  #include <share.h>
  #include <dir.h>
  #include <fcntl.h>
#elif (CLIENT_OS == OS_WIN32) || (CLIENT_OS == OS_WIN32S)
  #include <sys/timeb.h>
  #include <process.h>
  #include <ras.h>
  #include <conio.h>
  #include <share.h>
  #include <fcntl.h>
  #ifdef __TURBOC__
    #include <dir.h>
  #endif
  #if (CLIENT_OS == OS_WIN32)
    typedef DWORD (CALLBACK *rasenumconnectionsT)(LPRASCONN, LPDWORD, LPDWORD);
    typedef DWORD (CALLBACK *rasgetconnectstatusT)(HRASCONN, LPRASCONNSTATUS);
    extern rasenumconnectionsT rasenumconnections;
    extern rasgetconnectstatusT rasgetconnectstatus;
  #endif
#elif (CLIENT_OS == OS_DOS)
  #include <sys/timeb.h>
  #include <io.h>
  #include <conio.h>
  #include <share.h>
  #include <fcntl.h>
  #include "platforms/dos/clidos.h"
#elif (CLIENT_OS == OS_BEOS)
// nothing  #include <share.h>
  #include <fcntl.h>
#elif (CLIENT_OS == OS_NETWARE)
  #include <sys/time.h>
  #include <process.h>
  #include <conio.h>
  #include <direct.h>
  #include <share.h>
  #include <fcntl.h>
  #include "platforms/netware/netware.h" //for stuff in netware.cpp
#elif (CLIENT_OS == OS_SUNOS) || (CLIENT_OS == OS_SOLARIS)
  #include <fcntl.h>
  extern "C" int nice(int);
  extern "C" int gethostname(char *, int); // Keep g++ happy.
#endif

// --------------------------------------------------------------------------

#ifdef max
#undef max
#endif
#define max(a,b)            (((a) > (b)) ? (a) : (b))

#ifdef min
#undef min
#endif
#define min(a,b)            (((a) < (b)) ? (a) : (b))

// --------------------------------------------------------------------------

#ifdef DONT_USE_PATHWORK
  #if (CLIENT_OS == OS_NETWARE)
  //#define PATH_SEP   "\\"   //left undefined so I can see
  //#define PATH_SEP_C '\\'   //where the references are
  #define EXTN_SEP   "."
  #define EXTN_SEP_C '.'
  #elif ((CLIENT_OS == OS_DOS) || CLIENT_OS == OS_WIN32) || (CLIENT_OS == OS_WIN32S) || (CLIENT_OS == OS_WIN16) || (CLIENT_OS == OS_OS2)
  #define PATH_SEP   "\\"
  #define PATH_SEP_C '\\'
  #define ALT_PATH_SEP '/'
  #define ALT_PATH_SEP_C '/'
  #define DRIVE_SEP ':'
  #define DRIVE_SEP_C ':'
  #define EXTN_SEP   "."
  #define EXTN_SEP_C '.'
  #elif (CLIENT_OS == OS_MACOS)
  #define PATH_SEP   ":"
  #define PATH_SEP_C ':'
  #define EXTN_SEP   "."
  #define EXTN_SEP_C '.'
  #elif (CLIENT_OS == OS_RISCOS)
  #define PATH_SEP   "."
  #define PATH_SEP_C '.'
  #define EXTN_SEP   "/"
  #define EXTN_SEP_C '/'
  #else
  #define PATH_SEP   "/"
  #define PATH_SEP_C '/'
  #define EXTN_SEP   "."
  #define EXTN_SEP_C '.'
  #endif
#else
  #if (CLIENT_OS == OS_NETWARE)
  #define EXTN_SEP   "."
  #define EXTN_SEP_C '.'
  #elif ((CLIENT_OS == OS_DOS) || CLIENT_OS == OS_WIN32) || (CLIENT_OS == OS_WIN32S) || (CLIENT_OS == OS_WIN16) || (CLIENT_OS == OS_OS2)
  #define EXTN_SEP   "."
  #define EXTN_SEP_C '.'
  #elif (CLIENT_OS == OS_MACOS)
  #define EXTN_SEP   "."
  #define EXTN_SEP_C '.'
  #elif (CLIENT_OS == OS_RISCOS)
  #define EXTN_SEP   "/"
  #define EXTN_SEP_C '/'
  #else
  #define EXTN_SEP   "."
  #define EXTN_SEP_C '.'
  #endif
#endif

// --------------------------------------------------------------------------

typedef enum
{
  OP_REQUEST,
  OP_DATA,
  OP_SUCCESS,
  OP_DONE,
  OP_FAIL,
  OP_MAX,
  OP_PERMISSION = 42,
  OP_DONE_NOCLOSE,
  OP_DONE_NOCLOSE_ACK,
  OP_PROXYCHANGE,
  OP_PERREQUEST = 50,
  OP_PERDATA,
  OP_SCRAMREQUEST = 69,
  OP_SCRAM = 96,
  OP_SUCCESS_ACK,
  OP_COMPRESSED_FOLLOWS = 100,
  OP_REQUEST_MULTI,
  OP_SUCCESS_MULTI,
  OP_DONE_MULTI,
  OP_PERREQUEST_MULTI
} Operation;

// --------------------------------------------------------------------------

typedef enum
{
  IDCONTEST_ANY = 0,
  IDCONTEST_DES = 1,       /* obsolete */
  IDCONTEST_RC556,         /* obsolete */
  IDCONTEST_RC564,
  IDCONTEST_DESII
} contest_id_t;

// --------------------------------------------------------------------------

#pragma pack(1)               // no padding allowed

// remember that for now, the u64's come from the server in the "wrong" order
// lo|hi, not hi|lo like they should
// - everything in network byte order.

typedef struct Packet
{
  u32  op;            // operation code                     }--|    }--|
  u64  key;           // the Key starting point                |       |
  u64  iv;            // the IV                                |       |
  u64  plain;         // the Plaintext                         |       |
  u64  cypher;        // the Cyphertext                        |       |
  u32  iterations;    // number of iterations (the low 32bits) |       |
  char id[64];        // identifier (email address)            |       |
  u32  ip;            // IP Address (proxy filled)             |       |
  char other[24];     // extra space                           |       |
  u32  rc564contestdone; //  set to htonl(0xBEEFF00D) by the proxies if the rc564 contest is finished
  u32  descontestdone;   //   set to htonl(0xBEEFF00D) by the proxies if current des contest is over
  u32  contestid;     // contest identifier                    |       |
  u32  build;         // build number                          |       |
  u32  os;            // OS - see defines                      |       |
  u32  cpu;           // CPU - see defines                     |       |
  u32  version;       // currently set to = 0x00000002      }--|       |
  u32  checksum;      // pre scrambled checksum            ----|    }--|
  u32  scramble;      // the key we're using to scrambling this -------|
} Packet;

// --------------------------------------------------------------------------

typedef struct
{
  u32 lock;
  u32 count;
} FileHeader;

// --------------------------------------------------------------------------

// note this begins exactly as a ContestWork struct,
// so that we can just pass it to LoadWork.

typedef struct
{
  u64  key;               // starting key
  u64  iv;                // initialization vector
  u64  plain;             // plaintext we're searching for
  u64  cypher;            // cyphertext
  u64  keysdone;          // iterations done (also current position in block)
  u64  iterations;        // iterations to do
  u32  op;                // (out)OP_SUCCESS, (out)OP_DONE, or (in)OP_DATA
  char id[59];            // email address of original worker...
  u8   contest;           //
  u8   cpu;               // added 97.11.25
  u8   os;                // added 97.11.25
  u8   buildhi;           // added 97.11.25
  u8   buildlo;           // added 97.11.25
  u32  checksum;          // checksum for file curruption
  u32  scramble;          // scramble key for this entry (NOT! the same as OP_KEY)
} FileEntry;

#pragma pack()

// --------------------------------------------------------------------------

class Client
{
public:
  char exename[64];
  char exepath[128];
  char inifilename[128];
  char id[64];
  s32  inthreshold[2];
  s32  outthreshold[2];
  s32  blockcount;
  char hours[64];
  s32  minutes;
  s32  timeslice;
  s32  niceness;
  char keyproxy[64];
  s32  keyport;
  char httpproxy[64];
  s32  httpport;
  s32  uuehttpmode;
  char httpid[128];
  s32  cputype;
  s32  messagelen;
  char smtpsrvr[128];
  s32  smtpport;
  char smtpfrom[128];
  char smtpdest[128];
  s32  offlinemode;

#ifndef DONT_USE_PATHWORK
  char ini_logname[128];// Logfile name as is in the .ini
  char ini_in_buffer_file[2][128];
  char ini_out_buffer_file[2][128];
  char ini_exit_flag_file[128];
  char ini_checkpoint_file[2][128];
  char ini_pausefile[128];
#endif
  char logname[128];// Logfile name as used by the client
  char in_buffer_file[2][128];
  char out_buffer_file[2][128];
  char exit_flag_file[128];
  char checkpoint_file[2][128];
  char pausefile[128];

  MailMessage mailmessage;
  s32  numcpu, numcputemp;
  s32 checkpoint_min;
  s32 percentprintingoff;
  s32 connectoften;
  s32 nodiskbuffers;
  s32 membuffcount[2][2];
  FileEntry *membuff[2][500][2];
  s32 nofallback;
  s32 randomprefix;
  s32 randomchanged;
  s32 consecutivesolutions[2];
  s32 quietmode;
  s32 nonewblocks;
  s32 nettimeout;
  s32 noexitfilecheck;
  s32 exitfilechecktime;
#if ( ((CLIENT_OS == OS_OS2) || (CLIENT_OS == OS_WIN32)) && defined(MULTITHREAD) )
  s32 lurk;
  s32 oldlurkstatus;
#endif
#if ( (CLIENT_OS==OS_WIN32) && (!defined(WINNTSERVICE)) )
  s32 win95hidden;
#endif

#if (CLIENT_OS == OS_OS2)
  s32 os2hidden;
//  s32 connectstatus;          // 0 is not connected, 1 is connected
#endif
  s32 preferred_contest_id;  // 0 for RC564, 1 for DESII (unlike config)
  s32 preferred_blocksize;
  s32 contestdone[2];

protected:
  char proxymessage[64];
  char logstr[1024];

  ContestWork contestwork;
  RC5Result rc5result;

  u32 totalBlocksDone[2];
  u32 old_totalBlocksDone[2];
  u32 timeStarted;


#if (CLIENT_OS == OS_WIN32) && defined(NEEDVIRTUALMETHODS)
  u32 connectrequested;       // used by win32gui to signal an update
#endif


#if (CLIENT_OS == OS_WIN16) || (CLIENT_OS == OS_WIN32S)
  virtual void SurrenderCPU( void ) {};
    // pause for other tasks
#endif

#ifndef NEW_STATS_AND_LOGMSG_STUFF //Substituted by CliTimeString(NULL,1)
  const char * Time( void );
    // Returns: string with GMT time in "mm/dd/yy hh:mm:ss"
#endif

  void RandomWork( FileEntry * data );
    // returns a random block.

  // these put/get data blocks to the file buffers
  // and trigger Fetch/Flush when needed
  s32  PutBufferInput( const FileEntry * data );  // hidden
    // dump an entry into in buffer
    // Returns: -1 on error, otherwise number of entries now in file.

  s32  GetBufferInput( FileEntry * data, u8 contest );        // "user"
    // get work from in buffer
    // Returns: -1 on error, otherwise number of blocks now in file.

  s32  CountBufferInput(u8 contest);
    // returns number of blocks currently in input buffer

  s32  PutBufferOutput( const FileEntry * data );  // "user"
    // dump an entry into output buffer
    // Returns: -1 on error, otherwise number of entries now in file.

  s32  GetBufferOutput( FileEntry * data , u8 contest);       // hidden
    // get work from output buffer
    // Returns: -1 on error, otherwise number of blocks now in file.

  s32  CountBufferOutput(u8 contest);
    // returns number of blocks currently in output buffer

  s32 InternalPutBuffer( const char *filename, const FileEntry * data );
  s32 InternalGetBuffer( const char *filename, FileEntry * data, u32 *optype , u8 contest);
  s32 InternalCountBuffer( const char *filename , u8 contest);
  const char *InternalGetLocalFilename( const char *filename );
  s32 EnsureBufferConsistency( const char *filename );

#if ( ((CLIENT_OS == OS_OS2) || (CLIENT_OS == OS_WIN32)) && defined(MULTITHREAD) )
s32 StartLurk(void);// Initializes Lurk Mode
  // 0 == Successfully started lurk mode
  // -1 == Start of lurk mode failed
s32 LurkStatus(void);// Checks status of connection
  // 0 == not currently connected
  // 1 == currently connected 
#endif

#if defined(NEEDVIRTUALMETHODS)
  // methods that can be overriden to provide additional functionality
  // virtuals required for OS2 & win GUI clients...
  virtual void InternalReadConfig( IniSection &ini ) {};
  virtual void InternalValidateConfig( void) {};
  virtual void InternalWriteConfig( IniSection &ini ) {};
#endif

  bool CheckForcedKeyport(void);
  bool CheckForcedKeyproxy(void);

  void SetNiceness(void);
    // set the client niceness

public:
  Client();
  ~Client();

  void PrintBanner(const char * clname);
    // prints out a version banner to screen

  void ParseCommandlineOptions(int Argc, char *Argv[], s32 &inimissing);
    // parses commandline options, setting parsed items to NULL

  s32 CkpointToBufferInput(u8 contest);
    // Copies info in checkpint file back to input buffer

  void DoCheckpoint( int load_problem_count );
    // Make the checkpoint file represent current blocks being worked on

  void DisplayHelp( const char * unrecognized_option );
    // Displays the interactive command line help screen.

  void Log( const char *format, ... );
    // logs message to screen and file (append mode)
    // if logname isn't set, then only to screen

  void LogScreenf( const char *format, ... );
    // logs message to screen only

#if defined(NEEDVIRTUALMETHODS)
  virtual void LogScreen ( const char *text );
    // logs preformated message to screen only.  can be overriden.

  virtual void LogScreenPercentSingle(u32 percent, u32 lastpercent, bool restarted);

  virtual void LogScreenPercentMulti(u32 cpu, u32 percent, u32 lastpercent, bool restarted);
    // progress percentage printing to screen only.

  virtual s32  Configure( void );
    // runs the interactive configuration setup
#else
  void LogScreen ( const char *text );
    // logs preformated message to screen only.  can be overriden.

  void LogScreenPercentSingle(u32 percent, u32 lastpercent, bool restarted);

  void LogScreenPercentMulti(u32 cpu, u32 percent, u32 lastpercent, bool restarted);
    // progress percentage printing to screen only.

  s32  Configure( void );
    // runs the interactive configuration setup
#endif

  s32  ConfigureGeneral( s32 currentmenu );
    // part of the interactive setup

  static s32 yesno(char *str);
    // Checks whether user typed yes or no, used in interactive setup
    // Returns 1=yes, 0=no, -1=unknown

  static s32 findmenuoption( s32 menu, s32 option);
    // Returns the id of the option that matches the menu and option
    // requested. Will return -1 if not found.

  void setupoptions( void );
    // Sets all the pointers/etc for optionstruct options

#ifndef DONT_USE_PATHWORK
  static void killwhitespace( char *string );
    // Removes all spaces from a string

  static int isstringblank( char *string );
    // returns 1 if a string is blank (or null), 0 if it is not
#endif

  static void clearscreen( void );
    // Clears the screen. (Platform specific ifdefs go inside of it.)

  s32  ReadConfig( void );
    // returns -1 if no ini exits, 0 otherwise

  void ValidateConfig( void );
    // verifies configuration and forces valid values

  s32  WriteConfig( void );
    // returns -1 on error, 0 otherwise

  s32  WriteContestandPrefixConfig( void );
    // returns -1 on error, 0 otherwise
    // only writes contestdone and randomprefix .ini entries

  u32  Benchmark( u8 contest, u32 numk );
    // returns keys/second

  s32  SelfTest( u8 contest );
    // run all the test keys
    // Returns: number of tests passed, or negative number of test that failed

  s32 Run( void );
    // run the loop, do the work
    // returns:
    //    -2 = exit by error (all contests closed)
    //    -1 = exit by error (critical)
    //     0 = exit for unknown reason
    //     1 = exit by user request
    //     2 = exit by exit file check
    //     3 = exit by time limit expiration
    //     4 = exit by block count expiration

  s32  Fetch( u8 contest, Network *netin = NULL, s32 quietness=0 );
    // fills up all of the input buffers
    // this is for sneakernet support amung other things
    // Returns: number of buffers received, negative if some error occured
    // If quietness > 1, it will not display the proxymessage.
    // in the future, 1 could make it not show # of blocks transferred

  s32  ForceFetch( u8 contest, Network *netin = NULL );
    // Like fetch, but keeps trying until done or until buffer size doesn't get bigger
    // Basically, ignores premature disconnection.

  s32  Flush( u8 contest, Network *netin = NULL, s32 quietness=0 );
    // flushes out result buffers, useful when a SUCCESS happens
    // Also remove buffer file stub if done
    // this is for sneakernet support
    // Returns: number of buffers sent, negative if some error occured
    // If quietness > 1, it will not display the proxymessage.
    // in the future, 1 could make it not show # of blocks transferred

  s32  ForceFlush( u8 contest, Network *netin = NULL );
    // Like flush, but keeps trying until done or until buffer size doesn't get smaller
    // Basically, ignores '31' and '32' type errors -- bad buffer file entry problems.

  s32 Update( u8 contest, s32 fetcherr, s32 flusherr);
    // flushes out buffer, and fills input buffers
    // returns: number of buffers sent, negative if some error occurred
    // Return value is the return from fetch*fetcherr +
    //                      value from flush*flusherr

  s32 Install();
    // installs the clients into autolaunch configuration
    // returns: non-zero on failure

  s32 Uninstall(void);
    // removes the client from autolaunch configuration
    // returns: non-zero on failure

  s32 RunStartup(void);
    // to be called before calling Run() for the first time
    // returns: non-zero on failure

  s32 SelectCore(void);
    // to be called before Run(), Benchmark(), or Test() to configure for cpu
    // returns: non-zero on failure

  s32 UnlockBuffer( const char *filename );
    // unlock buffer 'filename'

  int GetProcessorType();  //was x86id(); and ARMid(); nullfunction otherwise
  // Identify CPU type by hardware check - in cpucheck.cpp

  void ValidateProcessorCount();
  // validates numcpu (stores result in numcputemp) - in cpucheck.cpp

  s32 SetContestDoneState( Packet * packet);
    // Set the contest state appropriately based on packet information
    // Returns 1 if a change to contest state was detected
};

// --------------------------------------------------------------------------
#if (CLIENT_OS == OS_OS2)
  #include "platforms\os2cli\dod.h"   // needs to be included after Client
#endif
// --------------------------------------------------------------------------

#ifdef NEW_STATS_AND_LOGMSG_STUFF
  #include "clitime.h"
  #include "clirate.h"
  #include "clisrate.h"
  #define Time() (CliGetTimeString(NULL,1))
#else
  #if (CLIENT_OS == OS_WIN32) || (CLIENT_OS == OS_NETWARE) || \
      (CLIENT_OS == OS_DOS) || (CLIENT_OS == OS_WIN32S) || \
      (CLIENT_OS == OS_WIN16) || \
      ((CLIENT_OS == OS_VMS) && !defined(MULTINET))
    #ifdef __WATCOMC__
      // disable "Warning! W481: col(1) class/enum has the same name
      // as the function/variable 'timezone'"
      #pragma warning 481 9 ;
    #endif
    struct timezone
    {
      int  tz_minuteswest;    /* of Greenwich */
      int  tz_dsttime;        /* type of dst correction to apply */
    };
  #endif
  #if (((CLIENT_OS == OS_SUNOS) && (CLIENT_CPU == CPU_68K)) ||   \
     (CLIENT_OS == OS_MACOS) ||                              \
     (CLIENT_OS == OS_SCO) ||                                \
     (CLIENT_OS == OS_OS2) ||                                \
     (CLIENT_OS == OS_AMIGAOS) ||                            \
     (CLIENT_OS == OS_NETWARE) ||                            \
     (CLIENT_OS == OS_WIN32) ||                              \
     (CLIENT_OS == OS_WIN32S) ||                             \
     (CLIENT_OS == OS_WIN16) ||                              \
     (CLIENT_OS == OS_DOS) ||                                \
     ((CLIENT_OS == OS_VMS) && !defined(MULTINET)))
  extern "C" gettimeofday(struct timeval *tv, struct timezone *);
  #endif
#endif //#ifdef NEW_STATS_AND_LOGMSG_STUFF

// --------------------------------------------------------------------------

extern Problem problem[2*MAXCPUS];

extern volatile u32 SignalTriggered, UserBreakTriggered;
extern volatile s32 pausefilefound;
extern void CliSetupSignals( void );

// --------------------------------------------------------------------------

#endif //CLIENT_H

