.PHONY: clean all debug

CC = gcc
BASE_FLAGS = -Wall -Wextra -Wpedantic -Wshadow -Wstrict-prototypes -Iinclude `pkg-config --cflags glib-2.0` -Werror=discarded-qualifiers -D_GNU_SOURCE -lm
DEBUG_FLAGS = -O0 -ggdb -g
PERF_FLAGS = -O3 -funroll-loops -finline-functions -ftree-vectorize -flto 
LDFLAGS = `pkg-config --libs glib-2.0` 

SRC_DIR = src
OBJ_DIR = obj

INT_SRC = $(SRC_DIR)/interactive_main.c \
          $(shell find $(SRC_DIR)/interactive -name '*.c')

CORE_SRC = $(shell find $(SRC_DIR)/core $(SRC_DIR)/entities $(SRC_DIR)/queries -name '*.c') \
           $(SRC_DIR)/validation.c 

MAIN_SRC = $(SRC_DIR)/main.c

TESTS_SRC = $(SRC_DIR)/tests_main.c \
            $(shell find $(SRC_DIR)/tests -name '*.c' 2>/dev/null)

# Map objects
CORE_OBJ = $(CORE_SRC:$(SRC_DIR)/%.c=$(OBJ_DIR)/%.o)
MAIN_OBJ = $(MAIN_SRC:$(SRC_DIR)/%.c=$(OBJ_DIR)/%.o)
TESTS_OBJ = $(TESTS_SRC:$(SRC_DIR)/%.c=$(OBJ_DIR)/%.o)
INT_OBJ = $(INT_SRC:$(SRC_DIR)/%.c=$(OBJ_DIR)/%.o)

TARGET_MAIN = programa-principal
TARGET_TESTES = programa-testes
TARGET_INT = programa-interativo

# --- Flag Selection Logic ---
# Check if "debug" is in the command list passed to make (e.g., make debug or make debug programa-principal)
ifneq ($(filter debug,$(MAKECMDGOALS)),)
    # If "debug" is present, use DEBUG_FLAGS
    CFLAGS = $(BASE_FLAGS) $(DEBUG_FLAGS)
else
    # Otherwise (make, make all, make programa-principal), use PERF_FLAGS
    CFLAGS = $(BASE_FLAGS) $(PERF_FLAGS)
endif

# --- Targets ---

# Default target
all: $(TARGET_MAIN) $(TARGET_TESTES) $(TARGET_INT)

# Target - interativo
interativo: $(TARGET_INT)

# Debug target: Force clean to ensure recompilation with debug flags
# Depends on all to build everything if no specific executable is requested
debug: clean all

# Main Program Linking (Batch Mode)
$(TARGET_MAIN): $(MAIN_OBJ) $(CORE_OBJ)
	$(CC) $^ $(CFLAGS) $(LDFLAGS) -o $@

# Test Program Linking
$(TARGET_TESTES): $(TESTS_OBJ) $(CORE_OBJ)
	$(CC) $^ $(CFLAGS) $(LDFLAGS) -o $@

# Interactive Program Linking
$(TARGET_INT): $(INT_OBJ) $(CORE_OBJ)
	$(CC) $^ $(CFLAGS) $(LDFLAGS) -lncurses -lhistory -lreadline -o $@

# Object compilation
$(OBJ_DIR)/%.o: $(SRC_DIR)/%.c
	@mkdir -p $(dir $@)
	$(CC) $(CFLAGS) -c $< -o $@

clean:
	rm -rf $(OBJ_DIR) $(TARGET_MAIN) $(TARGET_TESTES) $(TARGET_INT)