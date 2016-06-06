/*
  dummy.c --- dummy functions of hb.f for debugging
  Copyright (C) 2000  A. Tamii
  Author:   A. Tamii
  Version 1.00 04-JUN-2000 by A. Tamii
*/
#include <stdio.h>
#include <string.h>
#include <math.h>
#include <errno.h>
#include <stdarg.h>

#include "freadin.h"
#include "histogram.h"

#if 0
void hstini_(char *name){}
void hstend_(){}
void hstbk1_(int *h, char *nam, float *min, float *max, int *nbin){}
void hstfl1_(int *h, float *data){}
void hstbk2_(int *h, char *nam, float *min1, float *max1, int *nbin1,
            	float *min2, float *max2, int *nbin2){}
void hstfl2_(int *h, float *data1, float *data2){}
void hstdel_(int *h){}
void hstout_(char *name){}
void hstshm_(char *name){}
#endif

#if 1
void G77_lnblnk_0(){}
#endif

/*
Local Variables:
mode: C
tab-width: 2
End:
*/
