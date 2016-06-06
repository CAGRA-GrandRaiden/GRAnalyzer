/*
  vdc.c ---- VDC data analysis
  Version 1.00  10-JUL-2000  by A. Tamii
  Version 1.10  21-MAY-2004  by A. Tamii (Add offset to TDC)
*/

#include <stdio.h>
#include <stdlib.h>  /* for drand48() */
#include <math.h>
#include "freadin.h"
#include "histogram.h"
#include "analyzer.h"

#define iswap(a,b) {i_swap=(a); (a)=(b); (b)=i_swap;}
static int i_swap;

//FLAG

/* VDC Clusterize */
int chamb_vdc_clust(plane)
		 plane_p plane;
{
	double  f1, f2;
	int  min, max;
	int  i, j, k, l, m, n, w, t, wp;
	int  *p;
	int  ss, se, id, idp;
	int  tdc[Clst_MaxNData];
	int  wire[Clst_MaxNData];
	int  nc;

	if(plane==(plane_p)NULL){
		fprintf(stderr, "evt_vdc_clust: null plane pointer\n");
		return(-1);
	}
	min = dr_min_ref(plane->h_wire);
	max = dr_max_ref(plane->h_wire);

	/* Get TDC data (accept one hit for each wire) */
	n=0; wp=-1; 
	for(i=max-1; i>=min; i--){
		/* Use reverse order for sorting to be faster */
		w = dr_int(dr_get_ref(plane->h_wire,i));
		t = dr_int(dr_get_ref(plane->h_tdc,i));
		if(w==wp)
			continue;
		/* take the first tdc data if multi-hits in a wire */
		wp = w;
		if(n>=Clst_MaxNData){
			// fprintf(stderr, "evt_vdc_clust: too many wire hits (>%d)\n",
			//					Clst_MaxNData);
			break;
		}
		wire[n] = w;
		tdc[n] = t;
		n++;
	}
	dr_set_ref(plane->h_nhit,0,n);

	/* Sort TDC data */
	ss=0; idp=-1;
  for(i=0; i<n; i++){
		id = wire[i]>>5;
		if(id==idp)
			continue;
		se = i-1;
		while(ss<se){ /* reverse the order */
			iswap(wire[ss],wire[se]);
			iswap(tdc[ss],tdc[se]);
			ss++; se--;
		}
		idp = id;
		ss = i;
	}
	se = i-1;
	while(ss<se){ /* reverse the order */
		iswap(wire[ss],wire[se]);
		iswap(tdc[ss],tdc[se]);
		ss++; se--;
	}
  if(1){  /* check */
		ss = -1;
		p = wire;
		for(i=0; i<n; i++){
			if(ss<*p){
				ss = *p++;
				continue;
			}

			//fprintf(stderr, "evt_vdc_clust: 3377 sorting error in %s of %s\n",
			//				plane->name, plane->chamb->name);
			//for(j=0; j<n; j++)
			//	fprintf(stderr, "%d ", wire[j]);
			//fprintf(stderr, "\n");
			//fprintf(stderr, "Re-sort\n");

			/* bubble sort */
			for(j=0; j<n-1; j++){
				for(l=j+1; l<n; l++){
					if(wire[j]>wire[l]){
						iswap(wire[j],wire[l]);
						iswap(tdc[j],tdc[l]);
					}
				}
			}

			break;
		}
	}
	for(i=0; i<n; i++){
#if 1  /* 21-MAY-2004 */
		tdc[i] += plane->tab.offset;
#endif
		dr_append_ref(plane->h_wirer,wire[i]);
		dr_append_ref(plane->h_tdcr,tdc[i]);
	}

	/* Clustering TDC data */
	nc = 0; ss=0; p=wire; k=n-1;
	for(i=0; i<n; i++){
    j = *p++;
#if VDC_ALLOW_HITSKIP
		if(i==k || (j+1!=*p && j+2!=*p))
#else
		if(i==k || j+1!=*p)
#endif
		{
			if(nc>=Clst_MaxNClst){
				if(0){
					fprintf(stderr,
						"evt_vdc_clust: too many clusters in %s of %s (>%d)\n",
									plane->name, plane->chamb->name, Clst_MaxNClst);
				}
				break;
			}
			m = i-ss+1;
			if(m<plane->min_clst_size){
				ss = i+1;
				continue;
			}
      if(m>plane->max_clst_size){
				if(0)
					fprintf(stderr, "evt_vdc_clust: too large cluster size (%d)\n", m);
				m = plane->max_clst_size;
			}
			plane->clst[nc].size = m;
			dr_append_ref(plane->h_clstsz,m);
			for(j=0; j<m; j++){
				plane->clst[nc].wire[j] = wire[ss];
        plane->clst[nc].tdc[j] = tdc[ss];
				ss++;
			}
			nc++;
			ss = i+1;
		}
	}
	plane->nc = nc;
	dr_set_ref(plane->h_nclst,0,nc);
	
	return(0);
}

