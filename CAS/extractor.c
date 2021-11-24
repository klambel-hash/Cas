#include "extractor.h"
int  extract(mf3 *pk, f3_vector *presig, f3_vector *sig,  uint8_t seed, f3_vector *sig_r, f3_vector *statement, f3_vector *witness){
        
        /*f3_vector test3=f3_vector_new(N-K);
        f3_vector test4=f3_vector_new(K);
        f3_vector test5=f3_vector_new(N-K);
        f3_vector test6=f3_vector_new(N-K);*/
        prng_t *PRNG2 = prng_init(seed);
        f3_vector r = f3_vector_new(N);
		f3_vector_rand(&r, PRNG2);
        f3_vector_neg(&r);
       
        f3_vector_add(&r, sig_r, witness);
        if (f3_vector_weight(witness)!=WIT_WEIGHT){
            printf("witness extraction failed!!!!!!");
            return 0;
        }
/*
        if (f3_vector_weight(witness)==WIT_WEIGHT){
            for (size_t b = 0; b < N-K; b++){
		        uint8_t bb=f3_vector_get_coeff(witness, b);
		        f3_vector_setcoeff(&test3, b, bb);
		    }
	        for (size_t b = 0; b < K; b++){
		        uint8_t bb=f3_vector_get_coeff(witness, N-K+b);
		        f3_vector_setcoeff(&test4, b, bb);
	        }
            uint8_t *aa=(uint8_t*) malloc((N-K)* sizeof(uint8_t));
	        mf3_mv_mul(mf3_transpose(pk), &test4, aa);
            
            f3_vector_set_from_array(&test5,aa,N-K);
            free(aa);
	        f3_vector_add(&test3, &test5, &test6);
            int a=f3_vector_are_equal(statement, &test6);
                if(a==0){
                     printf("witness extraction failed!!!!!!");
                 return 0;
            }
        }
        else{
            printf("\nwitness extraction failed!!!!!!\n");
            return 0;
        }*/
/*
		f3_vector_free(&test3);
        f3_vector_free(&test4);
		f3_vector_free(&test5);
		f3_vector_free(&test6);
*/

       printf("Success of witness extraction!!!!!!");
            return 1;
}