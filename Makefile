SHELL := /bin/bash

ifeq ($(OS),Windows_NT)
    EXE_EXT := .exe
else
    EXE_EXT :=
endif

NAME        = ft_vox$(EXE_EXT)
NAME_DEBUG  = ft_vox_debug$(EXE_EXT)
TEST_NAME   = automated_tests$(EXE_EXT)

ifdef FORWARD_TRANSLATION
export FT_VOX_ENABLE_FORWARD_TRANSLATION := $(FORWARD_TRANSLATION)
endif
PYTHON          ?= python3
LINT_SCRIPT      = scripts/lint_sources.py
COVERAGE_SCRIPT  = scripts/coverage_report.py
GOINFRE_LOCAL_PREFIX ?= /goinfre/$(USER)/local

SRC         = \
    main.cpp \
    src/platform/libft_min.cpp \
    src/platform/libft_recursive_mutex_min.cpp

LIBFT_DIR   = Libft
LIBFT_LIBS  =
LIBFT_COMPILE_FLAGS = -Wall -Wextra -std=c++17 -DLIBFT_INTERNAL_HEADERS $(OPT_FLAGS)
LIBFT_DUMB_SRC = \
    $(LIBFT_DIR)/Modules/DUMB/dumb_controls.cpp \
    $(LIBFT_DIR)/Modules/DUMB/dumb_render.cpp

CC          = g++

OPT_LEVEL ?= 0

ifeq ($(OPT_LEVEL),0)
    OPT_FLAGS = -O0 -g
else ifeq ($(OPT_LEVEL),1)
    OPT_FLAGS = -O1 -flto -s -ffunction-sections -fdata-sections -Wl,--gc-sections
else ifeq ($(OPT_LEVEL),2)
    OPT_FLAGS = -O2 -flto -s -ffunction-sections -fdata-sections -Wl,--gc-sections
else ifeq ($(OPT_LEVEL),3)
    OPT_FLAGS = -O3 -flto -s -ffunction-sections -fdata-sections -Wl,--gc-sections
else
    $(error Unsupported OPT_LEVEL=$(OPT_LEVEL))
endif

COMPILE_FLAGS = -Wall -Werror -Wextra -std=c++17 -Wmissing-declarations \
                -Wold-style-cast -Wshadow -Wconversion -Wformat=2 -Wundef \
                -Wfloat-equal -Wconversion -Wzero-as-null-pointer-constant \
                -I. $(OPT_FLAGS)

ifneq ($(shell uname -s 2>/dev/null),Darwin)
    COMPILE_FLAGS += -Wodr -Wuseless-cast -Wmaybe-uninitialized
endif
DEPFLAGS = -MMD -MP

CFLAGS = $(COMPILE_FLAGS)

REPRODUCIBLE ?= 1
SOURCE_DATE_EPOCH ?= 1700000000

ifneq ($(REPRODUCIBLE),0)
    export SOURCE_DATE_EPOCH
    COMPILE_FLAGS += -ffile-prefix-map=$(CURDIR)=. -fmacro-prefix-map=$(CURDIR)=. -fdebug-prefix-map=$(CURDIR)=. -frandom-seed=ft_vox
endif

ifeq ($(OS),Windows_NT)
    MKDIR   = mkdir
    RMDIR   = rmdir /S /Q
    RM      = del /F /Q
else
    MKDIR   = mkdir -p
    RMDIR   = rm -rf
    RM      = rm -f
endif

BUILD_LOG_DIR = ./build_logs

OBJ_DIR         = ./objs
OBJ_DIR_DEBUG   = ./objs_debug
OBJ_DIR_TEST    = ./objs_tests

ENABLE_LTO  ?= 0
ENABLE_PGO  ?= 0
COVERAGE    ?= 0
COVERAGE_LINE_THRESHOLD ?= 60
COVERAGE_BRANCH_THRESHOLD ?= 65
export ENABLE_LTO ENABLE_PGO COVERAGE

ifeq ($(ENABLE_LTO),1)
    COMPILE_FLAGS   += -flto
    LDFLAGS  += -flto
endif

ifeq ($(ENABLE_PGO),1)
    COMPILE_FLAGS  += -fprofile-generate
endif

ifeq ($(COVERAGE),1)
    COMPILE_FLAGS  += --coverage
    LDFLAGS        += --coverage
endif

ifeq ($(DEBUG),1)
    CFLAGS    += -DDEBUG=1
    OBJ_DIR    = $(OBJ_DIR_DEBUG)
    TARGET     = $(NAME_DEBUG)
else
    TARGET     = $(NAME)
endif

export COMPILE_FLAGS

ifeq ($(OS),Windows_NT)
    LDFLAGS     =
