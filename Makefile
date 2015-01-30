CC = gcc
CFLAGS = -Wall -O2
INCLUDES = # -I. -I/usr/include
LIBS = -lgmp
BIN = calc

CFILES = stack.c calc.c
OBJS = ${CFILES:.c=.o}

all: ${CFILES} ${BIN}

${BIN}: ${OBJS}
	${CC} ${CFLAGS} ${LIBS} ${OBJS} -o $@

.c.o:
	${CC} ${CFLAGS} ${INCLUDES} -c $< -o $@

clean:
	rm -f ${OBJS} ${BIN}
