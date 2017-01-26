/*
  fread_rcnp.c --- rawdata readout program for analyzer (rcnp TAMIDAQ format)
  Author:   A. Tamii
  Facility: Department of Physics, University of Tokyo
  Created:  04-MAY-1997 by A. Tamii
  Version 1.00 04-MAY-1997 by A. Tamii (fread)
  Version 1.22 06-MAY-1997 by A. Tamii (fread)
  Version 2.00 03-JUN-2000 by A. Tamii
  Version 2.10 07-JAN-2008 by A. Tamii (Uchida format comment conversion
	                                     with byte swap)
  Version 2.20 04-NOV-2014 by A. Tamii (for V830)
*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <fcntl.h>
#ifdef OSF1
#include <strings.h>
#endif
#include <time.h>
#include <stdarg.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/msg.h>
#include <signal.h>
#include <errno.h>

#include "freadin.h"
#include "histogram.h"
#include "analyzer.h"
#include "mtformat.h"
#include "lr3377.h"
#include "fera.h"
#include "pcos.h"
#include "v1190.h"
#include "madc32.h"

//FLAG

extern int neval;
extern int level;
extern int swap;
extern int byte_order;
int parse_error;

extern int pflag;
extern int cflag;

extern int nrun;
extern int nblk;
extern int cblk;
static int nevt = 0;
int nblk_add  = 0;
int nblk_flag = 0;

#define USE_SEQ_NUMBER_IN_DATA  1  // Use block number and event number in the data

static char *module[16] = {
	"Irregal ", "VDC-OLD ", "NIM-IN  ", "ADC     ",
	"TDC     ", "PCOS-OLD", "Scaler  ", "LR3377  ",
	"Reserved", "VDC     ", "PCOS    ", "ADC-LAS ",
	"TDC-LAS ", "FERA    ", "FERET   ", "CheckSum"
	};

static char *fera_name[16] = {
  "GR_FERA_ADC", "LAS_FERA_ADC", "GSO_FERA_ADC", "SSD_FERA_ADC",
  "GR_3351_ADC", "LAS_3351_ADC", "GSO_3351_ADC", "SSD_3351_ADC",
  "GR_FERA_TDC", "LAS_FERA_TDC", "GSO_FERA_TDC", "SSD_FERA_TDC",
  "GR_3351_TDC", "LAS_3351_TDC", "GSO_3351_TDC", "SSD_3351_TDC"
};
static int  h_fera_data_id[16];

//-- Added on 2017.1.25 by A. Tamii
static int get_qtc_ch(geo,ch){
	// modified on 2017.1.20
	if(geo<0 || geo==2 || geo==6 || 8<=geo) return -1; // correct
	//	if(geo<0 || 8<=geo) return -1; // wrong for U1 and U2 channel 0-15
	if(ch<96 || 112<=ch) return -1;
	return (geo*16+(ch-96));
}
//-- End added

#if 1 // 13-July-2014
#define V1190_BASE_TIME_CH (127)
#define V1190_BASE_TIME_CH2 (0)
#define V1190_BUFCH_CH     (126)
#else
#define V1190_BASE_TIME_CH (112)
#endif

/* read region */
static int read_rgn(buf, size)
		 unsigned short  *buf;
		 int             size;
{
	unsigned short *ptr, *tp,  *te;
	unsigned short *rgn, *pv;
	lr3377_header  lr3377;
	lr3377_mid     lr3377mid;
	fera_header    fera;
	fera_data      fdata;
	pcos_data      pcos;
	int nrgn;
	int rgn_size;
	unsigned int rgn_id, rgn_32bits;
	int i;
	int type, id;
	int data;
	double ddata;
	double pdata; //-- added on 2017.1.25 by A. Tamii
  int idata; // --- newer code 2016.11.9 according to the discussion with Y. Watanabe --- // inserted on 2017.1.25 by A. Tamii
	int mid;
	int pl;
	int ch;
	int qtc_ch, detector_ch;
	int wire, w, wire2;
	V1190_DATA_t   v1190;
	int geo=0;
	int base_time[V1190_MAX_N_MODULES];
  unsigned long long time_stamp;

	MADC32_HEADER_SIGNATURE_p    madc32_signature;
	MADC32_DATA_HEADER_p         madc32_header;
	MADC32_DATA_EVENT_p          madc32_event;
	MADC32_EXTENDED_TIME_STAMP_p madc32_extended_time_stamp;
	MADC32_FILL_p                madc32_fill;
	MADC32_END_OF_EVENT_p        madc32_end_of_event;
	int madc_geo, madc_ch, madc_amplitude,j;

	ptr = buf;
	te = &buf[size];
	nrgn = 0;
	while(ptr < te){
		rgn = ptr;
		nrgn++;
		
		/* get the next region header */
		rgn_id = *rgn & ModuleIDMask;
		rgn_size = *rgn & DataLengthMask;
#if 0
		showerr("data = %xH\n", *rgn);
#endif
		rgn++;

		/* check the region header */
		switch(rgn_id){
		case ID_Reserved:
			showerr("Illegal region ID. ID = %xH\n", rgn_id);
			if(ptr < te){
				/* search the next field header */
				showerr("  ... Searching for the next Region Header\n");
				for(; rgn<te; rgn++){
					rgn_id = *rgn>>12;
					break;
				}
				rgn_size = ((long)rgn-(long)ptr)/2-1;
			}
			break;
		case ID_NimIn:
			for(i=0; i<rgn_size && rgn<te; i++){
				dr_append(IPR,*rgn++);
			}
			break;
		case ID_MADC32:
			for(i=0; i<rgn_size/2 && rgn<te; i++){	// rgn = 'data block', te = 'last element of data block'
				rgn_32bits = rgn[0] | (rgn[1]<<16);	// convert 16 bits rgn to 32 bits, as our words
				rgn += 2; // move to the next word for next iteration over the event
				madc32_signature = (MADC32_HEADER_SIGNATURE_p)&rgn_32bits;
				switch(madc32_signature->header){
					case MADC32_HEADER_SIGNATURE_HEADER:
						madc32_header = (MADC32_DATA_HEADER_p)&rgn_32bits;
						madc_geo = madc32_header->module_id;
						break;
					case MADC32_HEADER_SIGNATURE_DATA:
						switch(madc32_signature->subheader){
							case MADC32_SUBHEADER_EVENT:
								madc32_event = (MADC32_DATA_EVENT_p)&rgn_32bits;
								madc_ch = madc32_event->channel_number;
								madc_amplitude = madc32_event->adc_amplitude;
								switch (madc_geo){
									case 0:
										dr_append(MADC32_0_CH, madc_ch);
										dr_append(MADC32_0_ADC, (double)madc_amplitude);
										break;
									case 1:
										dr_append(MADC32_1_CH, madc_ch);
										dr_append(MADC32_1_ADC, (double)madc_amplitude);
										break;
									case 2:
										dr_append(MADC32_2_CH, madc_ch);
										dr_append(MADC32_2_ADC, (double)madc_amplitude);
										break;
								}
								break;
							case MADC32_SUBHEADER_EXTENDED_TIME_STAMP: // unused at the moment
								break;
							case MADC32_SUBHEADER_FILL: // unused at the moment
								break;	
						}
						break;
					case MADC32_HEADER_SIGNATURE_END_OF_EVENT: // unused at the moment
						break;
				}
			}
			break;
		case ID_MYRIAD:
			if(rgn_size==4){
				rgn++; // skip the MyRIAD Header
				time_stamp
					= ((unsigned long long)(rgn[0])<<32)
					| ((unsigned long long)(rgn[1])<<16)
					| ((unsigned long long)(rgn[2])<< 0);
				rgn+=3;
				dr_append(GR_MYRIAD, (double)time_stamp);
			}else{
				fprintf(stderr, "Unexpected MyRIAD data length (%d)\n", rgn_size);
			}
			break;
		case ID_V830:
			for(i=0; i<rgn_size && rgn<te; i+=2){
				dr_append(GR_V830, (rgn[0] | (rgn[1]<<16)));
				rgn+=2;
			}
			break;
		case ID_ADC_LAS:
			for(i=0; i<rgn_size && rgn<te; i++){
				dr_append(LAS_ADC_OLD, *rgn++);
			}
			break;
		case ID_TDC_LAS:
			for(i=0; i<rgn_size && rgn<te; i++){
				dr_append(LAS_TDC_OLD, (*rgn++ & 0x0FFF));
			}
			break;
		case ID_3377:
			// break;  // 26-APR-2013 cut 3377 data
			type = -1;
			for(i=0; i<rgn_size && rgn<te; i++){
				*(short*)&lr3377 = *rgn++;
				if(lr3377.hdr){
					type = lr3377.id >> 4;
          id   = (lr3377.id & 0x000f)<<5;
				}else{
					ch   = ((lr3377_data*)&lr3377)->ch;
					wire = id+ch;
          data  = ((lr3377_data*)&lr3377)->data;
					switch(type){
					case 0x00:
						dr_append(GR_WIRE_X1, (double)wire);
						dr_append(GR_TDC_X1, (double)data);
						break;
					case 0x01:
						dr_append(GR_WIRE_BASE, (double)wire);
						dr_append(GR_TDC_BASE, (double)data);
						break;
					case 0x02:
						dr_append(GR_WIRE_U1, (double)wire);
						dr_append(GR_TDC_U1, (double)data);
						break;
					case 0x03:
						dr_append(GR_WIRE_V1, (double)wire);
						dr_append(GR_TDC_V1, (double)data);
						break;
					case 0x04:
						dr_append(GR_WIRE_X2, (double)wire);
						dr_append(GR_TDC_X2, (double)data);
						break;
					case 0x05:
						dr_append(GR_WIRE_Y2, (double)wire);
						dr_append(GR_TDC_Y2, (double)data);
						break;
					case 0x06:
						dr_append(GR_WIRE_U2, (double)wire);
						dr_append(GR_TDC_U2, (double)data);
						break;
					case 0x07:
						dr_append(GR_WIRE_V2, (double)wire);
						dr_append(GR_TDC_V2, (double)data);
						break;
					case 0x08:
						dr_append(LAS_WIRE_X1, (double)wire);
						dr_append(LAS_TDC_X1, (double)data);
						break;
					case 0x09: /* E154 05DEC */
						w = wire & 0x3f;
            switch(wire>>6){
						case 0x00:
							dr_append(LF_WIRE_X1, (double)w);
							dr_append(LF_TDC_X1, (double)data);
							break;
						case 0x01:
							dr_append(LF_WIRE_X2, (double)w);
							dr_append(LF_TDC_X2, (double)data);
							break;
						case 0x02:
							dr_append(LF_WIRE_X3, (double)w);
							dr_append(LF_TDC_X3, (double)data);
							break;
						case 0x03:
							dr_append(LF_WIRE_X4, (double)w);
							dr_append(LF_TDC_X4, (double)data);
							break;
						}
						break;
					case 0x0a:
						if(wire<50 || 200<=wire){
							break;
						}
						dr_append(LAS_WIRE_U1, (double)wire);
						dr_append(LAS_TDC_U1, (double)data);
						break;
					case 0x0b:
						if(wire<50 || 200<=wire){
							break;
						}
						dr_append(LAS_WIRE_V1, (double)wire);
						dr_append(LAS_TDC_V1, (double)data);
						break;
					case 0x0c:
						dr_append(LAS_WIRE_X2, (double)wire);
						dr_append(LAS_TDC_X2, (double)data);
						break;
					case 0x0d: /* E154 05DEC */
						w = (wire & 0x3e)>>1;
            switch((wire>>6)*2+(wire&0x01)){
						case 0x00:
							dr_append(LF_WIRE_Y1, (double)w);
							dr_append(LF_TDC_Y1, (double)data);
							break;
						case 0x01:
							dr_append(LF_WIRE_Y2, (double)w);
							dr_append(LF_TDC_Y2, (double)data);
							break;
						case 0x02: /* folowing order of Y4 and Y3 is correct */
							dr_append(LF_WIRE_Y4, (double)w);
							dr_append(LF_TDC_Y4, (double)data);
							break;
						case 0x03:
							dr_append(LF_WIRE_Y3, (double)w);
							dr_append(LF_TDC_Y3, (double)data);
							break;
						}
						break;
					case 0x0e:
						if(wire<50 || 200<=wire){
							break;
						}
						dr_append(LAS_WIRE_U2, (double)wire);
						dr_append(LAS_TDC_U2, (double)data);
						break;
					case 0x0f:
						if(wire<50 || 200<=wire){
							break;
						}
						dr_append(LAS_WIRE_V2, (double)wire);
						dr_append(LAS_TDC_V2, (double)data);
						break;
					default:
						showerr("Unsupported 3377 Type (%d)\n", type);
						rgn++;
						break;
					}
				}
			}
			break;			
///////////////////////////////////////////////////////////////////////////////			
		case ID_V1190:
			pv = rgn;
			
      	for(i=0; i<V1190_MAX_N_MODULES; i++){
        		base_time[i] = -10000;
			}
			
			for(i=0; i<rgn_size/2 && pv<te; i++){
        		v1190.idata = pv[0] | (pv[1]<<16);
				pv += 2;
				switch(v1190.global_header.id){
					case V1190_HEADER_ID_GH:
						geo = v1190.global_header.geo;
          			break;
					case V1190_HEADER_ID_TM:
						wire = v1190.tdc_measurement.channel;
						data = v1190.tdc_measurement.measurement;
#if 0
						fprintf(stderr, "Module geo= %1d, Wire = %3d, data = %7d\n",geo, wire, data);
#endif
						if(geo<8){
							// GR time reference is in chan 127
							if(wire==V1190_BASE_TIME_CH && !v1190.tdc_measurement.trailing){
								base_time[geo] = data;
								dr_set(V1190BASE[geo],data);
							}
						}
						else if(geo>7 && geo<22){
							// LAS X1 plane, 3rd module is a particular case (even and chan 127)
							if (wire==V1190_BASE_TIME_CH && (geo==10) && !v1190.tdc_measurement.trailing){
								base_time[geo] = data;
								dr_set(V1190BASE[geo],data);
							}
							// LAS odd geo modules time reference is in chan 127
							else if(wire==V1190_BASE_TIME_CH && (geo%2 == 1) && !v1190.tdc_measurement.trailing){
								base_time[geo] = data;
								dr_set(V1190BASE[geo],data);
							}
							// LAS even geo modules time reference is in chan 0
							else if(wire==V1190_BASE_TIME_CH2 && (geo%2 == 0) && !v1190.tdc_measurement.trailing){
								base_time[geo] = data;
								dr_set(V1190BASE[geo],data);
							}
						}
#if 0
						else if(wire==V1190_BUFCH_CH){
             			dr_set(BUFCH[geo],data);
						}
#endif
				}
			}
		
			for(i=0; i<rgn_size/2 && rgn<te; i++){
        		v1190.idata = rgn[0] | (rgn[1]<<16);
				rgn += 2;
				switch(v1190.global_header.id){
					case V1190_HEADER_ID_GH:
          			geo = v1190.global_header.geo;
          			break;
					case V1190_HEADER_ID_TM:
						ch = v1190.tdc_measurement.channel;
						//if(base_time[geo]<-1000)
							//fprintf(stderr, "no base time for geo %d\n", geo);
#if 1     // --- newer code 2016.11.9 according to the discussion with Y. Watanabe --- // inserted on 2017.1.25 by A. Tamii
					idata = (v1190.tdc_measurement.measurement - base_time[geo])&0x7FFFF;
					if(idata>=0x40000) idata -= 0x80000;
					ddata = idata/10.0;
#else  		// ---older code---
					ddata = (v1190.tdc_measurement.measurement - base_time[geo])/10.0;
#endif							
						wire = v1190_wire_map(geo,ch);
						wire2 = (geo%2)*128+ch;
						dr_append(V1190_RAW_CH, geo*128+wire);
						dr_append(V1190_RAW_TDC, v1190.tdc_measurement.measurement);
						ddata = -ddata;  // for online VDC2013 25-SEP-2013

						// if QTC
						//qtc_ch = (get_qtc_ch(geo,ch))%16; // only 16 chan at the moment
						if (geo==0 && (ch>=96 && ch<112)) {
							qtc_ch = ch%16;
							if(v1190.tdc_measurement.trailing) {
								dr_append(QTC_TRAILING_CH, qtc_ch);
								dr_append(QTC_TRAILING_TDC, v1190.tdc_measurement.measurement - base_time[geo]);
							}
							else {
								dr_append(QTC_LEADING_CH, qtc_ch);
								dr_append(QTC_LEADING_TDC, v1190.tdc_measurement.measurement - base_time[geo]);
							}
						}
						
//-- Added on 2017.1.25 by Tamii
					qtc_ch = get_qtc_ch(geo,ch); 
					if(qtc_ch>=0){
						ddata = v1190.tdc_measurement.measurement - base_time[geo];
						if(v1190.tdc_measurement.trailing){
					    pdata = dr_get(QTC_TRAILING[qtc_ch]);
							if(dr_is_nothing(pdata) || pdata>ddata){  // use the first data
									dr_set(QTC_TRAILING[qtc_ch], ddata);
				      }
     				}else{
					    pdata = dr_get(QTC_LEADING[qtc_ch]);
					    if (dr_is_nothing(pdata) || pdata>ddata){  // use the first data
					      dr_set(QTC_LEADING[qtc_ch],  ddata);
							}
				    }
				  }
//--
						if(ch==V1190_BUFCH_CH){
							dr_set(BUFCH[geo], ddata);
						}
						// if we have a leading that is not from other stuff
						if(!v1190.tdc_measurement.trailing){
							if(wire>=0 && geo<8){
								switch(v1190_plane_map(geo,ch)){
         						case 0: // X1 Plane
										dr_append(GR_WIRE_X1, (double)wire);
										dr_append(GR_TDC_X1, ddata);
										break;
									case 1: // U1 Plane
										dr_append(GR_WIRE_U1, (double)wire);
										dr_append(GR_TDC_U1, ddata);
									  	break;
									case 2: // X2 Plane
										dr_append(GR_WIRE_X2, (double)wire);
										dr_append(GR_TDC_X2, ddata);
									 	break;
									case 3: // U2 Plane
										dr_append(GR_WIRE_U2, (double)wire);
										dr_append(GR_TDC_U2, ddata);
									  	break;
									}
							}
							else if(geo>7 && geo<22){//for LAS VDC modified 19-Apr-2016
								switch((int)(geo/2)){
									case 4://X1 plane 
										dr_append(LAS_WIRE_X1, (double)wire2);
										dr_append(LAS_TDC_X1, ddata);
										break;
									case 5://X1 plane, 3rd module
										wire2 = 128+16+ch; // ECL plugged on D input
										dr_append(LAS_WIRE_X1, (double)wire2);
										dr_append(LAS_TDC_X1, ddata);
										break;
									case 6://U1 plane 
										dr_append(LAS_WIRE_U1, (double)wire2);
										dr_append(LAS_TDC_U1, ddata);
										break;
									case 7://V1 plane 
										dr_append(LAS_WIRE_V1, (double)wire2);
										dr_append(LAS_TDC_V1, ddata);
										break;
									case 8://X2 plane 
										dr_append(LAS_WIRE_X2, (double)wire2);
										dr_append(LAS_TDC_X2, ddata);
										break;
									case 9://U2 plane 
										dr_append(LAS_WIRE_U2, (double)wire2);
										dr_append(LAS_TDC_U2, ddata);
										break;
									case 10://V2 plane 
										dr_append(LAS_WIRE_V2, (double)wire2);
										dr_append(LAS_TDC_V2, ddata);
										break;
									}
							}
							/*else if(geo == 24){
								detector_ch = ch;
								if(v1190.tdc_measurement.trailing) {
									dr_append(SSD_TRAI_CH, detector_ch);
									dr_append(SSD_TRAI_TDC, v1190.tdc_measurement.measurement - base_time[geo]);
								}
								else {
									dr_append(SSD_LEAD_CH, detector_ch);
									dr_append(SSD_LEAD_TDC, v1190.tdc_measurement.measurement - base_time[geo]);
								}
							}*/
					}
	  					break;
					case V1190_HEADER_ID_TH:
					case V1190_HEADER_ID_TT:
					case V1190_HEADER_ID_TE:
					case V1190_HEADER_ID_ETTT:
					case V1190_HEADER_ID_GT:
					case V1190_HEADER_ID_FILLER:
						break;
				}
			}
			break;
///////////////////////////////////////////////////////////////////////////////			
		case ID_FERA_ADC:
		case ID_FERA_TDC:
			type = 0;
			mid  = 0;
			for(i=0; i<rgn_size && rgn<te; i++){
				*(short*)&fera = *rgn;
				if(fera.hdr){
          type = (fera.vsn & 0x00f0) >> 4;
					mid  = fera.vsn & 0x000f;
					rgn++;
				}else{
					*(short*)&fdata = *rgn++;
					dr_set_r(&dr_data,&h_fera_data_id[type],fera_name[type], mid*16+fdata.ch, fdata.data);
				}
			}
			break;
		case ID_Scaler:
			for(i=0; i<rgn_size; i+=2){
				if((rgn+1)>=te) break;
				if(byte_order==BIG_ENDIAN){
					((short*)&data)[1] = *rgn++;
					((short*)&data)[0] = *rgn++;
				}else{
					((short*)&data)[0] = *rgn++;
					((short*)&data)[1] = *rgn++;
				}
				dr_append(SCALER, data);
			}
			break;
		case ID_CHKSUM:
			break;
		default:
			//showerr("Never come here. Region ID= %xH\n", rgn_id>>12);
			break;
		}
		tp = &ptr[rgn_size+1];
		if(tp>te){
			showerr("Errorr in region size. size = %xH\n", rgn_size);
		}

		/* advance to the next region */
		ptr += rgn_size+1;
	}
	return nrgn;
}