else
    UNAME_S := $(shell uname -s)
    ifeq ($(UNAME_S),Darwin)
        LIBFT_DUMB_SRC += \
            $(LIBFT_DIR)/Modules/Compatebility/Compatebility_dumb_controls_platform_macos.mm \
            $(LIBFT_DIR)/Modules/Compatebility/Compatebility_dumb_render_platform_macos.mm
        LDFLAGS = -framework Cocoa -framework CoreGraphics -framework ApplicationServices
    else
        LIBFT_DUMB_SRC += \
            $(LIBFT_DIR)/Modules/Compatebility/Compatebility_dumb_controls_linux_x11.cpp \
            $(LIBFT_DIR)/Modules/Compatebility/Compatebility_dumb_render_linux_x11.cpp
        LDFLAGS = -lX11 -lXext -lGL
    endif
endif

LIBFT_DUMB_OBJS = \
    $(patsubst %.cpp,$(OBJ_DIR)/%.o,$(filter %.cpp,$(LIBFT_DUMB_SRC))) \
    $(patsubst %.mm,$(OBJ_DIR)/%.o,$(filter %.mm,$(LIBFT_DUMB_SRC)))

OBJS        = $(SRC:%.cpp=$(OBJ_DIR)/%.o)

OBJS_NO_MAIN = $(filter-out $(OBJ_DIR)/main.o,$(OBJS))

.SILENT:

TOTAL_OBJS          := $(words $(OBJS))

TEST_SRC    = main.cpp

TEST_OBJS   = $(TEST_SRC:%.cpp=$(OBJ_DIR_TEST)/%.o)

TOTAL_TEST_OBJS     := $(words $(TEST_OBJS))
DEPS        = $(OBJS:.o=.d) $(TEST_OBJS:.o=.d)

all: dirs $(TARGET)

tests: dirs $(TEST_NAME)

dirs:
	@-$(MKDIR) $(OBJ_DIR)
	@-$(MKDIR) $(OBJ_DIR_DEBUG)
	@-$(MKDIR) $(OBJ_DIR_TEST)

$(BUILD_LOG_DIR):
	@-$(MKDIR) $(BUILD_LOG_DIR)

install_cobc:
	@if ! command -v cobc >/dev/null 2>&1; then \
		apt-get update && apt-get install -y gnucobol; \
	else \
		printf 'cobc already installed.\n'; \
	fi

tests_with_cobc: install_cobc
	$(MAKE) tests
	$(MAKE) test

submodule_init:
	git submodule update --init --recursive
	git submodule update --remote --recursive

submodule_update:
	@branch="$$(git config -f .gitmodules --get submodule.Libft.branch || printf main)"; \
	printf '\033[1;36m[FT_VOX SUBMODULE] Fetching Libft origin/%s\033[0m\n' "$$branch"; \
	git -C Libft fetch origin "$$branch"; \
	printf 'Current Libft commit: %s\n' "$$(git -C Libft rev-parse --short HEAD)"; \
	printf 'Latest origin/%s:   %s\n' "$$branch" "$$(git -C Libft rev-parse --short origin/$$branch)"; \
	printf 'Type yes to update Libft to origin/%s: ' "$$branch"; \
	read answer; \
	if [ "$$answer" = "yes" ]; then \
		git -C Libft checkout "origin/$$branch"; \
		printf '\033[1;36m[FT_VOX SUBMODULE] Libft updated to %s\033[0m\n' "$$(git -C Libft rev-parse --short HEAD)"; \
	else \
		printf 'Cancelled. Libft was not updated.\n'; \
	fi

debug:
	$(MAKE) all DEBUG=1

$(TARGET): $(OBJS) $(LIBFT_DUMB_OBJS) $(LIBFT_LIBS)
	@printf '\033[1;36m[FT_VOX BUILD] Linking %s\033[0m\n' "$@"
	@$(CC) $(CFLAGS) $(OBJS) $(LIBFT_DUMB_OBJS) $(LIBFT_LIBS) -o $@ $(LDFLAGS)

ifneq ($(strip $(LIBFT_LIBS)),)
$(LIBFT_LIBS):
	@$(MAKE) -C $(dir $@) $(notdir $@) COMPILE_FLAGS="$(LIBFT_COMPILE_FLAGS)"
endif

$(TEST_NAME): $(TEST_OBJS) $(OBJS_NO_MAIN) $(TARGET) $(LIBFT_DUMB_OBJS) $(LIBFT_LIBS)
	@printf '\033[1;36m[FT_VOX BUILD] Linking %s\033[0m\n' "$@"
	@$(CC) $(CFLAGS) $(TEST_OBJS) $(OBJS_NO_MAIN) $(LIBFT_DUMB_OBJS) $(LIBFT_LIBS) -o $@ $(LDFLAGS)

$(OBJ_DIR)/$(LIBFT_DIR)/%.o: $(LIBFT_DIR)/%.cpp
	@-$(MKDIR) $(dir $@)
	@$(CC) $(LIBFT_COMPILE_FLAGS) $(DEPFLAGS) -c $< -o $@
	@printf '\033[1;36m[FT_VOX LIBFT] %s\033[0m\n' "$<"

