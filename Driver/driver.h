#ifndef DRIVER_H
#define DRIVER_H

#ifdef __cplusplus
extern "C" {
#endif

int elm_open(void);
void elm_close(void);
void elm_reset(void);
int elm_store_img(const unsigned char *img);
int elm_store_bias(const unsigned short *bias);
int elm_store_beta(const unsigned short *beta);
int elm_store_weights(const unsigned short *weights);
int elm_start(void);
int elm_result(void);

#ifdef __cplusplus
}
#endif

#endif