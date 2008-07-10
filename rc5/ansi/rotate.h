// Copyright distributed.net 1997-2003 - All Rights Reserved
// For use in distributed.net projects only.
// Any other distribution or use of this source violates copyright.
// 
// $Id: rotate.h,v 1.7.4.2 2003/01/29 01:29:58 andreasb Exp $
//
// $Log: rotate.h,v $
// Revision 1.7.4.2  2003/01/29 01:29:58  andreasb
// 2003 copyright update - round 2
//
// Revision 1.7.4.1  2003/01/19 14:33:56  andreasb
// Applied NeXTstep patch (bug #1451, attachment #211) from
// Michael Weiser <mweiser@fachschaft.imn.htwk-leipzig.de>,
// patch was against pubsource 20010416, needed minor fixes.
//
// Revision 1.7  2002/09/02 00:35:55  andreasb
// sync: merged changes from release-2-7111 branch between
//       2000-07-11 and release-2-7111_20020901 into trunk
//
// Revision 1.5.2.5  2002/04/12 23:56:57  andreasb
// 2002 copyright update - round 2
//
// Revision 1.5.2.4  2001/05/20 21:30:22  andreasb
// disable sparc specific SHL/SHR macros when using Sun CC
//
// Revision 1.5.2.3  2001/02/09 04:16:57  sampo
// let ia64 use alpha rotate macro, 50% speedup
//
// Revision 1.6  2000/07/11 02:27:43  mfeiri
// sync
//
// Revision 1.5.2.2  2000/01/08 01:16:38  snake
// Makes ansi cores usable for 68k NetBSDs (maybe faster than crunch core)
//
// Revision 1.5.2.1  1999/12/31 20:05:04  patrick
//
// added ansi_increment for increment inside ansi cores
//
// Revision 1.5  1998/11/17 09:16:52  remi
// Reverted the previous change, rlmi and rlimi are valid POWER instructions ...
//
// Revision 1.4  1998/11/15 07:50:55  remi
// It seems that the POWER architecture doesn't have any 'roll'
// instructions, rlimi and rlmi are just macros which get expanded by the
// POWER assembler. I commented out the relevant inline functions since
// the compiler can probably better optimized the needed shifts, or, etc ...
//
// Revision 1.3  1998/06/14 08:13:55  friedbait
// 'Log' keywords added to maintain automatic change history
//
// 

#ifndef __ROTATE_H__
#define __ROTATE_H__

#if !defined(__GNUC__)
#define __inline__ inline
#endif

#ifdef USE_ANSI_INCREMENT
// "mangle-increment" the key number by the number of pipelines
// this will only work for powers of 2 and is way faster for
// small numbers than __increment_key()

#ifndef PIPELINE_COUNT
        #error "Expecting pipeline to be set to something"
#endif

#define key rc5unitwork->L0

static __inline__ void ansi_increment( RC5UnitWork *rc5unitwork ) {
    key.hi = (key.hi + ( PIPELINE_COUNT << 24)) & 0xFFFFFFFF;
    if (!(key.hi & 0xFF000000))
      {
      key.hi = (key.hi + 0x00010000) & 0x00FFFFFF;
      if (!(key.hi & 0x00FF0000))
        {
        key.hi = (key.hi + 0x00000100) & 0x0000FFFF;
        if (!(key.hi & 0x0000FF00))
          {
          key.hi = (key.hi + 0x00000001) & 0x000000FF;
        // we do not need to mask here, was done above
          if (!(key.hi))
            {
            key.lo = key.lo + 0x01000000;
            if (!(key.lo & 0xFF000000))
              {
              key.lo = (key.lo + 0x00010000) & 0x00FFFFFF;
              if (!(key.lo & 0x00FF0000))
                {
                key.lo = (key.lo + 0x00000100) & 0x0000FFFF;
                if (!(key.lo & 0x0000FF00))
                  {
                  key.lo = (key.lo + 0x00000001) & 0x000000FF;
                  }
                }
              }
            }
          }
        }
      }
}
#undef key
#endif // USE_ANSI_INCREMENT

#if (CLIENT_CPU == CPU_SPARC) && !defined(__SUNPRO_CC)

#define SHL(x, s) ((u32) ((x) << (s) ))
#define SHR(x, s) ((u32) ((x) >> (32 - (s)) ))

#else

#define SHL(x, s) ((u32) ((x) << ((s) & 31)))
#define SHR(x, s) ((u32) ((x) >> (32 - ((s) & 31))))

#endif

#if (CLIENT_CPU == CPU_PA_RISC) && defined(__GNUC__)

static __inline__ u32 ROTL(u32 x, u32 y)
{
	register u32 res;

	__asm__ __volatile(
		"mtsar %2\n\tvshd %1,%1,%0\n\t"
		: "=r" (res)
		: "r" (x), "r" (32 - y)
		);
	return res;
}

