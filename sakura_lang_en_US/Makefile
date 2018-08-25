# Makefile for MinGW32/MinGW-W64

ifndef PREFIX
PREFIX=
RCPREFIX=
else
ifeq ($(PREFIX),x86_64-w64-mingw32-)
RCPREFIX=$(PREFIX)
else
ifeq ($(PREFIX),i686-w64-mingw32-)
ifeq ($(OS),Windows_NT)
RCPREFIX=
else
RCPREFIX=$(PREFIX)
endif
endif
endif
endif

CC= $(PREFIX)gcc
CXX= $(PREFIX)g++
RC= $(RCPREFIX)windres
RM= cmd /c $(CURDIR)/../sakura/mingw32-del.bat

DEFINES= \
 -D_DEBUG \
 -D_UNICODE \
 -DUNICODE \
 -D_WIN32 \
 -DWIN32 \
 -D_WIN32_WINNT=0x500 \
 -DNOMINMAX \
 -DMINGW_HAS_SECURE_API=1
CFLAGS= -g -O0 \
 -finput-charset=utf-8 -fexec-charset=cp932 \
 -I. \
 $(DEFINES) $(MYCFLAGS)
CXXFLAGS= $(CFLAGS) $(MYCXXFLAGS)
LDFLAGS= \
 -mwindows \
 -municode \
 -static-libstdc++ \
 -static-libgcc
LIBS= \
 -shared \
 -dll \
 -s

exe= sakura_lang_en_US.dll

# ls *.cpp */*.cpp */*/*.cpp | sed -E -e "s/([[:alnum:]_]+)\.[[:alnum:]]+/\1.o \\\\/"
OBJS= \
sakura_lang_rc.o \

all: $(exe)

$(exe): sakura_lang.h $(OBJS)
	gcc $(LDFLAGS) $(OBJS) $(LIBS) -o $@

sakura_rc:
	$(MAKE) -C ../sakura_core sakura_rc.o

sakura_lang_rc.o: sakura_rc sakura_lang_rc.rc
	$(RC) -c utf-8 --language=0411 $(DEFINES) sakura_lang_rc.rc -o $@

clean:
	$(RM) $(exe) $(OBJS)

depend:
	$(CXX) -E -MM -w $(DEFINES) $(CXXFLAGS) *.cpp */*.cpp */*/*.cpp > depend.mak

.SUFFIXES: .cpp .o .rc
.PHONY: all clean depend

-include depend.mak
