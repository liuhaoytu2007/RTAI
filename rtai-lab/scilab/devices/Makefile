LIB = libsciblk.a
all: $(LIB)

RTAIDIR = $(shell rtai-config --prefix)
COMEDIDIR = $(shell rtai-config --comedi-dir)

SRCALL = $(wildcard *.c)

ifeq ($(strip $(COMEDIDIR)),)
EXCLUDE = exit_on_error.c rtai_comedi_datain.c rtai_comedi_dataout.c rtai_comedi_dioin.c rtai_comedi_dioout.c
else
EXCLUDE = exit_on_error.c
endif

SRC=$(filter-out $(EXCLUDE),$(SRCALL))

OBJ = $(SRC:.c=.o)
DBG = -g

CC_FLAGS = -c $(DBG) -I. -I$(RTAIDIR)/include -O2

%.o: %.c
	cc $(CC_FLAGS) $<

$(LIB): $(OBJ)
	ar -r $(LIB) $(OBJ)

install: $(LIB)
	cp $(LIB) $(RTAIDIR)/lib

clean:
	rm -f $(LIB) $(OBJ)