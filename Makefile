CC = gcc

CFLAGS =
DEBUGFLAGS = -g
RELEASEFLAGS = -O3

GLEWDIR = libs/external/glew-2.1.0
GLEWFLAGS = -I$(GLEWDIR)/include -L$(GLEWDIR)/lib/Release/x64

GLFWDIR = libs/external/glfw-3.4.bin.WIN64
GLFWFLAGS = -I$(GLFWDIR)/include -L$(GLFWDIR)/lib-mingw-w64

FREETYPE_DIR = libs/external/freetype
FREETYPE_FLAGS = -I$(FREETYPE_DIR) -L$(FREETYPE_DIR)

CK_DIR = ../Ck
CK_BUILD_DEBUG = $(CK_DIR)/build/Debug
CK_FLAGS = -I$(CK_DIR)/libs -L$(CK_BUILD_DEBUG)

LIBFLAGS =  $(GLFWFLAGS) $(GLEWFLAGS) $(FREETYPE_FLAGS) $(CK_FLAGS)

LIBS = -lCK -lglew32 -lglfw3 -lopengl32 -lgdi32 -luser32 -lshell32 -lfreetype

SRCDIR = src
SRCS = $(wildcard $(SRCDIR)/*.c)

HEADERS = libs/vinote.h

BUILD_DIR = build
DEBUG_DIR = $(BUILD_DIR)/Debug
RELEASE_DIR = $(BUILD_DIR)/Release

TARGET_DEBUG = $(DEBUG_DIR)/ViNote.exe
TARGET_RELEASE = $(RELEASE_DIR)/ViNote.exe

DEBUG_OBJS = $(patsubst $(SRCDIR)/%, $(DEBUG_DIR)/%, $(SRCS:.c=.obj))
RELEASE_OBJS = $(patsubst $(SRCDIR)/%, $(RELEASE_DIR)/%, $(SRCS:.c=.obj))

all: debug

debug: CFLAGS += $(DEBUGFLAGS) $(LIBFLAGS)
debug: $(TARGET_DEBUG)

release: CFLAGS += $(RELEASEFLAGS) $(LIBFLAGS)
release: $(TARGET_RELEASE)

$(TARGET_DEBUG): $(DEBUG_OBJS) $(CK_BUILD_DEBUG)/libCK.a
	mkdir -p $(DEBUG_DIR)
	$(CC) $(CFLAGS)  -o $(TARGET_DEBUG) $(DEBUG_OBJS) $(LDFLAGS) $(LIBS)

$(TARGET_RELEASE): $(RELEASE_OBJS)
	mkdir -p $(RELEASE_DIR)
	$(CC) $(CFLAGS) -o $(TARGET_RELEASE) $(RELEASE_OBJS) $(LDFLAGS) $(LIBS)

# Ensure CK static library is built before linking
$(CK_BUILD_DEBUG)/libCK.a:
	$(MAKE) -C $(CK_DIR) static-debug

$(DEBUG_DIR)/%.obj: $(SRCDIR)/%.c $(HEADERS)
	mkdir -p $(DEBUG_DIR)
	$(CC) $(CFLAGS) -c $< -o $@

$(RELEASE_DIR)/%.obj: $(SRCDIR)/%.c $(HEADERS)
	mkdir -p $(RELEASE_DIR)
	$(CC) $(CFLAGS) -c $< -o $@

clean:
	rm -f *.pdb
	rm -f $(DEBUG_DIR)/*.pdb
	rm -f $(RELEASE_DIR)/*.pdb
	rm -f $(DEBUG_OBJS) $(RELEASE_OBJS)

fclean: clean
	rm -rf $(BUILD_DIR)
	rm -f $(TARGET_DEBUG) $(TARGET_RELEASE)

re: fclean all

.PHONY: all debug release clean fclean re
