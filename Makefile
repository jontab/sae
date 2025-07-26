CC       := gcc
LD       := gcc

BIN      := bin
INC      := inc
SRC      := src
VEN      := ven

CFLAGS   := -I$(INC)
LDFLAGS  := -lncurses -ltinfo -static

EXE      := $(BIN)/sae

INCS     := $(shell find $(INC) -name "*.h" | sort)
SRCS     := $(shell find $(SRC) -name "*.c" | sort)
OBJS     := $(patsubst %.c, %.o, $(SRCS))

debug:   CFLAGS += -O0 -g
debug:   all

release: CFLAGS += -O3
release: all

all:     $(EXE)

$(EXE): $(OBJS) $(LIBS)
	@echo "Linking   $@"
	@mkdir -p $(shell dirname $@)
	@$(LD) $^ -o $@ $(LDFLAGS)

%.o: %.c $(INCS)
	@echo "Compiling $@"
	@$(CC) -c $< -o $@ $(CFLAGS)

clean:
	@echo "Removing  binary"
	@rm -f $(EXE)

	@echo "Removing  objects"
	@rm -f $(OBJS)

.PHONY: clean
.PRECIOUS: %.o
