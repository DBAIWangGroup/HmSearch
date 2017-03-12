##############
# Make the project
##############

PROD	:= PROD
OPT     := -O3
VERSION := \"1.0_${PROD}\"
TARGETS := q_sig_LSH_NGPPD32B_galpha
DEFINES := #-DOPEN_HASH
SRCS    := hash_dict.c  idf.c  index.c  main.c  query.c  query_prefix.c  record.c  search.c sig_index.c query_sig.c usage.c bit_fv.c multi_ordering_filtering.c
OBJS    := ${SRCS:.c=.o}

CCFLAGS = ${OPT} -Wall -Wno-deprecated -ggdb -D${PROD} ${DEFINES} -I./ # -DVERSION=${VERSION} # -m64 -mcmodel=large
LDFLAGS = ${OPT} -ggdb  #-mcmodel=large
LIBS    = -lcrypto
CC	= g++

.PHONY: all clean distclean 
all:: ${TARGETS} 

q_sig_LSH_NGPPD32B_galpha: main.o hash_dict.o idf.o index.o query.o query_prefix.o record.o search.o query_sig.o sig_index.o usage.o multi_ordering_filtering.o bit_fv.o
	${CC} ${LDFLAGS} -o $@ $^ ${LIBS}

${OBJS}: %.o: %.c
	${CC} ${CCFLAGS} -o $@ -c $< 

install:
	mkdir -p ../../bin
	cp ${TARGETS} ../../bin/

clean:: 
	-rm -f *~ *.o ${TARGETS}

distclean:: clean
