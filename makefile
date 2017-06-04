CC = gcc
CFLAGS = -Wall -ansi -g
LDFLAGS = -lSDL -lGL -lGLU -lm -lSDL_image -lglut

APP_BIN = KassPong

SRC_PATH = project
INC_PATH = -iquote project

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
	rm -f $(OBJ_FILES)

fclean: clean
	rm -f $(BIN_PATH)/$(APP_BIN)

re: fclean all

test:
	$(BIN_PATH)/$(APP_BIN) res/grid.txt cat teemo

.PHONY: all clean fclean re test
.SUFFIXES:
