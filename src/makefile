CC=gcc
INCLUDE=-I core
CFLAGS=-Wall -std=c99 -ggdb3 -O0 -DWARF2 -DW_AT_call_file -DW_AT_call_line  ${INCLUDE}

headers=\
v3_core.h \
v3_pool.h \
v3_string.h \
v3_foundation.h \
v3_tokenizer.h \

objects=\
v3_foundation.o \
v3_tokenizer.o \
v3_parser.o \
v3_pool.o \
v3js.o \


default:$(objects)
	$(CC) $(CFLAGS) -o v3js  $(objects)

$(objects):%.o:%.c
	$(CC) -c $(CFLAGS)  $< -o $@

clean:
	rm -rf *.o

