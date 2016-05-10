/*
  mwpc.c ---- MWPC data analysis
  Version 1.00  03-JUL-2001  by A. Tamii
*/

#include <stdio.h>
#include <math.h>
#include "freadin.h"
#include "histogram.h"
#include "analyzer.h"

//FLAG

/* MWPC Clusterize (Not much thing to do....) */
int chamb_mwpc_clust(plane)
	plane_p  plane;
{
	int  min, max;
	int  i, j, n, s, w, nhit;
	int  *p;
	static int n_half_bit_err=200;
	static int n_too_many_cluster_err=200;
	clst_p  clst;

	if(plane==(plane_p)NULL){
		fprintf(stderr, "evt_mwdc_clust: null plane pointer\n");
		return(-1);
	}

	min = dr_min_ref(plane->h_wire);
	max = dr_max_ref(plane->h_wire);

	/* Get data */
	n=0; 
	nhit = 0;
	for(i=min; i<max; i++){
		w = dr_int(dr_get_ref(plane->h_wire,i));
		s = dr_int(dr_get_ref(plane->h_clstsz,i));
		clst = &plane->clst[n++];
		if(((w+s)&0x01)==0){
			if(n_half_bit_err-->0){
				fprintf(stderr,
					"chamb_mwpc_clust: pcos half bit inconsistency in %s-%s (wire=%d,width=%d)\n",
					plane->chamb->name, plane->name, w, s);
				if(n_half_bit_err==0)
					fprintf(stderr,
						"chamb_mwpc_clust: too many same type of errors. Hereafter ignore it.\n");
			}
		}else{
			if(s>plane->max_clst_size){
				if(0)
					fprintf(stderr,
 					"chamb_mwpc_clust: too large cluster (size=%d). The size is reduced to %d.\n",
									s, plane->max_clst_size);
				s = plane->max_clst_size;
			}
			for(j=0; j<s; j++){
				clst->wire[j] = w;
				dr_append_ref(plane->h_wirer,((w-s)>>1)+j);
			}
		}
		clst->size = s;
		nhit += s;
		clst->pos = w/2.;  /* /2 is for the half bit */
		clst->posid = POSID_NOERR;
#if 1
		dr_append_ref(plane->h_pos, clst->pos);
#endif
		if(n>=Clst_MaxNClst){
			if(n_too_many_cluster_err-->0){
				fprintf(stderr, "evt_mwdc_clust: too many clusters (%d)\n", max-min);
				if(n_too_many_cluster_err==0)
					fprintf(stderr,
						"chamb_mwpc_clust: too many same type of errors. Hereafter ignore it.\n");
			}
			break;
		}
	}
	plane->nc = n;
	dr_set_ref(plane->h_nhit,0,nhit);
	dr_set_ref(plane->h_nclst,0,n);
	if(n==0)
		dr_append_ref(plane->h_posid, POSID_NOCLUST);
	else if(n==1){
#if 0
		dr_append_ref(plane->h_pos, plane->clst[0].pos);
#endif
		dr_append_ref(plane->h_posid, POSID_NOERR);
	}
	else
		dr_append_ref(plane->h_posid, POSID_MCLUST+n);

	return(0);
}

/* MWPC Position Calculation */
int chamb_mwpc_pos(plane)
		 plane_p plane;
{
	/* nothing to do */
	return(0);
}

/*
  Local Variables:
  mode: C
  tab-width: 2
  End:
*/
