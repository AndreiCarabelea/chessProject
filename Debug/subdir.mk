################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
CPP_SRCS += \
../cache.cpp \
../codificare.cpp \
../debug.cpp \
../functii_invatare.cpp \
../functii_tabla.cpp \
../retea.cpp \
../sah.cpp 

OBJS += \
./cache.o \
./codificare.o \
./debug.o \
./functii_invatare.o \
./functii_tabla.o \
./retea.o \
./sah.o 

CPP_DEPS += \
./cache.d \
./codificare.d \
./debug.d \
./functii_invatare.d \
./functii_tabla.d \
./retea.d \
./sah.d 


# Each subdirectory must supply rules for building sources it contributes
%.o: ../%.cpp
	@echo 'Building file: $<'
	@echo 'Invoking: GCC C++ Compiler'
	g++ -O0 -g3 -p -pg -Wall -c -fmessage-length=0 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@:%.o=%.d)" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


