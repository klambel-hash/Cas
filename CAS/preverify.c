#include "preverify.h"
#include <time.h>

int8_t f3_vector_coeff_f(const wave_word *t_0, const wave_word *t_1, int j) {

	int i;
	i = j / WORD_LENGTH;
	j %= WORD_LENGTH;
	if (((t_0[i] >> j) & 1) && ((t_1[i] >> j) & 1)) {
		return 2;
	} else if (((t_0[i] >> j) & 1)) {
		return 1;
	} else {
		return 0;
	}

}

int preverify(f3_vector *statement, const uint8_t seed_salt, const uint8_t *message, const size_t mlen, const wave_word *t_0, const wave_word *t_1, mf3 *pk) {


	//printf("\n======= Preverif    %u ========\n", seed_salt);


	f3_vector test1 = f3_vector_new(N-K);
	f3_vector test2 = f3_vector_new(N-K);
	f3_vector test3 = f3_vector_new(N - K);
	f3_vector test4 = f3_vector_new(K);
	uint8_t * aa= (uint8_t*) malloc((N-K)* sizeof(uint8_t));
	uint8_t salt[SALT_SIZE] = { 0 };
	prng_t *PRNG1 = prng_init(seed_salt);
	f3_vector r = f3_vector_new(N);
	f3_vector_rand(&r, PRNG1);
	for (size_t b = 0; b < N-K; b++){
		uint8_t bb=f3_vector_get_coeff(&r, b);
		f3_vector_setcoeff(&test1, b, bb);
		}
	for (size_t b = 0; b < K; b++){
		uint8_t bb=f3_vector_get_coeff(&r, N-K+b);
		f3_vector_setcoeff(&test4, b, bb);
	}
	mf3_mv_mul(mf3_transpose(pk), &test4, aa);
	f3_vector_set_from_array(&test2,aa,N-K);
	f3_vector_add(&test1, &test2, &test3);
	//printf("\n   Presign   \n ");
	
	f3_vector_add(&test3, statement, &test1);
	//f3_vector_print_big(&test1);
	for (size_t b = 0; b < N-K; b++){
		salt[b]=f3_vector_get_coeff(&test2, b);
	}







	int cmp = 0;
	size_t alloc = 1 + (((N - K) - 1) / WORD_LENGTH);
	wave_word hash_r0[(1 + (((N - K) - 1) / WORD_LENGTH))] = { 0 };
	wave_word hash_r1[(1 + (((N - K) - 1) / WORD_LENGTH))] = { 0 };
	hash_message_f(hash_r0, hash_r1, message, mlen, salt, SALT_SIZE);

	for (int i = 0; i < K / 2; i++) {
		int8_t c_i, c_ip;

		c_i = (f3_vector_coeff_f(t_0, t_1, 2 * i)
				+ f3_vector_coeff_f(t_0, t_1, (2 * i) + 1)) % 3;
		c_ip = (((f3_vector_coeff_f(t_0, t_1, 2 * i))
				- ((f3_vector_coeff_f(t_0, t_1, 2 * i + 1)))) + 3) % 3;

		if (c_i == 1) {
			f3_vector_sum_inplace_f(hash_r0, hash_r1, pk->row[2 * i].r0,
					pk->row[2 * i].r1, alloc);
		} else if (c_i == 2) {
			f3_vector_sub_inplace_f(hash_r0, hash_r1, pk->row[2 * i].r0,
					pk->row[2 * i].r1, alloc);
		}
		if (c_ip == 1) {
			f3_vector_sum_inplace_f(hash_r0, hash_r1, pk->row[2 * i + 1].r0,
					pk->row[2 * i + 1].r1, alloc);
		} else if (c_ip == 2) {
			f3_vector_sub_inplace_f(hash_r0, hash_r1, pk->row[2 * i + 1].r0,
					pk->row[2 * i + 1].r1, alloc);
		}

	}

	if (f3_vector_coeff_f(t_0, t_1, K - 1) == 1) {
		f3_vector_sum_inplace_f(hash_r0, hash_r1, pk->row[K - 1].r0,
				pk->row[K - 1].r1, alloc);
	} else if (f3_vector_coeff_f(t_0, t_1, K - 1) == 2) {
		f3_vector_sub_inplace_f(hash_r0, hash_r1, pk->row[K - 1].r0,
				pk->row[K - 1].r1, alloc);
	}

	hash_r0[alloc - 1] = hash_r0[alloc - 1] & 0x7F;
	hash_r1[alloc - 1] = hash_r1[alloc - 1] & 0x7F;

	size_t weight_t = popcnt(t_0, 704);
	size_t weight_v = popcnt(hash_r0, alloc * 8);

	if ((weight_t + weight_v) == W){
		printf("\n Preverification success \n");
		cmp = 1;
	}
	//f3_vector_free(&hash);

	return cmp;
}

