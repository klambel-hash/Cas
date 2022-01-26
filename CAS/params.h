
#ifndef PARAMS_H_
#define PARAMS_H_


#define PARAMS_ID_SIZE 5
static const char PARAMS_ID[PARAMS_ID_SIZE] = "128g\0";

#define N 8492
#define K 5605
#define W 7980
#define KU 3558
#define KV 2047
#define N2 4246
#define D 81
#define Mu 162

#define WIT_WEIGHT 13



/**********change MAX_BUFF_SIZE from 1000 to 10000 and SALT_SIZE from 16 to N-K**********/
#define SALT_SIZE N-K
#define MAX_BUFF_SIZE 10000

#endif /* PARAMS_H_ */
