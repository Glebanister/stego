CC = gcc
COMPILE_FLAGS = -c -Wextra -Werror -g
SAN_FLAGS = -fsanitize=address
LINK_FLAGS =
LINK_SAN_FLAGS = -lasan
EXE = stego
OBJ = obj
INCLUDE = include
SOURCE = src

.PHONY: clean all san debug

ifeq ($(SAN), 1)
COMPILE_FLAGS += $(SAN_FLAGS)
LINK_FLAGS += $(LINK_SAN_FLAGS)
endif

all: $(EXE)

san:
	make SAN=1

$(EXE): $(OBJ)/main.o $(OBJ)/bmp.o $(OBJ)/stego.o | $(OBJ)
	$(CC) $(OBJ)/main.o $(OBJ)/bmp.o $(OBJ)/stego.o -o $(EXE) $(LINK_FLAGS)

$(OBJ):
	mkdir -p $(OBJ)

$(OBJ)/main.o: $(SOURCE)/main.c $(INCLUDE)/bmp.h $(INCLUDE)/stego.h $(INCLUDE)/errors.h | $(OBJ)
	$(CC) -o $(OBJ)/main.o $(COMPILE_FLAGS) $(SOURCE)/main.c -I$(INCLUDE)

$(OBJ)/bmp.o: $(SOURCE)/bmp.c $(INCLUDE)/bmp.h $(INCLUDE)/errors.h | $(OBJ)
	$(CC) -o $(OBJ)/bmp.o $(COMPILE_FLAGS) $(SOURCE)/bmp.c -I$(INCLUDE)

$(OBJ)/stego.o: $(SOURCE)/stego.c $(INCLUDE)/stego.h $(INCLUDE)/errors.h | $(OBJ)
	$(CC) -o $(OBJ)/stego.o $(COMPILE_FLAGS) $(SOURCE)/stego.c -I$(INCLUDE)

clean:
	rm -rf $(OBJ) $(EXE)
