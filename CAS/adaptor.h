#ifndef SRC_WAVE_ADAPT_H_
#define SRC_WAVE_ADAPT_H_

#include <stdlib.h>
#include <string.h>

#include "fips202.h"

#include "f3.h"
#include "vf3.h"
#include "mf3.h"

#include "prng.h"
#include "randombytes.h"

#include "definitions.h"

#include "commons.h"

#include "wave.h"
#include "params.h"

#include "hash.h"

//void adapt(const f3_vector *presig, prng_t *PRNG_r, prng_t *PRNG_wit, f3_vector *sig, f3_vector * sig_r);
int adapt(const f3_vector *presig, uint8_t seed, f3_vector *witness, f3_vector *sig, f3_vector * sig_r);

#endif /* SRC_WAVE_ADAPT_H_ */
