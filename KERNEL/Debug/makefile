################################################################################
# Automatically-generated file. Do not edit!
################################################################################

-include ../makefile.init

RM := rm -rf

# All of the sources participating in the build are defined here
-include sources.mk
-include src/Auxiliares/subdir.mk
-include src/subdir.mk
-include subdir.mk
-include objects.mk

ifneq ($(MAKECMDGOALS),clean)
ifneq ($(strip $(C_DEPS)),)
-include $(C_DEPS)
endif
endif

-include ../makefile.defs

# Add inputs and outputs from these tool invocations to the build variables 

# All Target
all: KERNEL

dependents:
	-cd /home/utnso/workspace/projects/tp-2019-1c-one-more-time/Shared_Library/Debug && $(MAKE) clean all

# Tool invocations
KERNEL: $(OBJS) $(USER_OBJS) /home/utnso/workspace/projects/tp-2019-1c-one-more-time/Shared_Library/Debug/libShared-Library.so
	@echo 'Building target: $@'
	@echo 'Invoking: GCC C Linker'
	gcc -L"/home/utnso/workspace/projects/tp-2019-1c-one-more-time/Shared_Library/Debug" -o "KERNEL" $(OBJS) $(USER_OBJS) $(LIBS)
	@echo 'Finished building target: $@'
	@echo ' '

# Other Targets
clean:
	-$(RM) $(EXECUTABLES)$(OBJS)$(C_DEPS) KERNEL
	-@echo ' '

.PHONY: all clean dependents
/home/utnso/workspace/projects/tp-2019-1c-one-more-time/Shared_Library/Debug/libShared-Library.so:

-include ../makefile.targets
