CC = gcc

CCFLAGS = -Wall -Wextra -pedantic -ansi -O3
DATA_STRUCTURES_DIR = data_structures
MPT_DIR = mpt
BUILD_DIR = build
SRC_DIR = src

BIN = calc.exe
OBJ = $(BUILD_DIR)/calc.o $(BUILD_DIR)/operators.o $(BUILD_DIR)/shunting_yard.o $(BUILD_DIR)/conversion.o $(BUILD_DIR)/stack.o $(BUILD_DIR)/vector.o $(BUILD_DIR)/multiple_precision_operations.o $(BUILD_DIR)/multiple_precision_parsing.o $(BUILD_DIR)/multiple_precision_printing.o $(BUILD_DIR)/multiple_precision_type.o 

$(BUILD_DIR)/$(BIN): $(OBJ)
	$(CC) $(CCFLAGS) -o $(BIN) $(OBJ)

$(BUILD_DIR)/calc.o: $(SRC_DIR)/calc.c
	$(CC) $(CCFLAGS) -c $< -o $@

$(BUILD_DIR)/operators.o: $(SRC_DIR)/operators.c
	$(CC) $(CCFLAGS) -c $< -o $@

$(BUILD_DIR)/shunting_yard.o: $(SRC_DIR)/shunting_yard.c
	$(CC) $(CCFLAGS) -c $< -o $@

$(BUILD_DIR)/conversion.o: $(SRC_DIR)/$(DATA_STRUCTURES_DIR)/conversion.c
	$(CC) $(CCFLAGS) -c $< -o $@

$(BUILD_DIR)/stack.o: $(SRC_DIR)/$(DATA_STRUCTURES_DIR)/stack.c
	$(CC) $(CCFLAGS) -c $< -o $@

$(BUILD_DIR)/vector.o: $(SRC_DIR)/$(DATA_STRUCTURES_DIR)/vector.c
	$(CC) $(CCFLAGS) -c $< -o $@

$(BUILD_DIR)/multiple_precision_operations.o: $(SRC_DIR)/$(MPT_DIR)/multiple_precision_operations.c
	$(CC) $(CCFLAGS) -c $< -o $@

$(BUILD_DIR)/multiple_precision_parsing.o: $(SRC_DIR)/$(MPT_DIR)/multiple_precision_parsing.c
	$(CC) $(CCFLAGS) -c $< -o $@

$(BUILD_DIR)/multiple_precision_printing.o: $(SRC_DIR)/$(MPT_DIR)/multiple_precision_printing.c
	$(CC) $(CCFLAGS) -c $< -o $@

$(BUILD_DIR)/multiple_precision_type.o: $(SRC_DIR)/$(MPT_DIR)/multiple_precision_type.c
	$(CC) $(CCFLAGS) -c $< -o $@

$(BUILD_DIR):
	mkdir $@

clean:
	rm -rf $(BUILD_DIR)/* $(BIN)
