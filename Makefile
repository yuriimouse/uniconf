# Paths
SRC_DIR     := sources
TEST_DIR    := tests/unit
BUILD_DIR   := build
DATA_DIR    := $(TEST_DIR)/data

# Compiler and flags
CC          := gcc
CFLAGS      := -Wall -Wextra -O2 -I$(SRC_DIR) -I$(TEST_DIR)
LDFLAGS     := 
LDLIBS      := -lcjson -lyaml -lcunit -lconfig

# Output
LIB_NAME    := libuniconf.a
LIB_PATH    := $(BUILD_DIR)/$(LIB_NAME)

# Sources
SRC_FILES   := $(wildcard $(SRC_DIR)/*.c)
OBJ_FILES   := $(patsubst $(SRC_DIR)/%.c,$(BUILD_DIR)/%.o,$(SRC_FILES))

# Tests
TEST_NAME   := uniconf_tests
TEST_SRCS   := $(wildcard $(TEST_DIR)/test_*.c) $(TEST_DIR)/main.c
TEST_OBJS   := $(patsubst $(TEST_DIR)/%.c,$(BUILD_DIR)/%.test.o,$(TEST_SRCS))
TEST_BIN    := $(BUILD_DIR)/$(TEST_NAME)

# valgrind
VALGRIND    := valgrind
VG_OPTS     := --leak-check=full --error-exitcode=1 --track-origins=yes --quiet

# Install
PREFIX      ?= /usr/local
INCLUDE_DIR := $(PREFIX)/include/uniconf
LIB_DIR     := $(PREFIX)/lib

.PHONY: all clean install tests run

all: $(LIB_PATH)

$(BUILD_DIR):
	mkdir -p $@

$(BUILD_DIR)/%.o: $(SRC_DIR)/%.c | $(BUILD_DIR)
	$(CC) $(CFLAGS) -c $< -o $@

$(LIB_PATH): $(OBJ_FILES)
	ar rcs $@ $^

clean:
	rm -rf $(BUILD_DIR)

install: $(LIB_PATH)
	install -d $(INCLUDE_DIR)
	install -d $(LIB_DIR)
	install -m 644 $(SRC_DIR)/uniconf.h $(INCLUDE_DIR)
	install -m 644 $(LIB_PATH) $(LIB_DIR)

tests: $(TEST_BIN)

$(BUILD_DIR)/%.test.o: $(TEST_DIR)/%.c | $(BUILD_DIR)
	$(CC) $(CFLAGS) -c $< -o $@

$(TEST_BIN): $(OBJ_FILES) $(TEST_OBJS)
	$(CC) -o $@ $^ $(LDLIBS)

run: tests
	$(VALGRIND) $(VG_OPTS) $(TEST_BIN) $(DATA_DIR)
