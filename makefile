# Compiler and flags
GCC = gcc -g
CFLAGS = -Wall -lpthread -lrt
TARGETS = main file_generator csv_calculator file_mover inspector_type1 inspector_type2 inspector_type3

# Default rule: Build all targets
all: $(TARGETS)

# Build main executable
main: main.c shared_memories.h message_queues.h semphores.h constants.h
	$(GCC) $(CFLAGS) main.c -o main

# Build file generator
file_generator: file_generator.c shared_memories.h message_queues.h semphores.h constants.h
	$(GCC) $(CFLAGS) file_generator.c -o file_generator

# Build CSV calculator
csv_calculator: csv_calculator.c shared_memories.h message_queues.h semphores.h constants.h
	$(GCC) $(CFLAGS) csv_calculator.c -o csv_calculator

# Build file mover
file_mover: file_mover.c shared_memories.h message_queues.h semphores.h constants.h
	$(GCC) $(CFLAGS) file_mover.c -o file_mover

# Build inspectors
inspector_type1: inspector_type1.c shared_memories.h message_queues.h semphores.h constants.h
	$(GCC) $(CFLAGS) inspector_type1.c -o inspector_type1

inspector_type2: inspector_type2.c shared_memories.h message_queues.h semphores.h constants.h
	$(GCC) $(CFLAGS) inspector_type2.c -o inspector_type2

inspector_type3: inspector_type3.c shared_memories.h message_queues.h semphores.h constants.h
	$(GCC) $(CFLAGS) inspector_type3.c -o inspector_type3

# Clean all compiled files
clean:
	rm -f $(TARGETS)

# Run the main program
run:
	./main
