LIB= /usr/local/lib
INCLUDE= $(HOME)/lua/src

LINKS= -L$(LIB) -llua5.1 -Wl,-rpath,$(LIB)
INCLUDES= -I$(INCLUDE)
RM= rm -rf

BIN= inspect.out

CCOPT= -g -O0 -Wall -Werror -Wno-unused -Wpointer-arith

all: $(BIN)

$(BIN): inspect.c
	cc $(CCOPT) $(INCLUDES) $(LINKS) -o $@ $<

clean:
	$(RM) *.out *.dSYM


.PHONY: all clean
