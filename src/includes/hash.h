/*
  By Bob Jenkins, 1996.  bob_jenkins@burtleburtle.net.  You may use this
  code any way you wish, private, educational, or commercial.  It's free.

  See http://burtleburtle.net/bob/hash/evahash.html
  Use for hash table lookup, or anything where one collision in 2^^32 is
  acceptable.  Do NOT use for cryptographic purposes.
*/

#ifndef _jenkinshash_h
#define _jenkinshash_h

#ifdef __cplusplus
extern "C" {
#endif

typedef  unsigned long  int  ub4;   /* unsigned 4-byte quantities */
typedef  unsigned       char ub1;   /* unsigned 1-byte quantities */

#define hashsize(n) ((ub4)1<<(n))
#define hashmask(n) (hashsize(n)-1)

ub4 jenkins_hash(const ub1 *k,
                 ub4 length,
                 ub4 initval);

#ifdef __cplusplus
}
#endif

#endif // _jenkinshash_h
