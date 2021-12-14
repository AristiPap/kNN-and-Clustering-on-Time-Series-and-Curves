# lsh make file

PROGRAM := cluster

CUR-OBJS += $(BUILD)/GenericClusterSolver.o $(BUILD)/CurveCluster.o $(BUILD)/CurveAssignmentStep.o $(BUILD)/KMeans_pp_Solver.o $(BUILD)/AssignmentStepRoutines.o

include $(SRC)/rules.inc 

$(PROGRAM): $(BIN)/$(PROGRAM)
	cp $(BIN)/$(PROGRAM) $(MAKE-DIR)

$(BIN)/$(PROGRAM): $(OBJS) $(CUR-OBJS) $(INCLUDE)
	$(CC) $(CFLAGS) $(OBJS) $(CUR-OBJS) -o $(BIN)/$(PROGRAM) $(LIBS)

.PHONY : clean
clean :
	$(RM) -f $(MAKE-DIR)/$(PROGRAM) $(BIN)/$(PROGRAM) $(CUR-OBJS)