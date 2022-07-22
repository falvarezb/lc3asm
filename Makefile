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
CFLAGS = -Og -Wall -Wno-missing-braces -Wextra -Wshadow -Wpedantic -std=c11 -fno-common --coverage
LDFLAGS = 
SOURCE_DIR := src
OBJS_PROD := $(addprefix $(BUILD_DIR)/, $(patsubst %.c,%.o,$(shell ls $(SOURCE_DIR))))
SRCS_TEST := test.c parser_add_test.c parser_and_test.c parser_not_test.c parser_ret_test.c parser_jmp_test.c memory_access_instr_test.c
OBJS_TEST := $(addprefix $(BUILD_DIR)/, $(patsubst %.c,%.o,$(SRCS_TEST)))
SRCS_TOOLS := lc3objdump.c
OBJS_TOOLS := $(addprefix $(TOOLS_BUILD_DIR)/, $(patsubst %.c,%.o,$(SRCS_TOOLS)))
LDLIBS = -lglib-2.0

ifeq ($(shell uname), Linux)
	VALGRIND = valgrind --tool=memcheck --leak-check=full --show-leak-kinds=all
	VALGRIND += --verbose --log-file=${LOG_DIR}/valgrind.log
	# _POSIX_C_SOURCE=200809 to expose strdup and getline in Linux
	CFLAGS += -D_POSIX_C_SOURCE=200809 -I/usr/include/glib-2.0 -I/usr/lib/x86_64-linux-gnu/glib-2.0/include
else ifeq ($(shell uname), Darwin)
	VALGRIND =
	CFLAGS += -I/usr/local/include/glib-2.0 -I/usr/local/lib/glib-2.0/include -fsanitize=address -fsanitize=undefined
endif


.PHONY: all clean compile compiletest unittest runobjdump

unittest: addtest andtest jmptest nottest rettest jsrtest ldstitest filetest

all: clean compile unittest


$(OBJS_PROD): | ${OUTPUT_DIRS}
$(OBJS_TOOLS): | ${OUTPUT_DIRS}


compile: $(OBJS_PROD)
compiletest: $(OBJS_PROD) $(OBJS_TEST)

####################### 
#### tests  ###########
#######################

#### run unit tests using cmocka library  ######
addtest: $(BUILD_DIR)/addtest
	$(VALGRIND) ./$^	

$(BUILD_DIR)/addtest: $(OBJS_PROD) $(BUILD_DIR)/parser_add_test.o
	$(LINK.c) $^ -o $@ $(LDLIBS) -lcmocka

#######################

andtest: $(BUILD_DIR)/andtest
	$(VALGRIND) ./$^	

$(BUILD_DIR)/andtest: $(OBJS_PROD) $(BUILD_DIR)/parser_and_test.o
	$(LINK.c) $^ -o $@ $(LDLIBS) -lcmocka

#######################

jmptest: $(BUILD_DIR)/jmptest
	$(VALGRIND) ./$^	

$(BUILD_DIR)/jmptest: $(OBJS_PROD) $(BUILD_DIR)/parser_jmp_test.o
	$(LINK.c) $^ -o $@ $(LDLIBS) -lcmocka

#######################

nottest: $(BUILD_DIR)/nottest
	$(VALGRIND) ./$^	

$(BUILD_DIR)/nottest: $(OBJS_PROD) $(BUILD_DIR)/parser_not_test.o
	$(LINK.c) $^ -o $@ $(LDLIBS) -lcmocka

#######################

rettest: $(BUILD_DIR)/rettest
	$(VALGRIND) ./$^	

$(BUILD_DIR)/rettest: $(OBJS_PROD) $(BUILD_DIR)/parser_ret_test.o
	$(LINK.c) $^ -o $@ $(LDLIBS) -lcmocka

#######################

jsrtest: $(BUILD_DIR)/jsrtest
	$(VALGRIND) ./$^

$(BUILD_DIR)/jsrtest: $(OBJS_PROD) $(BUILD_DIR)/parser_jsr_test.o
	$(LINK.c) $^ -o $@ $(LDLIBS) -lcmocka

#######################

ldstitest: $(BUILD_DIR)/ldstitest
	$(VALGRIND) ./$^

$(BUILD_DIR)/ldstitest: $(OBJS_PROD) $(BUILD_DIR)/parser_ld_sti_test.o
	$(LINK.c) $^ -o $@ $(LDLIBS) -lcmocka

#######################

filetest: $(BUILD_DIR)/filetest
	$(VALGRIND) ./$^	

$(BUILD_DIR)/filetest: $(OBJS_PROD) $(BUILD_DIR)/parser_file_test.o
	$(LINK.c) $^ -o $@ $(LDLIBS) -lcmocka

#######################

dicttest: $(BUILD_DIR)/dicttest
	$(VALGRIND) ./$^	

$(BUILD_DIR)/dicttest: $(OBJS_PROD) $(BUILD_DIR)/dict_test.o
	$(LINK.c) $^ -o $@ $(LDLIBS) -lcmocka

#######################

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
${BUILD_DIR}/%.o: $(SOURCE_DIR)/%.c
	$(COMPILE.c) $< -o $@

${BUILD_DIR}/%.o: test/%.c
	$(COMPILE.c) $< -o $@

# if an object ﬁle is needed, compile the corresponding .c ﬁle
${TOOLS_BUILD_DIR}/%.o: tools/%.c
	$(COMPILE.c) $< -o $@


${OUTPUT_DIRS}:
	mkdir $@

clean:
	${RM} -r ${LOG_DIR}/* ${BUILD_DIR}/* ${TOOLS_BUILD_DIR}/* *.o 