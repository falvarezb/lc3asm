# Some references:
# https://docs.oracle.com/cd/E19504-01/802-5880/6i9k05dhg/index.html

# object files and executables
BUILD_DIR = out
# log files
LOG_DIR = logs
# output directories are created automatically by a rule
OUTPUT_DIRS = ${BUILD_DIR} ${LOG_DIR}

CC = gcc
CFLAGS = -g -Wall -Wno-missing-braces -Wextra -Wshadow -Wpedantic -std=c11
LDFLAGS = 
SRCS_PROD := myfile.c
OBJS_PROD := $(addprefix $(BUILD_DIR)/, $(patsubst %.c,%.o,$(SRCS_PROD)))
LDLIBS = 


#if __linux__
	VALGRIND = valgrind --tool=memcheck --leak-check=full 
	VALGRIND += --verbose --log-file=${LOG_DIR}/valgrind.log
#else
	VALGRIND = 
#endif


.PHONY: all clean unittest myfile

all: clean unittest


$(OBJS_PROD): | ${OUTPUT_DIRS}

####################### 
#### test goals  ######
#######################


#### run unit tests using cmocka library  ######
# run unit tests defined in unittest.c
unittest: $(BUILD_DIR)/myfile_test
	$(VALGRIND) ./$^

$(BUILD_DIR)/myfile_test: $(OBJS_PROD) ${BUILD_DIR}/myfile_test.o
	$(LINK.c) $^ -o $@ $(LDLIBS) -lcmocka

#### run individual module  ######
# run main method of myfile.c
# invoke with "make myfile CPPFLAGS=-DFAB_MAIN"
myfile: $(BUILD_DIR)/myfile
	$(VALGRIND) ./$^

$(BUILD_DIR)/myfile: $(OBJS_PROD)
	$(LINK.c) $^ -o $@ $(LDLIBS)


# if an object ﬁle is needed, compile the corresponding .c ﬁle
${BUILD_DIR}/%.o: %.c
	$(COMPILE.c) $< -o $@


${OUTPUT_DIRS}:
	mkdir $@

clean:
	${RM} -r ${LOG_DIR}/* ${BUILD_DIR}/* *.o