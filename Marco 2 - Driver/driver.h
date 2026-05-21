#ifndef DRIVER_H
#define DRIVER_H

#ifdef __cplusplus
extern "C" {
#endif

int elm_open(void);
void elm_close(void);
void elm_reset(void);
int elm_start(void);
int elm_result(void);
int elm_load(void);

#ifdef __cplusplus
}
#endif

#endif