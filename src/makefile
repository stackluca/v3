CC=gcc
INCLUDE=-I core -I base_type -I builtin_type
CFLAGS=-Wall -ggdb3 -O0 -DWARF2 -DW_AT_call_file -DW_AT_call_line  ${INCLUDE}

headers=\
v3_pool.h \
v3_string.h \
v3_tokenizer.h \
v3_error_object.h \
v3_eval.h \
base_type/v3_base_type.h \
base_type/v3_function_object.h \
base_type/v3_number_object.h \
base_type/v3_object.h \
base_type/v3_string_object.h \
core/v3_core.h \
core/v3_dict.h \
core/v3_pool.h \
core/v3_vector.h \
core/v3_foundation.h \


objects=\
v3_tokenizer.o \
v3_parser.o \
v3js.o \
v3_error_object.o \
v3_eval.o \
base_type/v3_function_object.o \
base_type/v3_number_object.o \
base_type/v3_object.o \
base_type/v3_string_object.o \
builtin_type/v3_Error.o \
builtin_type/v3_Date.o \
core/v3_dict.o \
core/v3_pool.o \
core/v3_vector.o \
core/v3_foundation.o \


default:$(objects)
	$(CC) $(CFLAGS) -o v3js  $(objects)

$(objects):%.o:%.c
	$(CC) -c $(CFLAGS)  $< -o $@

clean:
	rm -rf *.o

