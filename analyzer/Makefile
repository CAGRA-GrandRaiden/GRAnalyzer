# Make file for analyzer program
# Version 1.00  02-JUN-2000  by A. Tamii

#.SILENT:

#******************
#* Compiler etc.  *
#******************
# Directories
SDIR = src
HDIR = include
ODIR = obj
EDIR = bin
LDIR = lib
ROOTDIR = /usr/local/root/v5.34.32.Linux-slc6-x86_64-gcc4.4

# Common
CC = cc
FC = gfortran
AR = ar
LEX = flex
YACC = bison -v -d
AHIST = awk -f bin/hist.awk
PHIST = perl bin/moveline.pl
CFLAGS = -I. -I$(HDIR) -I$(SDIR) \
         -D`uname -m` -D`uname -s` -DLinux86 -Df2cFortran \
         -DUSE_PAW
ROOTFLAGS = -I$(ROOTDIR)/include `root-config --cflags --glibs`
LDFLAGS = -lm
VARIOUSLIB = -L$(LDIR) -lpacklib
GPP = g++ $(CFLAGS) $(CPPFLAGS) $(ROOTFLAGS) -c -g -O3
COMPILE.c = $(CC) $(CFLAGS) $(CPPFLAGS) -c -g -O3
LINK.c = g++ $(CFLAGS) $(CPPFLAGS) $(LDFLAGS)

# for Linux (miho-2)
FLIB  = -L/usr/lib/gcc/x86_64-redhat-linux/4.4.4 -lgfortran -lnsl

# for MacOSX
#FLIB  = -L/sw/lib/gcc4.8/lib -lgfortran

#*************************
#* Command Line Switches *
#*************************
TARGET = $(EDIR)/analyzer $(EDIR)/ask

OBJ  =	$(ODIR)/analyzer.o \
	$(ODIR)/chamb.o \
	$(ODIR)/vdc.o \
	$(ODIR)/mwdc.o \
	$(ODIR)/mwpc.o \
	$(ODIR)/chamb_rcnp.o \
	$(ODIR)/chamb_np.o \
	$(ODIR)/chamb_gr_fpp.o \
	$(ODIR)/chamb_las_fpp.o \
	$(ODIR)/chamb_f2.o \
	$(ODIR)/ray.o \
	$(ODIR)/mwdc_fit.o \
	$(ODIR)/matrix.o \
	$(ODIR)/fread.o \
	$(ODIR)/fread_rcnp.o \
	$(ODIR)/fread_mars.o \
	$(ODIR)/histogram.o \
	$(ODIR)/fread_dst.o \
	$(ODIR)/fread_root.o \
	$(ODIR)/proc_hb.o \
	$(ODIR)/lex.yy.o \
	$(ODIR)/freadin.tab.o \
	$(ODIR)/dummy.o \
	$(ODIR)/hb.o \

HDR  = $(HDIR)/analyzer.h $(HDIR)/histogram.h $(HDIR)/freadin.h

all: $(TARGET)

$(ODIR)/hb.o: $(SDIR)/hb.f
	$(FC) -c $(FFLAGS) -o $@ $<

$(ODIR)/dummy.o: $(SDIR)/dummy.c
	$(COMPILE.c) $(CFLAGS) -o $@ $<

$(ODIR)/proc_hb.o: $(SDIR)/proc_hb.c
	$(COMPILE.c) -o $@ $<

$(ODIR)/histogram.o: $(SDIR)/histogram.c $(HDIR)/histogram.h $(SDIR)/freadin.tab.c
	$(COMPILE.c) -o $@ $<

$(ODIR)/fread.o: $(SDIR)/fread.c $(HDR)
	$(COMPILE.c) $< -o $@

$(ODIR)/fread_rcnp.o: $(SDIR)/fread_rcnp.c $(HDR)
	$(AHIST) $< > $<.c
	$(PHIST) $<.c
	$(COMPILE.c) $<.c -o $@

$(ODIR)/fread_mars.o: $(SDIR)/fread_mars.c $(HDR)
	$(AHIST) $< > $<.c
	$(PHIST) $<.c
	$(COMPILE.c) $<.c -o $@

$(ODIR)/fread_dst.o: $(SDIR)/fread_dst.c $(HDR)
	$(AHIST) $< > $<.c
	$(PHIST) $<.c
	$(COMPILE.c) $<.c -o $@

