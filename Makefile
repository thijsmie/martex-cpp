ifndef WASM
#Compiler and Linker
CC          := g++ -std=c++11

#The Target Binary Program
TARGET      := martex.so

#The Directories, Source, Objects and Binary
SRCDIR      := src
SRCSPEC     := php
BUILDDIR    := obj
TARGETDIR   := bin
SRCEXT      := cpp
DEPEXT      := d
OBJEXT      := o

#Flags, Libraries and Includes
CFLAGS      := -fopenmp -Wall -Wextra -O3 -g -fpic
LFLAGS		:= -shared
LIB         := -fopenmp -lphpcpp
INC         := -I/usr/local/include -I./src/

INI_DIR				=	/etc/php/7.0/mods-available
EXTENSION_DIR		=	$(shell php-config --extension-dir)

else
# WEBASSEMBLY
#Compiler and Linker
CC          := em++ -std=c++11

#The Target Binary Program
TARGET      := martex.js

#The Directories, Source, Objects and Binary
SRCDIR      := src
SRCSPEC     := wasm
BUILDDIR    := obj
TARGETDIR   := res
SRCEXT      := cpp
DEPEXT      := d
OBJEXT      := o

#Flags, Libraries and Includes
CFLAGS      := -fopenmp -Wall -Wextra -O3 -g -fpic
LFLAGS		:= --bind -s WASM=1 -s ALLOW_MEMORY_GROWTH=1 -s DISABLE_EXCEPTION_CATCHING=0
LIB         := -fopenmp
INC         := -I/usr/local/include -I./src/

INI_DIR				=	res
EXTENSION_DIR		=	res
endif

#---------------------------------------------------------------------------------
#DO NOT EDIT BELOW THIS LINE
#---------------------------------------------------------------------------------
SOURCES     := $(shell find $(SRCDIR)/core -type f -name *.$(SRCEXT)) $(shell find $(SRCDIR)/$(SRCSPEC) -type f -name *.$(SRCEXT))
OBJECTS     := $(patsubst $(SRCDIR)/%,$(BUILDDIR)/%,$(SOURCES:.$(SRCEXT)=.$(OBJEXT)))

#Defauilt Make
all: directories $(TARGET)

#Remake
remake: cleaner all

#Make the Directories
directories:
	@mkdir -p $(TARGETDIR)
	@mkdir -p $(BUILDDIR)

#Clean only Objects
clean:
	@$(RM) -rf $(BUILDDIR)

#Full Clean, Objects and Binaries
cleaner: clean
	@$(RM) -rf $(TARGETDIR)

#Pull in dependency info for *existing* .o files
-include $(OBJECTS:.$(OBJEXT)=.$(DEPEXT))

#Link
$(TARGET): $(OBJECTS)
	$(CC) $(LFLAGS) -o $(TARGETDIR)/$(TARGET) $^ $(LIB)

#Compile
$(BUILDDIR)/%.$(OBJEXT): $(SRCDIR)/%.$(SRCEXT)
	@mkdir -p $(dir $@)
	$(CC) $(CFLAGS) $(INC) -c -o $@ $<
	@$(CC) $(CFLAGS) $(INC) -MM $(SRCDIR)/$*.$(SRCEXT) > $(BUILDDIR)/$*.$(DEPEXT)
	@cp -f $(BUILDDIR)/$*.$(DEPEXT) $(BUILDDIR)/$*.$(DEPEXT).tmp
	@sed -e 's|.*:|$(BUILDDIR)/$*.$(OBJEXT):|' < $(BUILDDIR)/$*.$(DEPEXT).tmp > $(BUILDDIR)/$*.$(DEPEXT)
	@sed -e 's/.*://' -e 's/\\$$//' < $(BUILDDIR)/$*.$(DEPEXT).tmp | fmt -1 | sed -e 's/^ *//' -e 's/$$/:/' >> $(BUILDDIR)/$*.$(DEPEXT)
	@rm -f $(BUILDDIR)/$*.$(DEPEXT).tmp

install:		
	cp ${BIN}/${TARGET} ${EXTENSION_DIR}
	cp martex.ini ${INI_DIR}

#Non-File Targets
.PHONY: all remake clean cleaner install

