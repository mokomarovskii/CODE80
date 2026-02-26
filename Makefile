CXX = g++
CXXFLAGS = -std=c++17 -Wall -Wextra -Iinclude
AR = ar
ARFLAGS = rcs

LIB_DIR = lib
OBJ_DIR = build
BIN_DIR = bin
SRC_DIR = src
APP_DIR = app

LIB_NAME = $(LIB_DIR)/libtimeline.a

LIB_SOURCES = \
	$(SRC_DIR)/event_list.cpp \
	$(SRC_DIR)/database.cpp \
	$(SRC_DIR)/input.cpp \
	$(SRC_DIR)/reports.cpp

LIB_OBJECTS = $(patsubst $(SRC_DIR)/%.cpp,$(OBJ_DIR)/%.o,$(LIB_SOURCES))

IMGUI_FLAGS := $(shell pkg-config --cflags --libs glfw3 imgui 2>/dev/null)

all: $(LIB_NAME) $(BIN_DIR)/timeline_gui $(BIN_DIR)/test_runner

$(OBJ_DIR) $(BIN_DIR) $(LIB_DIR):
	mkdir -p $@

$(OBJ_DIR)/%.o: $(SRC_DIR)/%.cpp | $(OBJ_DIR)
	$(CXX) $(CXXFLAGS) -c $< -o $@

$(LIB_NAME): $(LIB_OBJECTS) | $(LIB_DIR)
	$(AR) $(ARFLAGS) $@ $^

$(BIN_DIR)/timeline_gui: $(APP_DIR)/gui.cpp $(LIB_NAME) | $(BIN_DIR)
	@if [ -n "$(IMGUI_FLAGS)" ]; then \
		$(CXX) $(CXXFLAGS) -DTIMELINE_IMGUI_GUI $< -L$(LIB_DIR) -ltimeline -lsqlite3 $(IMGUI_FLAGS) -lGL -o $@; \
	else \
		$(CXX) $(CXXFLAGS) $< -L$(LIB_DIR) -ltimeline -lsqlite3 -o $@; \
		echo "[warn] imgui/glfw pkg-config files not found; built fallback GUI binary."; \
	fi

$(BIN_DIR)/test_runner: tests/test_runner.cpp $(LIB_NAME) | $(BIN_DIR)
	$(CXX) $(CXXFLAGS) $< -L$(LIB_DIR) -ltimeline -lsqlite3 -o $@

run-gui: all
	./$(BIN_DIR)/timeline_gui

test: all
	./$(BIN_DIR)/test_runner

clean:
	rm -rf $(OBJ_DIR) $(BIN_DIR) $(LIB_DIR) events.db test_events.db

.PHONY: all clean run-gui test
