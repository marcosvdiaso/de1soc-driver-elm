#ifndef DRIVER_H
#define DRIVER_H

int elm_open(void);
void elm_close(void);
void elm_reset(void);
int elm_start(void);
int elm_result(void);
int elm_load(void);

#endif