/* read field */
static int read_fld(buf, size)
		 unsigned short  *buf;
		 int             size;
{
	unsigned short *ptr, *tp,  *te;
	FldHeaderPtr   fld;
	int nfld, nrgn;
	int fld_size;
	int chksum, *sump;

	ptr = buf;
	te = &buf[size];
	nfld = 0;
	while(ptr < te){
		/* search the next field header */
		if(*ptr != FldHeaderID){
			showerr("Searching for the next Field Header\n");
			while(ptr < te){
				if(*ptr==FldHeaderID)
					break;
				ptr++;
			}
		}
		if(ptr >= te)
			break;

		/* check the field header */
		nfld++;
		fld = (FldHeaderPtr)ptr;
		if(fld->headerID != FldHeaderID){
			showerr("Error in field header ID. ID = %xH\n", fld->headerID);
		}
		if(fld->headerSize != sizeof(FldHeader)/2){
			showerr("Error in filed header size. ID = %xH\n", fld->headerSize);
		}
		if((fld->fieldID & ~7)!= FieldID){
			showerr("Error in field ID. ID = %xH\n", fld->fieldID);
		}
		tp = &ptr[fld->headerSize+fld->fieldSize];
		if(tp>te || (tp<te && *tp!=FldHeaderID) ){
			showerr("Error in field size. size = %xH\n", fld->fieldSize);
		}
		ptr += fld->headerSize;

#if CHKSUM
		/* check sum */
		chksum = 0;
		sump = ptr;
		while(sump<te)
			chksum += *sump++;
		if(chksum!=0){
			showerr("Field check sum error. Sum = %xH\n", chksum);
		}
#endif
		
		/* check the regions */
		if(ptr<=te){
			nrgn = read_rgn(ptr, fld->fieldSize);
		}
		
		/* advance to the next field */
		ptr += fld->fieldSize;
	}
	return nfld;
}

