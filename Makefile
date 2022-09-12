# export LD_LIBRARY_PATH=

PHONY : all

TARGET_NAME ?= bin/t2t_monitor

CROSS_COMPILE ?=

AS	= $(CROSS_COMPILE)as
LD	= $(CROSS_COMPILE)ld
CC	= $(CROSS_COMPILE)gcc
CPP	= $(CC) -E
AR	= $(CROSS_COMPILE)ar
NM	= $(CROSS_COMPILE)nm
STRIP	= $(CROSS_COMPILE)strip
OBJCOPY = $(CROSS_COMPILE)objcopy
OBJDUMP = $(CROSS_COMPILE)objdump
RANLIB	= $(CROSS_COMPILE)RANLIB

CFLAGS =
CFLAGS +=  -Wall -Wstrict-prototypes -O2 -fomit-frame-pointer
CFLAGS += -I include

LDFLAGS = 
LDFLAGS += -fPIC -rdynamic -shared 

export AS LD CC CPP AR NM STRIP OBJCOPY OBJDUMP RANLIB CFLAGS LDFLAGS

TEST_CFLAGS ?= ${CFLAGS}
LINK_PATH := -L libs
LD_SLIBS := -lcore -lmodules_parson -lmodules_util
LD_DLIBS :=
LD_LIBS := ${LD_SLIBS} ${LD_DLIBS}

export TEST_CFLAGS LINK_PATH LD_LIBS LD_SLIBS LD_DLIBS

MAKEFILE_BUILD := scripts/Makefile.build
MAKEFILE_TOOLS_BUILD := scripts/Makefile.tools.build
export MAKEFILE_BUILD MAKEFILE_TOOLS_BUILD

tools_dirs := tools/
tools_dirs := ${patsubst %/,%,$(filter %/, $(tools_dirs))}
$(tools_dirs): FORCE
	@make -f ${MAKEFILE_TOOLS_BUILD}  obj=$@
	
tools: $(tools_dirs) FORCE


dirs := core/ modules/
dirs := ${patsubst %/,%,$(filter %/, $(dirs))}
PHONY += $(dirs)
$(dirs): FORCE
	@make -f ${MAKEFILE_BUILD}  obj=$@

objs := init/main.o

all: $(dirs) ${objs}
	@mkdir -p bin
	$(CC) ${CFLAGS} ${LINK_PATH} -o ${TARGET_NAME} -Wl,-Bstatic -Wl,--whole-archive ${objs} ${LD_LIBS} -Wl,--no-whole-archive  -Wl,-Bdynamic

	
clean:	FORCE
	@echo  ">>> clean target"
	@rm -f *.bak *.so *.a
	@rm -f ${TARGET_NAME}
	@${shell for dir in `find -maxdepth 3 -type d | grep -v git| grep -v include | grep -v \.si4project`;\
	do rm -f $${dir}/*.o $${dir}/*.bak $${dir}/*.so $${dir}/*.a $${dir}/*.dep;done}
	
distclean: clean
	@echo ">>> distclean target"
	@rm -fr bin/ libs/
	@${shell cd tools/ && rm -f `ls *.c | sed 's/\(.*\)\.\w/\1/g'`}

help: 
	@echo  'Cleaning targets:'
	@echo  '  clean		  - Remove most generated files but keep the config and'
	@echo  '                    enough build support to build external modules'
	@echo  '  mrproper	  - Remove all generated files + config + various backup files'
	@echo  '  distclean	  - mrproper + remove editor backup and patch files'
	@echo  ''
	@exit 0


PHONY += FORCE
FORCE:
