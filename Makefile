# My Makefile

SRC= dpop.c
OBJ=$(SRC:.c=.o)

CC= cc

CFLAGS= -c -O2 -pipe -static
LCFLAGS=

LIBS=

.c.o:
	$(CC) $(CFLAGS) $<

dpopper: $(OBJ)
	$(CC) $(LCFLAGS) -o $@ $(OBJ) $(LIBS)

clean:
	rm -f dpopper *.[oa]

