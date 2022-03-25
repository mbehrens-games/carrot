MINGW_DIR = /opt/local/x86_64-w64-mingw32
SDL_DIR = /Users/mike/Desktop/SDL2/x86_64-w64-mingw32

#CC = /opt/local/bin/clang-mp-8.0
#CFLAGS = --target=x86_64-w64-mingw32 -pedantic -Wall -Wextra -std=gnu89 -m64 -O2 -I$(MINGW_DIR)/include -I$(SDL_DIR)/include -I$(GLAD_DIR)/include -Dmain=SDL_main
#LDFLAGS = -L$(MINGW_DIR)/lib -L$(SDL_DIR)/lib -lmingw32 -lSDL2main -lSDL2 -lSDL2_mixer -m64 -mwindows -W1,--strip-all

CC = /opt/local/bin/x86_64-w64-mingw32-gcc
CPP = /opt/local/bin/x86_64-w64-mingw32-cpp
CFLAGS = -pedantic -Wall -Wextra -O2 -I$(MINGW_DIR)/include -I$(SDL_DIR)/include -I$(GLAD_DIR)/include -Dmain=SDL_main -m64 -mwin32 -mwindows
LDFLAGS = -L$(MINGW_DIR)/lib -L$(SDL_DIR)/lib -lmingw32 -lSDL2main -lSDL2 -lSDL2_mixer -m64 -mwin32 -mwindows

TARGET = carrot.exe

SRC_DIR = src
OBJ_DIR = obj
BIN_DIR = bin

GLAD_DIR = glad

SRCS = $(wildcard $(SRC_DIR)/*.c)
OBJS = $(SRCS:$(SRC_DIR)/%.c=$(OBJ_DIR)/%.o)
DEPS = $(OBJS:$(OBJ_DIR)/%.o=$(OBJ_DIR)/%.d)

GLAD_SRCS = $(GLAD_DIR)/src/glad.c
GLAD_OBJS = $(OBJ_DIR)/glad.o
GLAD_DEPS = $(OBJ_DIR)/glad.d

$(BIN_DIR)/$(TARGET): $(OBJS) $(GLAD_OBJS)
	@$(CC) $(CFLAGS) $(OBJS) $(GLAD_OBJS) -o $@ $(LDFLAGS)

$(OBJS): $(OBJ_DIR)/%.o : $(SRC_DIR)/%.c
	@$(CC) $(CFLAGS) -c $< -o $@

$(GLAD_OBJS): $(OBJ_DIR)/%.o : $(GLAD_DIR)/src/%.c
	@$(CC) $(CFLAGS) -c $< -o $@

-include $(DEPS)

$(DEPS): $(OBJ_DIR)/%.d : $(SRC_DIR)/%.c
	@$(CPP) $(CFLAGS) $< -MM -MT $(@:.d=.o) >$@

-include $(GLAD_DEPS)

$(GLAD_DEPS): $(OBJ_DIR)/%.d : $(GLAD_DIR)/src/%.c
	@$(CPP) $(CFLAGS) $< -MM -MT $(@:.d=.o) >$@

.PHONY: clean
clean:
	rm -f $(OBJS)
	rm -f $(DEPS)
	rm -f $(GLAD_OBJS)
	rm -f $(GLAD_DEPS)
	rm -f $(BIN_DIR)/$(TARGET)
