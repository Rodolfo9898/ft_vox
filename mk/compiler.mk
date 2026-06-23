CC = g++

OPT_LEVEL ?= 2
UNAME_S    := $(shell uname -s)

ifeq ($(OPT_LEVEL),0)
    OPT_FLAGS = -O0 -g
else ifeq ($(OPT_LEVEL),1)
    OPT_FLAGS = -O1
else ifeq ($(OPT_LEVEL),2)
    OPT_FLAGS = -O2
else ifeq ($(OPT_LEVEL),3)
    OPT_FLAGS = -O3
else
    $(error Unsupported OPT_LEVEL=$(OPT_LEVEL))
endif

COMPILE_FLAGS = -Wall -Werror -Wextra -std=c++17 -Wmissing-declarations \
                -Wold-style-cast -Wshadow -Wconversion -Wformat=2 -Wundef \
                -Wfloat-equal -Wzero-as-null-pointer-constant -Wodr \
                -I. -DLIBFT_INTERNAL_HEADERS -DGAME_USE_VOXEL_REGION_BACKEND $(OPT_FLAGS)

ifeq ($(OS),Windows_NT)
    COMPILE_FLAGS += -Wuseless-cast -Wmaybe-uninitialized
    ifneq ($(OPT_LEVEL),0)
        COMPILE_FLAGS += -flto -ffunction-sections -fdata-sections
    endif
else ifeq ($(UNAME_S),Darwin)
    COMPILE_FLAGS += -Wno-format-nonliteral -Wno-tautological-compare
    ifneq ($(OPT_LEVEL),0)
        COMPILE_FLAGS += -flto -ffunction-sections -fdata-sections
    endif
else
    COMPILE_FLAGS += -Wuseless-cast -Wmaybe-uninitialized
    ifneq ($(OPT_LEVEL),0)
        COMPILE_FLAGS += -flto -ffunction-sections -fdata-sections
    endif
endif

DEPFLAGS = -MMD -MP
CFLAGS = $(COMPILE_FLAGS)

REPRODUCIBLE ?= 1
SOURCE_DATE_EPOCH ?= 1700000000

ifneq ($(REPRODUCIBLE),0)
    export SOURCE_DATE_EPOCH
    COMPILE_FLAGS += -ffile-prefix-map=$(CURDIR)=. -fmacro-prefix-map=$(CURDIR)=. \
                     -fdebug-prefix-map=$(CURDIR)=. -frandom-seed=ft_vox
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

FT_CFLAGS := $(shell pkg-config --cflags freetype2 2>/dev/null)
FT_LIBS   := $(shell pkg-config --libs   freetype2 2>/dev/null)
COMPILE_FLAGS += $(FT_CFLAGS)

ifeq ($(OS),Windows_NT)
    LDFLAGS = -lopengl32 -lgdi32 -lws2_32 -ldbghelp $(FT_LIBS)
    ifneq ($(OPT_LEVEL),0)
        LDFLAGS += -flto -s -Wl,--gc-sections
    endif
else ifeq ($(UNAME_S),Darwin)
    LDFLAGS = -lz -framework Cocoa -framework CoreGraphics \
              -framework ApplicationServices -framework QuartzCore \
              -framework AudioToolbox -framework OpenGL \
              -framework Carbon -lobjc -lpthread -lsqlite3 $(FT_LIBS)
    ifneq ($(OPT_LEVEL),0)
        LDFLAGS += -flto -Wl,-dead_strip
    endif
else
    LDFLAGS = -lX11 -lXext -lXi -lGL $(FT_LIBS)
    ifneq ($(OPT_LEVEL),0)
        LDFLAGS += -flto -s -Wl,--gc-sections
    endif
endif

ifeq ($(DEBUG),1)
    ifneq ($(OS),Windows_NT)
        ifneq ($(UNAME_S),Darwin)
            LDFLAGS += -rdynamic
        endif
    endif
endif
