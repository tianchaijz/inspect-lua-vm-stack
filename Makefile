LIB= /usr/local/lib
INCLUDE= $(HOME)/lua/src

LINKS= -L$(LIB) -llua5.1 -Wl,-rpath,$(LIB)
INCLUDES= -I$(INCLUDE)
RM= rm -rf

BIN= inspect.out

CCOPT= -g -O0 -Wall -Werror -Wno-unused -Wpointer-arith

SOCC= $(CC) -shared
SOLDFLAGS= -fPIC $(LDFLAGS)
SOCFLAGS= -fPIC $(CCOPT) $(CCWARN) $(DEFINES) $(INCLUDES) $(CFLAGS)

LINSPECT= linspect.so


all: $(BIN) $(LINSPECT)

$(BIN): inspect.c inspect.h
	cc $(CCOPT) $(INCLUDES) $(LINKS) -o $@ $<

linspect.o: linspect.c inspect.h
	$(CC) $(SOCFLAGS) -c -o $@ $<

$(LINSPECT): linspect.o
	$(SOCC) $(SOLDFLAGS) -o $@ $<

clean:
	$(RM) *.o *.so *.dSYM *.out


.PHONY: all clean
