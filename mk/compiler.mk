CC = g++

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

ENABLE_LTO  ?= 0
ENABLE_PGO  ?= 0
COVERAGE    ?= 0
COVERAGE_LINE_THRESHOLD ?= 60
COVERAGE_BRANCH_THRESHOLD ?= 65
export ENABLE_LTO ENABLE_PGO COVERAGE

ifeq ($(ENABLE_LTO),1)
    COMPILE_FLAGS += -flto
    LDFLAGS       += -flto
endif

ifeq ($(ENABLE_PGO),1)
    COMPILE_FLAGS += -fprofile-generate
endif

ifeq ($(COVERAGE),1)
    COMPILE_FLAGS += --coverage
    LDFLAGS       += --coverage
endif

ifeq ($(DEBUG),1)
    CFLAGS          += -DDEBUG=1 -fno-omit-frame-pointer
    OBJ_DIR          = $(OBJ_DIR_DEBUG)
    TARGET           = $(NAME_DEBUG)
    LIBFT_LINK_LIB   = $(LIBFT_FULL_DEBUG_LIB)
    LIBFT_LINK_FLAGS = $(LIBFT_LINK_LIB)
else
    TARGET           = $(NAME)
endif

export COMPILE_FLAGS

ifeq ($(OS),Windows_NT)
    LDFLAGS =
else
    UNAME_S := $(shell uname -s)
    ifeq ($(UNAME_S),Darwin)
        LIBFT_LINK_FLAGS = -Wl,-force_load,$(LIBFT_LINK_LIB)
        LDFLAGS = -lz -framework Cocoa -framework CoreGraphics -framework ApplicationServices -framework QuartzCore -framework AudioToolbox -lobjc -lpthread -lsqlite3
    else
        LDFLAGS = -lX11 -lXext -lGL
    endif
endif

ifeq ($(DEBUG),1)
    ifneq ($(OS),Windows_NT)
        ifneq ($(UNAME_S),Darwin)
            LDFLAGS += -rdynamic
        endif
    endif
endif