$(OBJ_DIR)/$(LIBFT_DIR)/%.o: $(LIBFT_DIR)/%.mm
	@-$(MKDIR) $(dir $@)
	@$(CC) $(LIBFT_COMPILE_FLAGS) -x objective-c++ $(DEPFLAGS) -c $< -o $@
	@printf '\033[1;36m[FT_VOX LIBFT] %s\033[0m\n' "$<"

$(OBJ_DIR)/%.o: %.cpp
	@-$(MKDIR) $(dir $@)
	@$(CC) $(CFLAGS) $(DEPFLAGS) -c $< -o $@
	@if [ $(TOTAL_OBJS) -gt 0 ] && [[ " $(OBJS) " == *" $@ "* ]]; then \
		built=0; \
		for obj in $(OBJS); do \
			[ -f "$$obj" ] && built=$$((built + 1)); \
		done; \
		printf '\033[1;36m[FT_VOX PROGRESS] %s (%d/%d)\033[0m\n' "$<" $$built $(TOTAL_OBJS); \
	fi

$(OBJ_DIR_TEST)/%.o: %.cpp
	@-$(MKDIR) $(dir $@)
	@$(CC) $(CFLAGS) $(DEPFLAGS) -c $< -o $@
	@if [ $(TOTAL_TEST_OBJS) -gt 0 ]; then \
		built=$$(find $(OBJ_DIR_TEST) -type f -name '*.o' | wc -l); \
		printf '\033[1;36m[FT_VOX TEST PROGRESS] %s (%d/%d)\033[0m\n' "$<" $$built $(TOTAL_TEST_OBJS); \
	fi

-include $(DEPS)

clean:
	-$(RMDIR) $(OBJ_DIR) $(OBJ_DIR_DEBUG) $(OBJ_DIR_TEST)
	-$(RM) test_example_compiler.c test_example_compiler.bin test_example_compiler.txt
	-$(RM) test_example_invalid_compiler.c test_example_invalid_compiler.bin test_example_invalid_compiler.log
	-$(RM) test_runtime_file.txt
	-$(RMDIR) $(BUILD_LOG_DIR)

fclean: clean
	-$(RM) $(NAME) $(NAME_DEBUG) $(TEST_NAME)
	-$(RMDIR) $(OBJ_DIR) $(OBJ_DIR_DEBUG) $(OBJ_DIR_TEST) data

re: fclean all

test: $(TEST_NAME)
	@if [ -x "$(GOINFRE_LOCAL_PREFIX)/bin/cobc" ]; then \
		PATH="$(GOINFRE_LOCAL_PREFIX)/bin:$$PATH" \
		LD_LIBRARY_PATH="$(GOINFRE_LOCAL_PREFIX)/lib:$(GOINFRE_LOCAL_PREFIX)/lib64$${LD_LIBRARY_PATH:+:$$LD_LIBRARY_PATH}" \
		CPATH="$(GOINFRE_LOCAL_PREFIX)/include$${CPATH:+:$$CPATH}" \
		LIBRARY_PATH="$(GOINFRE_LOCAL_PREFIX)/lib:$(GOINFRE_LOCAL_PREFIX)/lib64$${LIBRARY_PATH:+:$$LIBRARY_PATH}" \
		./$(TEST_NAME); \
	else \
		./$(TEST_NAME); \
	fi

both: all debug

re_both: re both

lint:
	$(PYTHON) $(LINT_SCRIPT)

coverage:
	@-$(RMDIR) $(OBJ_DIR)
	@-$(RMDIR) $(OBJ_DIR_DEBUG)
	@-$(RMDIR) $(OBJ_DIR_TEST)
	@-$(RM) $(TARGET)
	@-$(RM) $(NAME_DEBUG)
	@-$(RM) $(TEST_NAME)
	$(MAKE) tests OPT_LEVEL=0 COVERAGE=1
	$(MAKE) test COVERAGE=1
	$(PYTHON) $(COVERAGE_SCRIPT) --object-dir $(OBJ_DIR) --threshold-lines $(COVERAGE_LINE_THRESHOLD) --threshold-branches $(COVERAGE_BRANCH_THRESHOLD)

ci-coverage:
	$(MAKE) coverage

ci-build:
	$(MAKE) fclean
	$(MAKE) all OPT_LEVEL=2
	$(MAKE) debug

ci-test:
	$(MAKE) test

ci-lint: lint

ci:
	$(MAKE) ci-build
	$(MAKE) ci-test
	$(MAKE) ci-lint
	$(MAKE) ci-coverage

.PHONY: all dirs clean fclean re debug both re_both tests test lint coverage ci-build ci-test ci-lint ci-coverage ci submodule_init submodule_update ft_vox
