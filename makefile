CC = gcc
CFLAGS = -Wall -ansi
LDFLAGS = -lSDL -lGL -lGLU -lm -lSDL_image -lglut

APP_BIN = KassPong

SRC_PATH = project
INC_PATH = -I project

OBJ_PATH = obj
IMG_PATH = img
BIN_PATH = bin
LIB_PATH = lib

SRC_FILES = $(shell find $(SRC_PATH) -name '*.c')
OBJ_FILES = $(patsubst $(SRC_PATH)/%.c, $(OBJ_PATH)/%.o, $(SRC_FILES))

all: $(APP_BIN)

$(APP_BIN): $(OBJ_FILES)
	@mkdir -p $(BIN_PATH)
	$(CC) -o $(BIN_PATH)/$(APP_BIN) $(OBJ_FILES) $(LDFLAGS)

$(OBJ_PATH)/%.o: $(SRC_PATH)/%.c
	@mkdir -p "$(@D)"
	$(CC) -c $< -o $@ $(CFLAGS) $(INC_PATH)

clean:
	rm $(OBJ_FILES) $(BIN_PATH)/$(APP_BIN)

test:
	$(BIN_PATH)/$(APP_BIN) res/grid.txt