/* read_evt */
static int read_evt(buf, size)
		 unsigned short  *buf;
		 int             size;
{
	unsigned short *ptr, *tp, *te;
	EvtHeaderPtr   evt;
	int nfld;
	int evt_size;

	ptr = buf;
	te = &buf[size];
	nevt = 0;
	while(1){
		if(*ptr == BlkTrailerID)
			break;
		/* search the next event header */
		if(*ptr != EvtHeaderID){
			showerr("Searching for the next event header\n");
			while(ptr < te){
				if(*ptr==EvtHeaderID || *ptr==BlkTrailerID)
					break;
				ptr++;
			}
		}
		if(ptr >= te || *ptr == BlkTrailerID )
			break;

		/* read the event header */
		nevt++;
		evt = (EvtHeaderPtr)ptr;
		if(evt->headerID != EvtHeaderID){
			showerr("Error in event header ID. ID = %xH\n", evt->headerID);
		}
		if(evt->headerSize != sizeof(EvtHeader)/2){
			showerr("Error in event header size. size = %xH\n", evt->headerSize);
		}
		if(evt->eventID != DataEvent && evt->eventID != BlockEndEvent){
			showerr("Error in event ID. ID = %xH\n", evt->eventID);
		}
		tp = &ptr[evt->headerSize+evt->eventSize];
		if(tp>te){
			showerr("Error in event size. size = %xH\n", evt->eventSize);
			break;
		}
		if(tp<te && *tp != EvtHeaderID && *tp != BlkTrailerID ){
			showerr("Error in event size. size = %xH\n", evt->eventSize);
		}
		if(evt->eventNumber != nevt-1){
			showerr("Error in event number. number = %d\n",
							evt->eventNumber);
			nevt = evt->eventNumber+1;
			showerr("  ... Adjust the event number.\n");
		}
		ptr += evt->headerSize;
		
		dr_clear();
		dr_set(RUN_N[0], nrun);
#if USE_SEQ_NUMBER_IN_DATA
		dr_set(EVENT_N[0], evt->eventNumber);
		dr_set(BLOCK_N[0], nblk);
#else
		dr_set(EVENT_N[0], nevt);
		dr_set(BLOCK_N[0], nblk);
#endif
    dr_set(EVENT_RCNP[0], 1);
		/* read the fields */
		if(ptr<=te){
			nfld= read_fld(ptr, evt->eventSize);
			if(evt->numFields != nfld){
				showerr("Error in the number of fields. num. of fields = %d\n",
								evt->numFields);
			}
		}
		if(pflag){
		}else{
			dr_event();
		}
		
		/* advance to the next event */
		ptr += evt->eventSize;
	}
	return nevt;
}

