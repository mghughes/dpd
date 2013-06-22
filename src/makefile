#
# makefile for compiling dpd simulation program
#
# Copyright (C) Matthieu Hughes, 2013
# matthieuhughes.1@gmail.com
#
# last update: 22/06/13, Matthieu Hughes
#       Only works for all option
#       change to reflect new directory structure
#
    
SHELL=/bin/bash
#
SIM_SRCS = calc.c sim_helper.c sim_update.c
HIST_SRCS = hist_init.c hist_print.c hist_update.c
INIT_SRCS = cell.c io.c mem.c param_init.c part_init.c


SRCS = main.c calc.c cell.c hist_init.c hist_print.c hist_update.c io.c mem.c param_init.c part_init.c sim_helper.c sim_update.c

OBJS = main.o calc.o cell.o hist_init.o hist_print.o hist_update.o io.o mem.o param_init.o part_init.o sim_helper.o sim_update.o

HFILES = calc.h cell.h cell_struct.h file_struct.h hist_init.h hist_print.h hist_struct.h hist_update.h io.h mem.h param_init.h param_struct.h part_init.h part_struct.h sim_helper.h sim_update.h

CF = gcc
FLAGS = -Wall
LIBS = -lm
OUT = dpd.run

# to use link-time optimization on gcc<4.6
all: all.c
	$(CF) $(FLAGS) -O3 -o $(OUT) all.c $(LIBS)

opt3: $(SRCS)
	$(CF) $(FLAGS) -O3 -o $(OUT) $(SRCS) $(LIBS)

prof: $(SRCS)
	$(CF) $(FLAGS) -pg -o $(OUT) $(SRCS) $(LIBS)

$(SRCS):	$(HFILES)
$(OBJS):	$(HFILES)
