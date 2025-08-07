# Compiler and flags
CC := gcc

# Directories
CONFIG_DIR := .
MAIN_DIR := ../..
TEST_DIR := ..

CONFIG_SRC := $(CONFIG_DIR)/src
GENERIC_TESTS_SRC := $(TEST_DIR)/src
MAIN_SRC := $(MAIN_DIR)/src
INC_DIR := $(CONFIG_DIR)/inc
MAIN_INC := $(MAIN_DIR)/inc
TEST_INC := $(TEST_DIR)/inc

BUILD_DIR := $(CONFIG_DIR)/build
BIN_DIR := $(CONFIG_DIR)/bin

# Flags
CFLAGS := -Werror -Wall -Wextra -MMD -MP -g -I$(MAIN_INC) -I$(INC_DIR) -I$(TEST_INC)
CFLAGS += $(CONFIG_CFLAGS)

# Source and object files
CONFIG_SRCS := $(wildcard $(CONFIG_SRC)/*.c)
GENERIC_TESTS_SRCS := $(wildcard $(GENERIC_TESTS_SRC)/*.c)
MAIN_SRCS := $(wildcard $(MAIN_SRC)/*.c)

CONFIG_OBJS := $(patsubst $(CONFIG_SRC)/%.c, $(BUILD_DIR)/test_%.o, $(CONFIG_SRCS))
GENERIC_TESTS_OBJS := $(patsubst $(GENERIC_TESTS_SRC)/%.c, $(BUILD_DIR)/test_%.o, $(GENERIC_TESTS_SRC))
MAIN_OBJS := $(patsubst $(MAIN_SRC)/%.c, $(BUILD_DIR)/main_%.o, $(MAIN_SRCS))

DEPS := $(CONFIG_OBJS:.o=.d) $(MAIN_OBJS:.o=.d)

# Targets
CONFIG_BINS := $(patsubst $(CONFIG_SRC)/%.c, $(BIN_DIR)/test_%, $(CONFIG_SRCS))
CONFIG_BINS += $(patsubst $(GENERIC_TESTS_SRC)/%.c, $(BIN_DIR)/test_%, $(GENERIC_TESTS_SRCS))

all: $(CONFIG_BINS)

# Link each config obj with all main objs to produce a binary
$(BIN_DIR)/test_%: $(BUILD_DIR)/test_%.o $(MAIN_OBJS)
	@mkdir -p $(BIN_DIR)
	$(CC) $(CFLAGS) $^ -o $@

# Compile config source files
$(BUILD_DIR)/test_%.o: $(CONFIG_SRC)/%.c
	@mkdir -p $(BUILD_DIR)
	$(CC) $(CFLAGS) -c $< -o $@

# Compile generic tests source files
$(BUILD_DIR)/test_%.o: $(GENERIC_TESTS_SRC)/%.c
	@mkdir -p $(BUILD_DIR)
	$(CC) $(CFLAGS) -c $< -o $@

# Compile main source files
$(BUILD_DIR)/main_%.o: $(MAIN_SRC)/%.c
	@mkdir -p $(BUILD_DIR)
	$(CC) $(CFLAGS) -c $< -o $@

run: $(CONFIG_BINS)
	@failed="" && passed=0 && failed_count=0 && \
	for bin in $^; do \
		printf "\033[0;32m==> Running $$bin...\033[0m\n"; \
		if $$bin; then \
			printf "\033[0;32m✔ $$bin passed\033[0m\n"; \
			passed=$$((passed + 1)); \
		else \
			printf "\033[0;31m✘ $$bin failed\033[0m\n"; \
			failed="$$failed $$bin"; \
			failed_count=$$((failed_count + 1)); \
		fi; \
	done; \
	total=$$((passed + failed_count)); \
	if [ -n "$$failed" ]; then \
		printf "\033[0;31mSome tests failed:\033[0m\n"; \
		for f in $$failed; do \
			printf "  \033[0;31m✘ $$f\033[0m\n"; \
		done; \
		printf "\033[0;33mSummary: $$passed passed, $$failed_count failed, $$total total\033[0m\n"; \
		exit 1; \
	else \
		printf "\033[0;32mAll tests passed ✅\033[0m\n"; \
		printf "\033[0;32mSummary: $$passed passed, 0 failed, $$total total\033[0m\n"; \
	fi


clean:
	rm -rf $(BUILD_DIR) $(BIN_DIR)

-include $(DEPS)

.PHONY: all clean run
