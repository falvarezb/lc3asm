# Some references used to create this make file:
# https://docs.oracle.com/cd/E19504-01/802-5880/6i9k05dhg/index.html


# object files and executables
BUILD_DIR = out
TOOLS_BUILD_DIR = tools/out
# log files
LOG_DIR = logs
# output directories are created automatically by a rule
OUTPUT_DIRS = ${BUILD_DIR} ${LOG_DIR} tools/${BUILD_DIR}

CC = gcc
CFLAGS = -Og -Wall -Wno-missing-braces -Wextra -Wshadow -Wpedantic -std=c11 -fno-common --coverage -fprofile-exclude-files=.*test\.c
LDFLAGS =
SRCS_PROD := common.c lc3common.c parser_add.c parser_and.c parser_not.c parser_ret.c parser_jmp.c parser_jsr.c
OBJS_PROD := $(addprefix $(BUILD_DIR)/, $(patsubst %.c,%.o,$(SRCS_PROD)))
SRCS_TEST := test.c parser_add_test.c parser_and_test.c parser_not_test.c parser_ret_test.c parser_jmp_test.c
OBJS_TEST := $(addprefix $(BUILD_DIR)/, $(patsubst %.c,%.o,$(SRCS_TEST)))
SRCS_TOOLS := lc3objdump.c
OBJS_TOOLS := $(addprefix $(TOOLS_BUILD_DIR)/, $(patsubst %.c,%.o,$(SRCS_TOOLS)))
LDLIBS = 


#if __linux__
	VALGRIND = valgrind --tool=memcheck --leak-check=full 
	VALGRIND += --verbose --log-file=${LOG_DIR}/valgrind.log
#else
	VALGRIND = 
#endif


.PHONY: all clean compile compiletest unittest addtest myfile runobjdump

unittest: test jsrtest

all: clean compile unittest


$(OBJS_PROD): | ${OUTPUT_DIRS}
$(OBJS_TOOLS): | ${OUTPUT_DIRS}


compile: $(OBJS_PROD)
compiletest: $(OBJS_PROD) $(OBJS_TEST)

####################### 
#### tests  ###########
#######################

#### run unit tests using cmocka library  ######
test: $(BUILD_DIR)/test
	$(VALGRIND) ./$^	

$(BUILD_DIR)/test: $(OBJS_PROD) $(OBJS_TEST)
	$(LINK.c) $^ -o $@ $(LDLIBS) -lcmocka

jsrtest: $(BUILD_DIR)/jsrtest
	$(VALGRIND) ./$^

$(BUILD_DIR)/jsrtest: $(OBJS_PROD) $(BUILD_DIR)/parser_jsr_test.o
	$(LINK.c) $^ -o $@ $(LDLIBS) -lcmocka

coverage_report: unittest
	gcov $(BUILD_DIR)/*.gcda > /dev/null
	mv *.c.gcov $(BUILD_DIR)/
	lcov --directory $(BUILD_DIR) --capture --output-file $(BUILD_DIR)/app.info > /dev/null
	genhtml -o $(BUILD_DIR) $(BUILD_DIR)/app.info > /dev/null
	open $(BUILD_DIR)/index.html



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

# run lc3objdump.c
# e.g. "make runobjdump CPPFLAGS=-DFAB_MAIN filename=./lc3examples/test.obj output_mode=hex"
runobjdump: $(TOOLS_BUILD_DIR)/lc3objdump
	$(VALGRIND) ./$^ $(filename) $(output_mode)

$(TOOLS_BUILD_DIR)/lc3objdump: $(OBJS_TOOLS)
	$(LINK.c) $^ -o $@ $(LDLIBS)


############################## 
#### C files compilation #####
##############################

# if an object ﬁle is needed, compile the corresponding .c ﬁle
${BUILD_DIR}/%.o: src/%.c
	$(COMPILE.c) $< -o $@

${BUILD_DIR}/%.o: %.c
	$(COMPILE.c) $< -o $@

# if an object ﬁle is needed, compile the corresponding .c ﬁle
${TOOLS_BUILD_DIR}/%.o: tools/%.c
	$(COMPILE.c) $< -o $@


${OUTPUT_DIRS}:
	mkdir $@

clean:
	${RM} -r ${LOG_DIR}/* ${BUILD_DIR}/* ${TOOLS_BUILD_DIR}/* *.o 