static void strncpy_uchida(dst, src, len)
		 unsigned char *dst;
		 unsigned short *src;
		 int  len;
{
	int  i;
	char c;
	
	for(i=0; i<len; i++){
		c = (*src++) & 0x00ff;
		if(c==0x0a) continue;
		*dst++ = c;
		if(c==0x00)
			break;
	}
}

/* read comment block */
static int read_com(buf, size)
		 unsigned short  *buf;
		 int             size;
{
	RunCommentPtr com;
  time_t        t;
	unsigned int  time2;
	char str[MaxComLen+10];
	char sstr[MaxComLen+10];
	com = (RunCommentPtr)buf;
	if(size<sizeof(RunComment)/2){
		showerr("Error in comment block size\n");
		return -1;
	}

	/*---
		The following parts were modified on 07-JAN-2008
		for Uchida-system and proper treatment of the
		byte-swap
		---*/
	
	if(swap){
		//swaps(&com->version);
		//swaps(&com->time);
	}else{
	  swaps(&com->byte);
	}

  /* copy and convert the comment from Uchida version to normal one */	
  strncpy_uchida((unsigned char*)str, (unsigned short*)com->comment, MaxComLen);
	//if(swap)
	//	swab(str, sstr, MaxComLen);
	strncpy(sstr, str, MaxComLen);
	
	if(swap)
		swaps(&com->time);     // for Uchida version
  t = (time_t)com->time;

	/*----
		End of modification on 07-JAN-2008
		---*/

	printf("Format Ver = %d.%d\n", com->version>>8, com->version&0x00FF);
	printf("Byte Order = %x\n", com->byte);
	printf("Time Stamp = %s", ctime(&t));
	printf("Run Number = %d\n", com->run);
	printf("Comment    = %s\n", sstr);
  nrun = com->run;

	return 0;
}
	
