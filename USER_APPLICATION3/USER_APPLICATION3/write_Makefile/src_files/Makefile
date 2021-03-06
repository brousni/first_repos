#Makefile prepared by REC for Trackunit company

#list all subdirs of your project here
#hint: if atmel studio IDE is used then it can be found in <your-project-path>/Debug/Makefile 
SUBDIRS :=  \


#add all c files existing in project
#hint: if atmel studio IDE is used then it can be found in <your-project-path>/Debug/Makefile 
C_SRCS +=  \


#project name 
#Note: to work with Atmel studio target file must equal Atmel studio project name
PRO=jumpstart.elf

#obj files generation from c files list
PRO_OBJ=$(patsubst %.c, $(BUILD_DIR)/%.o, $(notdir $(C_SRCS)))

#dependencies files generation from *.o files
DEPS=$(PRO_OBJ:%.o=%.d)

#memory map file
MEM_MAP=$(PRO:%.elf=$(BUILD_DIR)/%.map)

#includes path generation from SUBDIRS
INCLUDES=$(addprefix -I, $(SUBDIRS))

#build directory
BUILD_DIR=obj


#toolchain
CC="C:/Program Files (x86)/Atmel/Atmel Toolchain/ARM GCC/Native/4.7.3.1029/arm-gnu-toolchain/bin/arm-none-eabi-gcc.exe"
BIN="C:/Program Files (x86)/Atmel/Atmel Toolchain/ARM GCC/Native/4.7.3.1029/arm-gnu-toolchain/bin/arm-none-eabi-objcopy.exe"

#compiler options
CFLAGS_OPT=-O0
#hint: -D__SAM4SD32C__ & -DBOARD=SAM4S_EK2 has to be set to proper CPU & BOARD !!
CFLAGS_DEF=-mthumb -D__SAM4E16E__ -DARM_MATH_CM4=true -DBOARD=USER_BOARD -Dprintf=iprintf
CFLAGS_W=-Wall -Wstrict-prototypes -Wmissing-prototypes -Werror-implicit-function-declaration -Wpointer-arith -std=gnu99 -ffunction-sections -fdata-sections -Wchar-subscripts -Wcomment -Wformat=2 -Wimplicit-int -Wmain -Wparentheses -Wsequence-point -Wreturn-type -Wswitch -Wtrigraphs -Wunused -Wuninitialized -Wunknown-pragmas -Wfloat-equal -Wundef -Wshadow -Wbad-function-cast -Wwrite-strings -Wsign-compare -Waggregate-return  -Wmissing-declarations -Wformat -Wmissing-format-attribute -Wno-deprecated-declarations -Wpacked -Wredundant-decls -Wnested-externs -Wlong-long -Wunreachable-code -Wcast-align
CFLAGS_MORE=-fdata-sections -ffunction-sections -mlong-calls -g3 -mcpu=cortex-m4 -c -pipe -fno-strict-aliasing  --param max-inline-insns-single=500
CFLAGS_DEP=-MMD -MP
CFLAGS=$(CFLAGS_DEF) $(CFLAGS_OPT) $(CFLAGS_W) $(CFLAGS_MORE) $(CFLAGS_DEP)

#linker script (use default generated by Atmel studio)
LSCRIPT=../src/ASF/sam/utils/linker_scripts/sam4e/sam4e8e/gcc/flash.ld
LFLAGS=-mthumb -Wl,-Map=$(MEM_MAP) -Wl,--start-group -larm_cortexM4lf_math -lm  -Wl,--end-group -L"../src/ASF/thirdparty/CMSIS/Lib/GCC"  -Wl,--gc-sections -mcpu=cortex-m4 -Wl,--entry=Reset_Handler -Wl,--cref -mthumb -T$(LSCRIPT)

#specify a list of directories that make should search
VPATH := $(SUBDIRS)

# Default target
all: $(PRO)

# General rule to make objs
$(BUILD_DIR)/%.o: %.c
	@echo "-----------------------------------------------------"
	@echo ------ Building $@ ...
	$(CC) $(CFLAGS) $(INCLUDES) -c $< -o $@


# Include all dependency files to add depedency to all header files in use.
-include $(DEPS)

# Main program
# Note: to work with Atmel studio Makefile and target must exist in the same folder
$(PRO): $(PRO_OBJ)
	@echo "-----------------------------------------------------"
	@echo "------ Building main target file..."
	$(CC) $(LFLAGS) $(PRO_OBJ) -Wl,-rpath,$(BUILD_DIR) -o $@
	@echo "-----------------------------------------------------"
	@echo "------ Creating BIN file..."
	$(BIN) -O binary $@ $(@:%.elf=%.bin)

# clean
.PHONY: clean
clean:
	del $(BUILD_DIR)\*
	rmdir $(BUILD_DIR)
	mkdir obj
	del $(PRO)

# regenerate directories
.PHONY: renew
renew:

	mkdir obj

# used for test purpose
.PHONY: test
test:      
	echo $(PRO)
	echo $(PRO_OBJ)
	echo $(DEPS)
