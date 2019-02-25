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
RM= "$(CURDIR)/../sakura/mingw32-del.bat"

DEFINES= \
 -DWIN32 \
 -D_WIN32_WINNT=_WIN32_WINNT_WIN7 \
 -D_UNICODE \
 -DUNICODE \
 $(MYDEFINES)
CFLAGS= \
 -finput-charset=utf-8 \
 -fexec-charset=cp932 \
 -I. \
 $(DEFINES) $(MYCFLAGS)
CXXFLAGS= $(CFLAGS) $(MYCXXFLAGS)
LIBS= \
 -static \
 -lwinspool \
 -lole32 \
 -loleaut32 \
 -luuid \
 -lcomctl32 \
 -limm32 \
 -lmpr \
 -limagehlp \
 -lshlwapi \
 -lwinmm \
 -lwindowscodecs \
 -lmsimg32 \
 -mwindows \
 -municode \
 $(MYLIBS)

exe= sakura.exe

# ls *.cpp */*.cpp */*/*.cpp | sed -E -e "s/([[:alnum:]_]+)\.[[:alnum:]]+/\1.o \\\\/"
OBJS= \

GENERATED_FILES= \
Funccode_define.h \
Funccode_enum.h \
githash.h \

HEADERMAKETOOLDIR= ../HeaderMake
HEADERMAKE= $(HEADERMAKETOOLDIR)/HeaderMake.exe

all: $(exe)

$(exe): githash $(OBJS)
	$(CXX) -o $@ $(OBJS) $(LIBS)

Funccode_define.h: $(HEADERMAKE) Funccode_x.hsrc
	$(HEADERMAKE) -in=../sakura_core/Funccode_x.hsrc -out=../sakura_core/Funccode_define.h -mode=define

Funccode_enum.h: $(HEADERMAKE) Funccode_x.hsrc
	$(HEADERMAKE) -in=../sakura_core/Funccode_x.hsrc -out=../sakura_core/Funccode_enum.h -mode=enum -enum=EFunctionCode

githash:
	"$(CURDIR)/../sakura/githash.bat" ../sakura_core

stdafx: Funccode_enum.h StdAfx.h
	$(CXX) $(CXXFLAGS) -c StdAfx.h

.cpp.o: stdafx
	$(CXX) $(CXXFLAGS) -o $@ -c $<

$(HEADERMAKE): $(HEADERMAKETOOLDIR)/HeaderMake.cpp
	$(CXX) $(CXXFLAGS) $(HEADERMAKETOOLDIR)/HeaderMake.cpp -o $@ -static-libgcc

sakura_rc.o: Funccode_define.h githash sakura_rc.rc
	$(RC) -c utf-8 --language=0411 $(DEFINES) sakura_rc.rc -o $@

clean:
	$(RM) $(exe) $(OBJS) $(HEADERMAKE) StdAfx.h.gch $(GENERATED_FILES) depend.mak

depend: Funccode_enum.h githash
	$(CXX) -E -MM -w $(DEFINES) $(CXXFLAGS) *.cpp > depend.mak

.SUFFIXES: .cpp .o .rc
.PHONY: all clean depend

-include depend.mak
