# Source
OBJS_DIR := ./build
SOURCE_DIR := ./src
LIB_DIR := ./lib
INCLUDE_DIR := ./include
TEST_SRC_DIR := ./test

# Flags
CXX := g++
CXXFLAGS := -std=c++23 -fsanitize=address -g
LINKFLAGS := -static-libasan
TEST_CXXFLAGS := -std=c++23 -I$(INCLUDE_DIR) -fsanitize=address -g
TEST_LINKFLAGS := -L$(LIB_DIR) -ltehimage -static-libasan

# Outputs
LIB := $(LIB_DIR)/libtehimage.so
TEST := image_test

# Files
SOURCE_FILES := $(wildcard $(SOURCE_DIR)/*.cpp)
SOURCE_HEADERS := $(wildcard $(SOURCE_DIR)/*.h)
OBJS := $(subst $(SOURCE_DIR),$(OBJS_DIR), $(patsubst %.cpp,%.o,$(SOURCE_FILES)))
INCLUDE_HEADERS := $(subst $(SOURCE_DIR),$(INCLUDE_DIR), $(SOURCE_HEADERS))
TEST_SOURCE := $(wildcard $(TEST_SRC_DIR)/*.cpp)
TEST_HEADERS := $(wildcard $(TEST_SRC_DIR)/*.h)
TEST_OBJS = $(subst $(TEST_SRC_DIR),$(OBJS_DIR), $(patsubst %.cpp,%.o,$(TEST_SOURCE)))

# Main lib
$(LIB): $(INCLUDE_HEADERS) $(OBJS)
	$(CXX) $(OBJS) $(CXXFLAGS) $(LINKFLAGS) -shared -o $(LIB)

$(OBJS_DIR)/%.o: $(SOURCE_DIR)/%.cpp $(SOURCE_DIR)/%.h
	$(CXX) $(CXXFLAGS) -c $< -o $@

$(INCLUDE_DIR)/%.h: $(SOURCE_DIR)/%.h lib
	cp $< $@

# Test binary
$(TEST): $(TEST_OBJS) $(LIB)
	$(CXX) $(TEST_OBJS) $(TEST_CXXFLAGS) $(TEST_LINKFLAGS) -o $(TEST)

$(OBJS_DIR)/%.o: $(TEST_SRC_DIR)/%.cpp $(TEST_SOURCE) $(TEST_HEADERS)
	$(CXX) $(TEST_CXXFLAGS) -c $< -o $@


# Phony
.PHONY: clean test
clean:
	rm -f $(LIB)
	rm -f $(OBJS_DIR)/*.o
	rm -f $(TEST)

test: $(TEST)

$(OBJS_DIR)/reader.o: $(SOURCE_DIR)/debug.h
$(OBJS_DIR)/image.o:
$(OBJS_DIR)/PNGImage.o: $(SOURCE_DIR)/debug.h $(SOURCE_DIR)/image.h
$(OBJS_DIR)/BMPImage.o: $(SOURCE_DIR)/image.h
$(OBJS_DIR)/zlib.o:
