# Makefile Create by Siyuan Xu in 2017

INCL   = \
	-L/usr/include \
	-L/usr/include/gsl \

HDER   = \
	global.h \

SRC    = \
        main.c \
		cwb.c \
		distri.c \
		func.c \
		v2c_v2v.c \
		dfg.c

	
OBJ    = $(SRC:.c=.o)
EXE    = probe.exe

# Compiler, Linker Defines
CC      = gcc
LINKER  = $(CC) -lgsl -lgslcblas

#-DGET_ERROR	    : Place & Route, Simulation..
#-DREAL_ERROR	    : Real error, (out0-golen)/golden..
#-DSIM_Prune	    : TVLSI Version, for comparsion only..
#-DAPX2ORG	    : Apx to orginal design, disable select signal 
#-DPRINT_REG_SIGNAL : Print the VCD's signals' value into signal.dat file
#-DMINFUS           : Use minium number of function units

OPTIONS = -Wall 
CFLAGS  = -c $(OPTIONS)
CFDEBUG = -g -DEBUG $(OPTIONS)
RM      = rm -f

default: all
all: clean $(EXE) 

geterror: OPTIONS+= -DGET_ERROR
geterror: clean $(EXE) 

realerror: OPTIONS+= -DGET_ERROR -DREAL_ERROR
realerror: clean $(EXE) 

backorg: OPTIONS+= -DGET_ERROR -DAPX2ORG
backorg: clean $(EXE)  

backtvlsi: OPTIONS+= -DGET_ERROR -DSIM_Prune
backtvlsi: clean $(EXE) 


# Compile and Assemble C Source Files into Object Files
.c.o: $(HDER)
	$(CC) $(CFLAGS) $< -o $@

# Link all Object Files with external Libraries into Binaries
$(EXE): $(OBJ)
	$(LINKER) -o $@ $(OBJ) $(INCL) -lm

# Clean Up Objects, Exectuables, Dumps out of source directory
clean: 
	$(RM) $(OBJ) $(EXE)