static __inline__ u32 ROTL3(u32 x)
{
	register u32 res;

	__asm__ __volatile(
		"shd %1,%1,29,%0\n\t"
		: "=r" (res)
		: "r" (x));
	return res;
}

#elif (CLIENT_CPU == CPU_POWERPC) && defined(__GNUC__)

static __inline__ u32 ROTL(u32 x, u32 y)
{
        register u32 res;

        __asm__ __volatile(
                "rlwnm %0,%1,%2,0,31\n\t"
                :"=r" (res)
                :"r" (x), "r" (y));

        return res;
}

static __inline__ u32 ROTL3(u32 x)
{
        register u32 res;

        __asm__ __volatile(
                "rlwinm %0,%1,3,0,31\n\t"
                :"=r" (res)
                :"r" (x));

        return res;
}


#elif (CLIENT_CPU == CPU_POWER) && defined(__GNUC__)

static __inline__ int ROTL(u32 x, u32 y)
{
  register u32 res;

  __asm(
       "rlmi %0, %1, %2, 0, 31"
       :"=r" (res)
       :"r" (x), "r" (y));
  return res;
}

static __inline__ u32 ROTL3(u32 x)
{
	register u32 res;

  __asm(
        "rlimi %0, %1, 3, 0, 31"
        :"=r" (res)
        :"r" (x));
  return res;
}


#elif (CLIENT_CPU == CPU_68K) && defined(__GNUC__)

#if (CLIENT_OS == OS_SUNOS) || (CLIENT_OS == OS_NETBSD) || \
    (CLIENT_OS == OS_NEXTSTEP)
  #define RC5_WORD u32
#endif

static __inline__ RC5_WORD ROTL(RC5_WORD y, RC5_WORD x)
{
        __asm__ __volatile(
                "roll %2,%0\n\t"
                :"=d" (y)
                :"0" (y), "d" (x) );
        return y;
}
static __inline__ RC5_WORD ROTL3(RC5_WORD x)
{
        register RC5_WORD res;

        __asm__ __volatile(
                "roll #3,%0\n\t"
                :"=d" (res)
                :"0" (x));

        return res;
}

#elif (CLIENT_CPU == CPU_MIPS) && defined(__GNUC__)

static __inline__ u32 ROTL(u32 x, u32 y)
{
  register u32 res;
  __asm(
        "rol %0, %1, %2"
       :"=r" (res)
       :"r" (x), "r" (y));
  return res;
}

static __inline__ u32 ROTL3(u32 x)
{
  register u32 res;
  __asm(
        "rol %0, %1, 3"
       :"=r" (res)
       :"r" (x));
  return res;
}

#elif (CLIENT_CPU == CPU_X86) && defined(__GNUC__)

static __inline__ u32 ROTL(u32 x, u32 y)
{
	register u32 res;

	__asm__ __volatile(
		"roll %%cl,%0\n\t"
		:"=g" (res)
		:"0" (x), "cx" (y)
		:"cx");

	return res;
}

static __inline__ u32 ROTL3(u32 x)
{
	register u32 res;

	__asm__ __volatile(
		"roll $3,%0\n\t"
		:"=g" (res)
		:"0" (x));

	return res;
}

#elif (CLIENT_CPU == CPU_ALPHA || CLIENT_CPU == CPU_IA64)

//// This is based on the post on the rc5 list by micha (mbruck@ins-coin.de)
//// It'll work on any DEC Alpha platform and maybe others
//#define ROTL(v, n) (((u32)((v) << ((n) & 31)) + 
//	(u32)((v) >> (32 - ((n) & 31)))) & 0xFFFFFFFF)
//	
//// This is based on the post on the rc5 list by Joao Miguel Neves
////   (rsacrack@camoes.rnl.ist.utl.pt)
//// It'll also work on any DEC Alpha platform and maybe others
//#define ROTL3(x) (((x) << 3) | ((x) >> 29))
//
// This is from Frank Horowitz <frank@ned.dem.csiro.au>, and is reportedly
// 10% faster on alphas (posted to rc5-coders@llamas.net Oct 12/97
//
static __inline__ u32 ROTL(u32 x, u32 s)
{
  register union {unsigned long long a;
    struct{unsigned int hi;unsigned int lo;}b;}temp;
  temp.a = ((unsigned long long) (x) << ((s) & 31));
  return        (temp.b.hi + temp.b.lo );
}

static __inline__ u32 ROTL3(u32 x)
{
  register union {unsigned long long a;
    struct{unsigned int hi;unsigned int lo;}b;}temp;
  temp.a = ((unsigned long long) (x) << 3);
  return        (temp.b.hi + temp.b.lo );
}

#else
	
#define ROTL(x, s) ((u32) (SHL((x), (s)) | SHR((x), (s))))
#define ROTL3(x) ROTL(x, 3)

#endif


#endif
