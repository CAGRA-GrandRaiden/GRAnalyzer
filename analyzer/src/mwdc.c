/*
  mwdc.c ---- MWDC data analysis
  Version 1.00  26-SEP-2000  by A. Tamii
*/

#include <stdio.h>
#include <math.h>
#include "freadin.h"
#include "histogram.h"
#include "analyzer.h"

/* MWDC Clusterize */
int chamb_mwdc_clust(plane)
	plane_p  plane;
{
	int  min, max;
	int  i, n, t, w;
	int  *p;
	clst_p  clst;

	if(plane==(plane_p)NULL){
		fprintf(stderr, "evt_mwdc_clust: null plane pointer\n");
		return(-1);
	}

	min = dr_min_ref(plane->h_wire);
	max = dr_max_ref(plane->h_wire);

	/* Get TDC data (allow multiple hits in a wire) */
	n=0; 
	for(i=max-1; i>=min; i--){
		w = dr_int(dr_get_ref(plane->h_wire,i));
		t = dr_int(dr_get_ref(plane->h_tdc,i));
		clst = &plane->clst[n++];
		clst->wire[0] = w;
		clst->tdc[0]  = t;
		clst->size = 1;
		dr_append_ref(plane->h_wirer,w);
		dr_append_ref(plane->h_tdcr,t);
		dr_append_ref(plane->h_clstsz,1);
		if(n>=Clst_MaxNClst){
			if(0)
				fprintf(stderr, "evt_mwdc_clust: too many wire hits (>=%d)\n",
								Clst_MaxNClst);
			break;
		}
	}
	plane->nc = n;
	dr_set_ref(plane->h_nhit,0,n);
	dr_set_ref(plane->h_nclst,0,n);

	return(0);
}

/* MWDC Position Calculation */
int chamb_mwdc_pos(plane)
		 plane_p plane;
{
	clst_p  clst;
	int     ic;
	double  dw, dd, disp, dl;
	if(!plane->wire.ok) return(-1);
	disp = plane->wire.disp;
	dl   = plane->wire.dl;
	for(ic=0; ic<plane->nc; ic++){
		clst = &plane->clst[ic];
		dr_set_nothing(&clst->pos);
		if(dr_exists(clst->dist[0])){
			dw = clst->wire[0]*disp;
			dd = clst->dist[0]*dl;
			clst->pos1 = dw - dd;
			clst->pos2 = dw + dd;
			clst->posid = POSID_NOERR;
		}else{
			dr_set_nothing(&clst->pos1);
			dr_set_nothing(&clst->pos2);
			clst->posid = POSID_NOHIT;
		}
	}
	return(0);
}

/*
  Local Variables:
  mode: C
  tab-width: 2
  End:
*/
