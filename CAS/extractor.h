
#ifndef SRC_WAVE_EXTRACT_H_
#define SRC_WAVE_EXTRACT_H_

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

int  extract(mf3 *pk, f3_vector *presig, f3_vector *sig,  uint8_t seed, f3_vector *sig_r, f3_vector *statement, f3_vector *witness);

#endif /* SRC_WAVE_EXTRACT_H_ */