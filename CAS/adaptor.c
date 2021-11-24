#include "adaptor.h"


//void adapt(const f3_vector *presig, prng_t *PRNG_r, prng_t *PRNG_wit, f3_vector *sig, f3_vector * sig_r){
int adapt(const f3_vector *presig, uint8_t seed, f3_vector *witness, f3_vector *sig, f3_vector * sig_r){
    if(f3_vector_weight(presig)!=W){
        printf("\n Adaptor Algorithm failed!!!!!\n");
        return 0;
    }
    else{

        prng_t *PRNG2 = prng_init(seed);
        f3_vector r = f3_vector_new(N);
		f3_vector_rand(&r, PRNG2);
        f3_vector_add(&r, witness, sig_r);
        printf("\nSuccess of Adaptor Algorithm\n");
        

    }
    return 1;
}