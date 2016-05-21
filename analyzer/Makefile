# Make file for analyzer program
# Performs code generation only
# Version 1.00  02-JUN-2000  by A. Tamii
# Version 1.50  20-MAY-2016  by C. Sullivan

#.SILENT:

#******************
#* Compiler etc.  *
#******************
# Directories
ANA = $(realpath ./)
SDIR = src
HDIR = include
DICTLIB = libRCNPEvent
ODIR = obj
EDIR = bin
COG = $(EDIR)/cog
ROOTCINT = rootcint
LEX = flex
YACC = bison -v -d
AHIST = awk -f bin/hist.awk
PHIST = perl bin/moveline.pl


SRC  =	$(SDIR)/analyzer.c.c \
	$(SDIR)/chamb.c.c \
	$(SDIR)/vdc.c.c \
	$(SDIR)/mwpc.c.c \
	$(SDIR)/chamb_rcnp.c.c \
	$(SDIR)/chamb_np.c.c \
	$(SDIR)/chamb_gr_fpp.c.c \
	$(SDIR)/chamb_las_fpp.c.c \
	$(SDIR)/chamb_f2.c.c \
	$(SDIR)/fread_rcnp.c.c \
	$(SDIR)/fread_mars.c.c \
	$(SDIR)/fread_dst.c.c \
	$(SDIR)/fread_root.cc.cc \
	$(SDIR)/freadin.tab.c \
	$(SDIR)/lex.yy.c \
	$(DICTLIB)/$(SDIR)/RCNPEventDictionary.cc \
	$(DICTLIB)/$(HDIR)/RCNPEventDictionary.h \
	$(DICTLIB)/$(HDIR)/RCNPEvent.h \
	$(ODIR)/hb.o

HDR  = $(HDIR)/analyzer.h $(HDIR)/histogram.h $(HDIR)/freadin.h
HISTDEF=$(realpath ../hist.def)

all:  $(SRC)

$(DICTLIB)/$(HDIR)/RCNPEvent.h: $(HISTDEF)
	$(COG) $(HDIR)/rootalyze.h > $@

$(SDIR)/%.c.c: $(SDIR)/%.c
	$(AHIST) $< > $<.c
	$(PHIST) $<.c

$(SDIR)/%.cc.cc: $(SDIR)/%.cc
	$(AHIST) $< > $<.cc
	$(PHIST) $<.cc

$(DICTLIB)/$(SDIR)/RCNPEventDictionary.h: $(SDIR)/RCNPEventDictionary.cc
	touch $@

$(DICTLIB)/$(SDIR)/RCNPEventDictionary.cc: $(DICTLIB)/$(HDIR)/RCNPEvent.h $(DICTLIB)/$(HDIR)/LinkDef.h
	$(ROOTCINT) -f $@ -c -p -I$(realpath $(DICTLIB)/$(HDIR)) $(realpath $(DICTLIB)/$(HDIR))/RCNPEvent.h $(realpath $(DICTLIB)/$(HDIR))/LinkDef.h
	mv $(basename $@).h $(DICTLIB)/$(HDIR)

$(SDIR)/lex.yy.c: $(SDIR)/freadin.lex $(SDIR)/freadin.tab.c
	$(LEX) $<
	mv lex.yy.c $@

$(SDIR)/freadin.tab.c: $(SDIR)/freadin.y $(HDIR)/freadin.h
	$(YACC) $<
	mv freadin.tab.c $(SDIR)
	mv freadin.tab.h $(HDIR)

$(ODIR)/hb.o: $(SDIR)/hb.f
	gfortran -c -o $@ $<

clean:
	@rm -f $(SDIR)/*.c.c $(SDIR)/*.cc.cc \
	$(HDIR)/freadin.tab.h $(SDIR)/freadin.tab.c \
	$(SDIR)/lex.yy.c $(SDIR)/freadin.output \
	$(DICTLIB)/$(SDIR)/*Dictionary.cc $(DICTLIB)/$(HDIR)/*Dictionary.h \
	$(DICTLIB)/$(HDIR)/RCNPEvent.h $(HDIR)/*.pyc \
	$(SDIR)/#* $(SDIR)/*~ $(SDIR)/.#* \
	$(HDIR)/#* $(HDIR)/*~ $(HDIR)/.#*
