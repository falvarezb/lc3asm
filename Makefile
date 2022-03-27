# Some references:
# https://docs.oracle.com/cd/E19504-01/802-5880/6i9k05dhg/index.html

# object files and executables
BUILD_DIR = out
TOOLS_BUILD_DIR = tools/out
# log files
LOG_DIR = logs
# output directories are created automatically by a rule
OUTPUT_DIRS = ${BUILD_DIR} ${LOG_DIR} tools/${BUILD_DIR}

CC = gcc
CFLAGS = -g -Wall -Wno-missing-braces -Wextra -Wshadow -Wpedantic -std=c11 #-fprofile-arcs -ftest-coverage
LDFLAGS = 
SRCS_PROD := myfile.c
OBJS_PROD := $(addprefix $(BUILD_DIR)/, $(patsubst %.c,%.o,$(SRCS_PROD)))
SRCS_TOOLS := lc3objdump.c
OBJS_TOOLS := $(addprefix $(TOOLS_BUILD_DIR)/, $(patsubst %.c,%.o,$(SRCS_TOOLS)))
LDLIBS = 


#if __linux__
	VALGRIND = valgrind --tool=memcheck --leak-check=full 
	VALGRIND += --verbose --log-file=${LOG_DIR}/valgrind.log
#else
	VALGRIND = 
#endif


.PHONY: all clean unittest myfile lc3objdump

all: clean unittest


$(OBJS_PROD): | ${OUTPUT_DIRS}
$(OBJS_TOOLS): | ${OUTPUT_DIRS}

####################### 
#### tests  ###########
#######################

#### run unit tests using cmocka library  ######
# run unit tests defined in unittest.c
unittest: $(BUILD_DIR)/myfile_test
	$(VALGRIND) ./$^

$(BUILD_DIR)/myfile_test: $(OBJS_PROD) ${BUILD_DIR}/myfile_test.o
	$(LINK.c) $^ -o $@ $(LDLIBS) -lcmocka


####################### 
#### modules  #########
#######################

#### run individual module  ######
# run main method of myfile.c
# invoke with "make myfile CPPFLAGS=-DFAB_MAIN"
myfile: $(BUILD_DIR)/myfile
	$(VALGRIND) ./$^

$(BUILD_DIR)/myfile: $(OBJS_PROD)
	$(LINK.c) $^ -o $@ $(LDLIBS)


####################### 
#### tools   ##########
#######################

# run main method of lc3objdump.c
# e.g. "make exec_lc3objdump CPPFLAGS=-DFAB_MAIN filename=../lc3practice/test.obj output_mode=hex"
exec_lc3objdump: $(TOOLS_BUILD_DIR)/lc3objdump
	$(VALGRIND) ./$^ $(filename) $(output_mode)

$(TOOLS_BUILD_DIR)/lc3objdump: $(OBJS_TOOLS)
	$(LINK.c) $^ -o $@ $(LDLIBS)


############################## 
#### C files compilation #####
##############################

# if an object ﬁle is needed, compile the corresponding .c ﬁle
${BUILD_DIR}/%.o: %.c
	$(COMPILE.c) $< -o $@

# if an object ﬁle is needed, compile the corresponding .c ﬁle
${TOOLS_BUILD_DIR}/%.o: tools/%.c
	$(COMPILE.c) $< -o $@


${OUTPUT_DIRS}:
	mkdir $@

clean:
	${RM} -r ${LOG_DIR}/* ${BUILD_DIR}/* *.o ${TOOLS_BUILD_DIR}/* *.o 