$(ODIR)/fread_root.o: $(SDIR)/fread_root.c $(HDR)
	$(AHIST) $< > $<.c
	$(PHIST) $<.c
	$(GPP) $<.c -o $@

$(ODIR)/analyzer.o: $(SDIR)/analyzer.c $(HDR)
	$(AHIST) $< > $<.c
	$(PHIST) $<.c
	$(COMPILE.c) $<.c -o $@

$(ODIR)/chamb_f2.o: $(SDIR)/chamb_f2.c $(HDR)
	$(AHIST) $< > $<.c
	$(PHIST) $<.c
	$(COMPILE.c) $<.c -o $@

$(ODIR)/chamb_np.o: $(SDIR)/chamb_np.c $(HDR)
	$(AHIST) $< > $<.c
	$(PHIST) $<.c
	$(COMPILE.c) $<.c -o $@

$(ODIR)/chamb_gr_fpp.o: $(SDIR)/chamb_gr_fpp.c $(HDR)
	$(AHIST) $< > $<.c
	$(PHIST) $<.c
	$(COMPILE.c) $<.c -o $@

$(ODIR)/chamb_las_fpp.o: $(SDIR)/chamb_las_fpp.c $(HDR)
	$(AHIST) $< > $<.c
	$(PHIST) $<.c
	$(COMPILE.c) $<.c -o $@

$(ODIR)/chamb_rcnp.o: $(SDIR)/chamb_rcnp.c $(HDR)
	$(AHIST) $< > $<.c
	$(PHIST) $<.c
	$(COMPILE.c) $<.c -o $@

$(ODIR)/vdc.o: $(SDIR)/vdc.c $(HDR)
	$(AHIST) $< > $<.c
	$(PHIST) $<.c
	$(COMPILE.c) $<.c -o $@

$(ODIR)/mwdc.o: $(SDIR)/mwdc.c $(HDR)
	$(COMPILE.c) $< -o $@

$(ODIR)/mwpc.o: $(SDIR)/mwpc.c $(HDR)
	$(AHIST) $< > $<.c
	$(PHIST) $<.c
	$(COMPILE.c) $<.c -o $@

$(ODIR)/chamb.o: $(SDIR)/chamb.c $(HDR)
	$(AHIST) $< > $<.c
	$(PHIST) $<.c
	$(COMPILE.c) $<.c -o $@

$(ODIR)/ray.o: $(SDIR)/ray.c $(HDR)
	$(COMPILE.c) $< -o $@

$(ODIR)/mwdc_fit.o: $(SDIR)/mwdc_fit.c $(HDR)
	$(COMPILE.c) $< -o $@

$(ODIR)/matrix.o: $(SDIR)/matrix.c $(HDR)
	$(COMPILE.c) $< -o $@

$(EDIR)/ask: $(SDIR)/ask.c $(HDIR)/histogram.h $(HDIR)/freadin.h
	$(LINK.c) $< -o $@

lex.yy.c: $(SDIR)/freadin.lex $(SDIR)/freadin.tab.c
	$(LEX) $<

$(ODIR)/lex.yy.o: lex.yy.c $(SDIR)/freadin.tab.c
	$(COMPILE.c) $< -o $@

$(SDIR)/freadin.tab.c: $(SDIR)/freadin.y $(HDIR)/freadin.h
	$(YACC) $<
	mv freadin.tab.c $(SDIR)
	mv freadin.tab.h $(SDIR)

$(ODIR)/freadin.tab.o: $(SDIR)/freadin.tab.c
	$(COMPILE.c) $< -o $@

$(EDIR)/analyzer: $(OBJ)
	$(LINK.c)  $(OBJ) $(STATIC) $(VARIOUSLIB) $(FLIB) -o $@ $(ROOTFLAGS)

test: test.c histogram.o histogram.h
	$(AHIST) $< > $<.c
	$(LINK.c) $<.c histogram.o -o $@

clean:
	rm -f core $(TARGET) $(ODIR)/* $(SDIR)/*.c.c \
	$(SDIR)/freadin.tab.h $(SDIR)/freadin.tab.c \
	lex.yy.c $(SDIR)/freadin.output \
        last.kumac last.kumacold 
