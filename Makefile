#
# A Makefile that compiles all .c and .s files in "src" and "include" 
# subdirectories and places the output in a "obj" subdirectory
#

# If you move this project you can change the directory 
# to match your GBDK root directory (ex: GBDK_HOME = "C:/GBDK/"
GBDK_HOME = $(GBDKDIR)

LCC = $(GBDK_HOME)bin/lcc

# You can set flags for LCC here
# For example, you can uncomment the line below to turn on debug output
LCCFLAGS = -Wl-lhUGEDriver.lib

# You can set the name of the .gb ROM file here
PROJECTNAME    = CBTFX_DEMO

SRCDIR      = src
INCDIR      = include
SFXDIR      = include/sfx
OBJDIR      = obj
BINS	    = $(OBJDIR)/$(PROJECTNAME).gb
CSOURCES    = $(foreach dir,$(SRCDIR),$(notdir $(wildcard $(dir)/*.c))) $(foreach dir,$(INCDIR),$(notdir $(wildcard $(dir)/*.c))) $(foreach dir,$(SFXDIR),$(notdir $(wildcard $(dir)/*.c)))
OBJS       	= $(CSOURCES:%.c=$(OBJDIR)/%.o) $(ASMSOURCES:%.s=$(OBJDIR)/%.o)

all:	prepare $(BINS)

rebuild:
	make clean
	make default_sfx
	make

# Choose any of these 3 to check the different SFX banks

default_sfx:
	python3 hammer2cbt.py --fxammo 10 --fxnamelist filenames.txt hammered.sav 0 include/sfx/

nes_sfx:
	python3 hammer2cbt.py --fxammo 10 --fxnamelist filenames.txt nes_sfx.sav 0 include/sfx/

gbwordyl_sfx:
	python3 hammer2cbt.py --fxammo 10 --fxnamelist filenames.txt gbwordyl.sav 10 include/sfx/

make.bat: Makefile
	@echo "REM Automatically generated from Makefile" > make.bat
	@make -sn | sed y/\\//\\\\/ | grep -v make >> make.bat

# Compile .c files in "src/" to .o object files
$(OBJDIR)/%.o:	$(SRCDIR)/%.c
	$(LCC) $(LCCFLAGS) -c -o $@ $<

# Compile .c files in "include/" to .o object files
$(OBJDIR)/%.o:	$(INCDIR)/%.c
	$(LCC) $(LCCFLAGS) -c -o $@ $<

# Compile .c files in "include/sfx" to .o object files
$(OBJDIR)/%.o:	$(SFXDIR)/%.c
	$(LCC) $(LCCFLAGS) -c -o $@ $<

# If needed, compile .c files in "src/" to .s assembly files
# (not required if .c is compiled directly to .o)
$(OBJDIR)/%.s:	$(SRCDIR)/%.c
	$(LCC) $(LCCFLAGS) -S -o $@ $<

# Link the compiled object files into a .gb ROM file
$(BINS):	$(OBJS)
	$(LCC) -Wm-ys -Wm-yn$(PROJECTNAME) $(LCCFLAGS) -o $(BINS) $(OBJS)
	
prepare:
	mkdir -p $(OBJDIR)

clean:
	rm -f  $(OBJDIR)/*.*