/* read block */
int read_blk_rcnp(buf, size)
		 unsigned short  *buf;
		 int             size;
{
	unsigned short *ptr;
	BlkHeaderPtr   blk;
	BlkTrailerPtr  trl;
	ptr = buf;

	/* check the block header */
	blk = (BlkHeaderPtr)ptr;
	
	if(blk->headerID != BlkHeaderID){
		showerr("Error in block header ID. ID = %xH\n", blk->headerID);
	}
	if(blk->headerSize != sizeof(BlkHeader)/2 && blk->headerSize != sizeof(BlkHeaderV3)/2){
		showerr("Error in block header size. size = %xH\n",
						blk->headerSize);
	}
	switch(blk->blockID){
	case DataBlockID+0: /* Crate 0 */
	case DataBlockID+1: /* Crate 1 */
	case DataBlockID+2: /* Crate 2 */
	case DataBlockID+3: /* Crate 3 */
	case DataBlockID+4: /* Crate 4 */
	case DataBlockID+5: /* Crate 5 */
	case DataBlockID+6: /* Crate 6 */
	case DataBlockID+7: /* Crate 7 */
	case ScalerBlockID:
		break;
	case StartBlockID:
		printf("Run Start Block\n");
		break;
	case EndBlockID:
		printf("Run End Block\n");
		break;
	default:
		showerr("Unknown block ID. ID = %xH\n", blk->blockID);
	}
#if 0
	if(blk->blockSize32 > size-sizeof(BlkHeader)/2){
		showerr("Error in block size. size = %xH\n", blk->blockSize32);
	}
#endif
	if((blk->blockID & ~7)==DataBlockID){
		/* for counting blocks above 65535 */
		if(nblk_flag){
			if(blk->blockNumber<0x08000){
				nblk_flag = 0;
				nblk_add += 0x10000;
			}
		}else{
			if(blk->blockNumber>=0x08000){
				nblk_flag = 1;
			}
		}                       
		nblk++;
		if(blk->blockNumber != (nblk&0xFFFF)){
#if 0
			showerr("Error in block number. number = %d, should be %d\n",
							blk->blockNumber, nblk);
			showerr("  ... Adjust the block number.\n");
#endif
			nblk = blk->blockNumber+nblk_add;
		}
	}
	ptr += blk->headerSize;
#if 0
	fprintf(stderr, "Block = %d\n", nblk);
#endif

	/* read information block */
	switch(blk->blockID){
	case StartBlockID:
		read_com(ptr, size-sizeof(BlkHeader)/2);
		//		nblk--;
		cblk++;
		return(0);
	case EndBlockID:
		read_com(ptr, size-sizeof(BlkHeader)/2);
		nblk--;
		cblk++;
#if ENDRUNBLK
		showerr("Avoid the rest of files. \n");
		return(1);
#else
		save_scalers();
		save_histograms();
		return(0);
#endif
	}

	/* read events */
	if(!evt_blk(nblk)&&!pflag){ /* chack analyze block range */
		//if(!evt_blk(nblk)){ /* chack analyze block range */
		(void)read_evt(ptr, size-(sizeof(BlkHeader)+sizeof(BlkTrailer))/2);
		if(blk->numEvents != nevt){
			showerr("Inconsistent event number. number = %d, detected events = %d\n",
							blk->numEvents, nevt);
		}
	}
	
	ptr += (blk->blockSize32_l|(blk->blockSize32_u<<16)) - sizeof(BlkTrailer)/2;
	
	/* read block trailer */
	if(&ptr[sizeof(BlkTrailer)/2]>&buf[size]){
		showerr("Incomplete block trailer.\n");
	}else{
		trl = (BlkTrailerPtr)ptr;
		if(trl->trailerID != BlkTrailerID){
			showerr("Error in block trailer ID. ID = %x\n", trl->trailerID);
			fprintf(stderr, "block number = %d, ptr = %lx\n", nblk, (size_t)ptr-(size_t)buf);
		}
		if(trl->trailerSize != sizeof(BlkTrailer)/2){
			showerr("Error in block trailer size. size = %d\n", trl->trailerSize);
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