/* VDC Position Calculation */
int chamb_vdc_pos(plane)
		 plane_p plane;
{
	struct clst *clst;
	double  dist[Clst_MaxClstSize];
	int     wire[Clst_MaxClstSize];
	int     i, ic, n, mid, size;
	double  m, mx, my, mx2, mxy;
	double  slope;
	double  mind;
	int     mini;

	if(plane==(plane_p)NULL){
		fprintf(stderr, "evt_pos: null plane pointer\n");
		return(-1);
	}

	if(plane->nc<=0){
		dr_append_ref(plane->h_posid, POSID_NOCLUST);
		return(-1);
	}

	/* initialize */
	for(ic=0; ic<plane->nc && ic<Clst_MaxNClst; ic++){
		clst = &plane->clst[ic];
		clst->posid = POSID_UNDEF;
		clst->pos   = 0.;
	}

	/* save time if multi clusters are not analyzed  */
	if(!plane->chamb->ana_mult_clst){
		if(plane->nc>=2){
			dr_append_ref(plane->h_posid, POSID_MCLUST+plane->nc);
			return(-1);
		}
	}
	
  for(ic=0; ic<plane->nc && ic<Clst_MaxNClst; ic++){
		clst = &plane->clst[ic];
		n    = clst->size;
		clst->posid = POSID_UNDEF;
	
		/* ignore unresolved drift distance */
		size = 0;
		mind = 1e+10;
		mini = -1;
		for(i=0; i<n; i++){
#if DEBUG_TEMP && 0
			fprintf(stderr, "clst->dist[%d]=%f\n", i, clst->dist[i]);
#endif
			if(dr_exists(clst->dist[i])){
				dist[size] = clst->dist[i];
				wire[size] = clst->wire[i];
				if(dist[size]<mind){
					mind = dist[size];
					mini = size;
				}
				size++;
			}
		}
#if 0  /* neglect the minimum distance wire */
		for(i=mini; i<size-1; i++){
			dist[i] = dist[i+1];
			wire[i] = wire[i+1];
		}
		size--;
#endif
		
		/* check monotone increase/decrease */
		/* note: 'size' might be smaller than the value deteremined in the
			 clusterinsing stage when the drift time to distance conversion
			 fails.
		*/
		switch(size){
		case 0:
			dr_append_ref(plane->h_posid, clst->posid=POSID_NOHIT);
			break;
		case 1:
			dr_append_ref(plane->h_posid, clst->posid=POSID_1HIT);
      break;
		case 2:
			if(plane->ana_2hit_clst){
				slope = (double)(wire[1]-wire[0])/(dist[0]+dist[1]);
				clst->pos  = wire[0]+slope*dist[0];
				clst->grad = slope*plane->wire.sc;
				dr_append_ref(plane->h_pos,  clst->pos);
				dr_append_ref(plane->h_grad, clst->grad);
				dr_append_ref(plane->h_posid, clst->posid=POSID_NOERR);
			}else{
				dr_append_ref(plane->h_posid, clst->posid=POSID_2HIT);
			}
      break;
		}
   if(clst->posid!=POSID_UNDEF)
		 continue;
		
#if 0 /* for debug */
		fprintf(stderr, "before dist (%d)= \n",pl);
		for(i=0; i<size; i++){
			fprintf(stderr, "%5d %9.5f\n", i, dist[i]);
		}
#endif

		for(i=1; i<size; i++)
			if(dist[i-1]<dist[i]) break;
		mid=i-1;
		for(; i<size; i++)
			if(dist[i-1]>dist[i]) break;
		if(i<size){
			if(0){
				showerr("evt_vdc_pos: drift length inconsistency.\n");
				for(i=0; i<size; i++)
					showerr("p_vdc_pos: drift[%d] = %5d\n", i, dist[i]);
			}
			dr_append_ref(plane->h_posid, clst->posid=POSID_DRIFT);
			continue;
		}
  /* check monotone increase/decrease */
		if(mid<=0 || size-1<=mid){
			dr_append_ref(plane->h_posid, clst->posid=POSID_MONOTONE);
			continue;
		}
		
#if 0  /* simplified position fitting (for debug) */
		clst->pos = clst->wire[0]
			+(double)(wire[size-1]-wire[0])*dist[0]/(dist[0]+dist[size-1]);
		dr_append_ref(plane->h_posid, clst->posid=POSID_SIMPLE);
		continue;
#endif
		
		/* change the signs of drift lengths */
		for(i=(dist[mid-1]<=dist[mid+1] ? mid-1:mid); i>=0; i--)
			dist[i] = -dist[i];
		
#if 0 /* for debug */
		fprintf(stderr, "after dist = \n");
		for(i=0; i<size; i++){
			fprintf(stderr, "%5d %9.5f\n", i, dist[i]);
		}
#endif
		
		/* get mean values */
		mx = my = mx2 = mxy = 0.;
		for(i=0; i<size; i++){
			mx  += wire[i];
			my  += dist[i];
			mx2 += wire[i]*wire[i];
			mxy += wire[i]*dist[i];
		}
		my  /= size;
		mxy /= size;
		mx  /= size;
		mx2 /= size;
		m = mx2-mx*mx;
		if(m<=0.0){
			fprintf(stderr, "evt_vdc_pos: error of least square fit(1).\n");
			dr_append_ref(plane->h_posid, clst->posid=POSID_LFIT);
			continue;
		}
		slope = (mxy-mx*my)/m;
		if(fabs(slope)<=1e-10){
			fprintf(stderr, "evt_vdc_pos: error of least square fit(2).\n");
			dr_append_ref(plane->h_posid, clst->posid=POSID_LFIT);
			continue;
		}
		clst->pos  = mx-my/slope;
		clst->grad = plane->wire.sc/slope;
		dr_append_ref(plane->h_posid, clst->posid=POSID_NOERR);
		dr_append_ref(plane->h_pos, clst->pos);
		dr_append_ref(plane->h_grad, clst->grad);
#if 0 /* for DEBUG */
		if(ic==0 && !strcmp(plane->name, "X1")){
			for(i=0; i<size; i++){
				printf("%5d %10.3f\n", wire[i], dist[i]);
			}
			printf("\n");
		}
#endif
	} /* loop of ic */
	return(0);
}

/*
  Local Variables:
  mode: C
  tab-width: 2
  End:
*/
