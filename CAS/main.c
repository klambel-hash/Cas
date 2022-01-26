#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#include "cpucycles.h"
#include "vf3.h"
#include "mf3.h"
#include "prng.h"
#include "commons.h"
#include "compress.h"

#include "keygen.h"
#include "presign.h"
#include "preverify.h"

#include "adaptor.h"
#include "extractor.h"

#define MAXMLEN 16

void print_signature(f3_vector *sig) {

	f3_vector_print(sig);
	printf("\n");
}

#define ITERATIONS 100

double timekey_f[ITERATIONS];
double time_presign_f[ITERATIONS];
double time_sign_f[ITERATIONS];
double time_preverify_f[ITERATIONS];
double time_verif_f[ITERATIONS];
double time_adapt_f[ITERATIONS];
double time_extract_f[ITERATIONS];

void swap(double *a, double *b) {
	double t = *a;
	*a = *b;
	*b = t;
}

// A function to implement bubble sort

void bubbleSort(double arr[], int n) {
	int i, j;
	for (i = 0; i < n - 1; i++)

		// Last i elements are already in place
		for (j = 0; j < n - i - 1; j++)
			if (arr[j] > arr[j + 1])
				swap(&arr[j], &arr[j + 1]);
}

int main(void) {
	const int wt=WIT_WEIGHT;
	init(PARAMS_ID);

	clock_t start, end;
	memset(timekey_f, 0, sizeof(double) * ITERATIONS);
	memset(time_presign_f, 0, sizeof(double) * ITERATIONS);
	memset(time_preverify_f, 0, sizeof(double) * ITERATIONS);
	memset(time_adapt_f, 0, sizeof(double) * ITERATIONS);

	printf("Warming up.\n");
	//uint8_t compressed[940] = { 0 };
	for (int i = 0; i < ITERATIONS ; i++) {
		wave_pk_t pk;
		wave_sk_t sk;
		
		//memset(compressed, 0, sizeof(uint8_t) * 940);

		f3_vector e = f3_vector_new(N);
		f3_vector witness = f3_vector_new(N);
		f3_vector witness_extract = f3_vector_new(N);
		f3_vector_zero(&witness_extract);
		f3_vector statement = f3_vector_new(N-K);
		f3_vector test1 = f3_vector_new(N-K);
		f3_vector test2 = f3_vector_new(N-K);
		f3_vector test3 = f3_vector_new(N - K);
		f3_vector test4 = f3_vector_new(K);
		
		uint8_t * aa= (uint8_t*) malloc((N-K)* sizeof(uint8_t));



		f3_vector signature = f3_vector_new(N);
		f3_vector sig_r = f3_vector_new(N);
		f3_vector presignature = f3_vector_new(N);

		f3_vector m_hash = f3_vector_new(N - K);
		f3_vector m_hash1 = f3_vector_new(N - K);
		uint8_t mi[MAXMLEN] = { 0 };
/**************************************** Key generation *******************************************/		
		start = clock();
		keygen(&sk, &pk);
		end = clock();
		timekey_f[i] = (double) (end - start) * 1000.0 / CLOCKS_PER_SEC;

		mf3 *MM=pk;
/************************************************************************************************/	

		uint8_t seed_salt,seed_salt1, seed_wit;
		randombytes(&seed_salt, 1);
		randombytes(&seed_salt1, 1);
		randombytes(&seed_wit, 1);
		//uint8_t salt[SALT_SIZE] = { 0 };
		prng_t  *PRNG = prng_init(seed_wit);
		f3_vector_zero(&witness);
		
		f3_error_vector_rand(&witness, wt, PRNG);
		
/************************************* Statement ***************************************************/		
		for (size_t b = 0; b < N-K; b++){
			uint8_t bb=f3_vector_get_coeff(&witness, b);
			f3_vector_setcoeff(&test1, b, bb);
		}

		for (size_t b = 0; b < K; b++){
			uint8_t bb=f3_vector_get_coeff(&witness, N-K+b);
	
			f3_vector_setcoeff(&test4, b, bb);
	}
	mf3_mv_mul(mf3_transpose(MM), &test4, aa);
	f3_vector_set_from_array(&test2,aa,N-K);
	f3_vector_add(&test1, &test2, &statement);
	//f3_vector_print_big(&statement);

/****************************************    Message    ****************************************/

		f3_vector_zero(&m_hash);
		f3_vector_zero(&m_hash1);
		memset(mi, 0, MAXMLEN * sizeof(uint8_t));
		randombytes(mi, MAXMLEN);
/************************************    preSignature    ****************************************/

		start = clock();
		presign( &statement, &presignature, &m_hash, seed_salt, mi, MAXMLEN, &sk, MM);
		end = clock();
		time_presign_f[i] = (double) (end - start) * 1000.0 / CLOCKS_PER_SEC;

		printf("\n======= main function %u ========\n", i);
		size_t alloc = (1 + ((K - 1)) / WORD_LENGTH);
		wave_word a_r0[(1 + ((K - 1)) / WORD_LENGTH)] = { 0 };
		wave_word a_r1[(1 + ((K - 1)) / WORD_LENGTH)] = { 0 };

		memcpy(a_r0, presignature.r0 + 45, 88 * sizeof(wave_word));
		memcpy(a_r1, presignature.r1 + 45, 88 * sizeof(wave_word));
		shift7(a_r0, alloc);
		shift7(a_r1, alloc);

		f3_vector a = f3_vector_new(K);
		memcpy(a.r0, presignature.r0 + 45, 88 * sizeof(wave_word));
		memcpy(a.r1, presignature.r1 + 45, 88 * sizeof(wave_word));
		shift7(a.r0, a.alloc);
		shift7(a.r1, a.alloc);
		size_t outlen = 0;

//		printf("\n");
/************************************    preverification    ****************************************/

		start = clock();
		int res = preverify( &statement, seed_salt, mi, MAXMLEN, a_r0, a_r1, MM);
		end = clock();
		time_preverify_f[i] = (double) (end - start) * 1000.0 / CLOCKS_PER_SEC;

		if (res != 1) {
			printf("\nfail...\n");
		}
		else{
			printf("\n Preverification success \n");
		}



/*********************************** Adaptor *************************************/
		start = clock();
		adapt(&presignature, seed_salt,  &witness, &signature, &sig_r);
		end = clock();
		time_adapt_f[i] = (double) (end - start) * 1000.0 / CLOCKS_PER_SEC;
		if (res != 1) {
			printf("\n Adaptation failed...\n");
		}
		else{
			printf("\n Adaptation success \n");
		}

/************************************ Extractor ************************************/
		start = clock();
		res=extract(MM, &presignature, &signature, seed_salt, &sig_r, &statement, &witness_extract);
		end = clock();
		time_extract_f[i] = (double) (end - start) * 1000.0 / CLOCKS_PER_SEC;
		if (res != 1) {
			printf("\nExtraction fail...\n");
		}
		else{
			printf("\n Extraction success \n");
		}






/************************************    Signature    ****************************************/

		start = clock();
		sign(&signature, &m_hash1, seed_salt1, mi, MAXMLEN, &sk, MM);
		end = clock();
		time_sign_f[i] = (double) (end - start) * 1000.0 / CLOCKS_PER_SEC;

		//size_t alloc = (1 + ((K - 1)) / WORD_LENGTH);
		wave_word b_r0[(1 + ((K - 1)) / WORD_LENGTH)] = { 0 };
		wave_word b_r1[(1 + ((K - 1)) / WORD_LENGTH)] = { 0 };

		memcpy(b_r0, signature.r0 + 45, 88 * sizeof(wave_word));
		memcpy(b_r1, signature.r1 + 45, 88 * sizeof(wave_word));
		shift7(b_r0, alloc);
		shift7(b_r1, alloc);

		f3_vector b = f3_vector_new(K);
		memcpy(b.r0, signature.r0 + 45, 88 * sizeof(wave_word));
		memcpy(b.r1, signature.r1 + 45, 88 * sizeof(wave_word));
		shift7(b.r0, b.alloc);
		shift7(b.r1, b.alloc);

/************************************    Verification    ****************************************/

		start = clock();
		res = verif(seed_salt1, mi, MAXMLEN, b_r0, b_r1, MM);
		end = clock();
		time_verif_f[i] = (double) (end - start) * 1000.0 / CLOCKS_PER_SEC;

		if (res != 1) {
			printf("\nfail...\n");
		}
		else{
			printf("\n verification success \n");
		}	

/*******************************************************************************************/

		f3_vector_free(&test1);
		f3_vector_free(&test2);
		f3_vector_free(&test3);
		f3_vector_free(&test4);
		f3_vector_free(&witness);
		f3_vector_free(&witness_extract);
		f3_vector_free(&statement);
		free(aa);
		wave_pk_clear(pk);
		wave_sk_clear(&sk);
		f3_vector_free(&signature);
		f3_vector_free(&presignature);
		f3_vector_free(&sig_r);
		f3_vector_free(&m_hash);
		f3_vector_free(&m_hash1);
		f3_vector_free(&a);
		prng_clear(PRNG);
		//printf(".\n");
	}


	bubbleSort(timekey_f, ITERATIONS - 1);
	bubbleSort(time_preverify_f, ITERATIONS - 1);
	bubbleSort(time_presign_f, ITERATIONS - 1);
	bubbleSort(time_verif_f, ITERATIONS - 1);
	bubbleSort(time_sign_f, ITERATIONS - 1);
	bubbleSort(time_adapt_f, ITERATIONS - 1);
	bubbleSort(time_extract_f, ITERATIONS - 1);
	double presign_avg = 0;
	double preverify_avg = 0;
	double sign_avg = 0;
	double verif_avg = 0;
	double keygen_avg = 0;
	double adapt_avg = 0;
	double extract_avg = 0;
	for (int i = 0; i < ITERATIONS; i++) {
		presign_avg += time_presign_f[i];
		preverify_avg += time_preverify_f[i];
		sign_avg += time_presign_f[i];
		verif_avg += time_preverify_f[i];
		keygen_avg += timekey_f[i];
		adapt_avg += time_adapt_f[i];
		extract_avg += time_extract_f[i];
	}

	printf("keygen() MEDIAN %f milli-seconds \n", timekey_f[ITERATIONS / 2]);

	printf("keygen() AVG %f milli-seconds \n", (keygen_avg / ITERATIONS));

	printf("presign() MEDIAN %f milli-seconds\n", time_presign_f[ITERATIONS / 2]);

	printf("presign() AVG  %f milli-seconds \n", (presign_avg / ITERATIONS));

	printf("preverify() MEDIAN %f milli-seconds \n", time_preverify_f[ITERATIONS / 2]);

	printf("preverify() AVG  %f milli-seconds \n", (preverify_avg / ITERATIONS));

	printf("sign() MEDIAN %f milli-seconds\n", time_sign_f[ITERATIONS / 2]);

	printf("sign() AVG  %f milli-seconds \n", (sign_avg / ITERATIONS));

	printf("verif() MEDIAN %f milli-seconds \n", time_verif_f[ITERATIONS / 2]);

	printf("verif() AVG  %f milli-seconds \n", (verif_avg / ITERATIONS));

	printf("adapt() MEDIAN %f milli-seconds \n", time_adapt_f[ITERATIONS / 2]);

	printf("adapt() AVG %f milli-seconds \n", (adapt_avg / ITERATIONS));

	printf("extract() MEDIAN %f milli-seconds \n", time_extract_f[ITERATIONS / 2]);

	printf("extract() AVG %f milli-seconds \n", (extract_avg / ITERATIONS));

	cleanup();
	return EXIT_SUCCESS;
}
