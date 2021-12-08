# lsh make file

PROGRAM := discrete-frechet-lsh

CFLAGS += -D NN

CUR-OBJS += $(BUILD)/LSHNearestNeighbours.o $(BUILD)/hypercube.o $(BUILD)/curve_lsh.o  $(BUILD)/DiscreteLSHSolver.o 

include $(SRC)/rules.inc 

$(PROGRAM): $(BIN)/$(PROGRAM)
	cp $(BIN)/$(PROGRAM) $(MAKE-DIR)

$(BIN)/$(PROGRAM): $(OBJS) $(CUR-OBJS) $(INCLUDE)
	$(CC) $(CFLAGS) $(OBJS) $(CUR-OBJS) -o $(BIN)/$(PROGRAM) $(LIBS)

.PHONY : clean
clean :
	$(RM) -f $(MAKE-DIR)/$(PROGRAM) $(BIN)/$(PROGRAM) $(CUR-OBJS)

	