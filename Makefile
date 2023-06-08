include pretty.mk

# Library name
LIB_NAME = uniconf
# Install path
INSTALL_PATH = /usr/local/custom/
#test path
TEST_PATH = tests/

VERSION := $(shell ./version.sh)

CC = gcc # compiler
CFLAGS = -fPIC -Wall -Wextra -O2 -g -std=gnu99 -DVERSION=$(VERSION) -I/usr/include -I/usr/local/custom/include # compiling flags
LDFLAGS = -shared # linking flags

STATIC_LIB = lib$(LIB_NAME).a
TARGET_LIB = lib$(LIB_NAME).so

SRC_DIR = sources/
FILES = $(shell find $(SRC_DIR) -name '*.c') #$(wildcard $(SRC_DIR)*.c) #
SRC_FILES = $(patsubst $(SRC_DIR)%,%,$(FILES))

OBJ_DIR = obj/
OBJECT_LINKS = $(addprefix $(OBJ_DIR), $(SRC_FILES:.c=.o))

.PHONY: make
make: $(TARGET_LIB)

$(TARGET_LIB): $(OBJECT_LINKS)
	ar rcs $(STATIC_LIB) $(OBJECT_LINKS)
	@$(PRINTF)	"$(WARN_COLOR)\n  Archiving...  $(STATIC_LIB) $(OK_COLOR)         [✓]\n  static library created$(NO_COLOR)\n"
	@$(CC) ${LDFLAGS} -o $@ $^
	@$(PRINTF)	"$(WARN_COLOR)\n  Linking...  $(TARGET_LIB) $(OK_COLOR)         [✓]\n  dynamic library created$(NO_COLOR)\n"
	@rm -rf $(OBJ_DIR)

$(OBJ_DIR)%.o : $(SRC_DIR)%.c
	@/bin/mkdir -p $(@D)
	@$(CC) $(CFLAGS) -o $@ -c $<
	@$(PRINTF) "$(WARN_COLOR) Compiling... $(OK_COLOR) $< ✓ $(NO_COLOR)\n"

ifeq ($(UNIT_TEST_PATH),)
UNIT_TEST_PATH=tests/unit/
endif

TEST_BIN = $(UNIT_TEST_PATH)unitTests

TEST_FILES = $(shell find $(UNIT_TEST_PATH) -name '*.c')
TEST_SRC_FILES = $(patsubst $(UNIT_TEST_PATH)%,%,$(TEST_FILES))

TEST_OBJ_DIR = $(TEST_PATH)obj/
TEST_OBJECT_LINKS = $(addprefix $(TEST_OBJ_DIR), $(TEST_SRC_FILES:.c=.o))

.PHONY: test
test: $(TEST_BIN)

$(TEST_BIN): $(TEST_OBJECT_LINKS)
	@$(CC) -L/usr/lib64  -lcunit -L/usr/local/lib -L$(INSTALL_PATH)lib -lconfig -lcjson -llists -l$(LIB_NAME) -o $@ $^
	@$(PRINTF)	"$(WARN_COLOR)\n  Linking...  $(TEST_BIN) $(OK_COLOR)         [✓]\n  tests created$(NO_COLOR)\n"
	@rm -rf $(TEST_OBJ_DIR)

$(TEST_OBJ_DIR)%.o : $(UNIT_TEST_PATH)%.c
	@/bin/mkdir -p $(@D)
	@$(CC) $(CFLAGS) -I/usr/include -I/usr/local/include -I$(INSTALL_PATH)include -o $@ -c $<
	@$(PRINTF) "$(WARN_COLOR) Compiling... $(OK_COLOR) $< ✓ $(NO_COLOR)\n"

.PHONY: clean
clean: 
	@rm -rf $(OBJ_DIR)
	@rm -f $(TARGET_LIB)

.PHONY: re
re: clean make

.PHONY: install
install: re
	@mkdir -p $(INSTALL_PATH)include
	@mkdir -p $(INSTALL_PATH)lib
	@cp $(SRC_DIR)$(LIB_NAME).h $(INSTALL_PATH)include/
	@cp $(STATIC_LIB) $(INSTALL_PATH)lib/
	@cp $(TARGET_LIB) $(INSTALL_PATH)lib/
	@$(PRINTF) "$(OK_COLOR) Installed to $(OK_COLOR) $(INSTALL_PATH)$(NO_COLOR)\n"

default: make

