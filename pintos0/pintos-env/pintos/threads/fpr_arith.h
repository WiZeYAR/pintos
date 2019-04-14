/*
 * fpr_arith.h
 *
 *  Created on: 20/03/2013
 *      Author: Eduardo Bezerra - kdubezerra@gmail.com
 */

#ifndef FPR_ARITH_H_
#define FPR_ARITH_H_

/* FIXED POINT REAL

   This header contains the definition of the FPReal type, which
   is a signed 32 bits integer. It has the following structure:

   32 bits:
      1 for sign
      17 for integer part
      14 for fractional part

   See section B.6 of Pintos documentation for a detailed explanation:
   http://www2.icorsi.ch/mod/resource/view.php?id=91264

*/

/* Fixed-point real type. */
typedef int FPReal;

/* Number of bits reserved for the fractional part. */
#define FRACBITS 14

/* Convert an FPReal to int */
static inline int FPR_TO_INT (FPReal fpr);

/* Convert an int to FPReal */
static inline FPReal INT_TO_FPR (int);

/* Increment an FPReal by 1 */
static inline FPReal FPR_INC (FPReal *);

/* Add two FPReals */
static inline FPReal FPR_ADD_FPR (FPReal, FPReal);

/* Subtract one FPReal from another */
static inline FPReal FPR_SUB_FPR (FPReal, FPReal);

/* Multiply an FPReal by an int */
static inline FPReal FPR_MUL_INT (FPReal, int);

/* Divide an FPReal by an int */
static inline FPReal FPR_DIV_INT (FPReal, int);

/* Add an int to an FPReal */
static inline FPReal FPR_ADD_INT (FPReal, int);

/* Subtract an int from an FPReal */
static inline FPReal FPR_SUB_INT (FPReal, int);

/* Multiply two FPReals */
static inline FPReal FPR_MUL_FPR (FPReal, FPReal);

/* Divide an FPReal by another */
static inline FPReal FPR_DIV_FPR (FPReal, FPReal);

/* Divide two ints and return the real result as an FPReal */
static inline FPReal INT_DIV_INT (int, int);




/* ==================================*/
/* FPReal functions implementations. */

/* Convert an FPReal to int */
static inline int
FPR_TO_INT (FPReal fpr) {
   return fpr >> FRACBITS;
}

/* Convert an int to FPReal */
static inline FPReal
INT_TO_FPR (int i) {
   return i << FRACBITS;
}

/* Increment an FPReal by 1 */
static inline
FPReal FPR_INC (FPReal * fpr) {
   (*fpr) += (1 << FRACBITS);
   return *fpr;
}

/* Add two FPReals */
static inline
FPReal FPR_ADD_FPR (FPReal fpr_a, FPReal fpr_b) {
   return fpr_a + fpr_b;
}

/* Subtract one FPReal from another */
static inline
FPReal FPR_SUB_FPR (FPReal fpr_a, FPReal fpr_b) {
   return fpr_a - fpr_b;
}

/* Multiply an FPReal by an int */
static inline
FPReal FPR_MUL_INT (FPReal fpr_a, int b) {
   return fpr_a * b;
}

/* Divide an FPReal by an int */
static inline
FPReal FPR_DIV_INT (FPReal fpr_a, int b) {
   return fpr_a / b;
}

/* Add an int to an FPReal */
static inline
FPReal FPR_ADD_INT (FPReal fpr_a, int b) {
   return fpr_a + (b << FRACBITS);
}

/* Subtract an int from an FPReal */
static inline
FPReal FPR_SUB_INT (FPReal fpr_a, int b) {
   return fpr_a - (b << FRACBITS);
}

/* Multiply two FPReals */
static inline
FPReal FPR_MUL_FPR (FPReal fpr_a, FPReal fpr_b) {
   int64_t extended_a = fpr_a;
   extended_a *= (int64_t) fpr_b;
   extended_a >>= FRACBITS;
   return (FPReal) extended_a;
}

/* Divide an FPReal by another */
static inline
FPReal FPR_DIV_FPR (FPReal fpr_a, FPReal fpr_b) {
   int64_t extended_a = fpr_a;
   extended_a <<= FRACBITS;
   extended_a /= (int64_t) fpr_b;
   return (FPReal) extended_a;
}

/* Divide two ints and return the real result as an FPReal */
static inline
FPReal INT_DIV_INT (int a, int b) {
   int64_t exta = a;
   exta <<= FRACBITS;
   exta /= b;
   return (FPReal) exta;
}

#endif /* FPR_ARITH_H_ */
