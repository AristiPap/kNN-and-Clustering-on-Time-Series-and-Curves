# link CUnit library
LIBS += -lcunit

CUR-OBJS := $(BUILD)/tests_main.o \
			$(BUILD)/GenericClusterSolver.o \
			$(BUILD)/CurveCluster.o \
			$(BUILD)/CurveAssignmentStep.o \
			$(BUILD)/KMeans_pp_Solver.o \
			$(BUILD)/AssignmentStepRoutines.o\

PROGRAM := run_tests

include $(SRC)/rules.inc 

$(PROGRAM): $(BIN)/$(PROGRAM)
	echo $(OBJS)
	cp $(BIN)/$(PROGRAM) $(MAKE-DIR)

$(BIN)/$(PROGRAM): $(OBJS) $(CUR-OBJS) $(INCLUDE)
	$(CC) $(CFLAGS) $(OBJS) $(CUR-OBJS) -o $(BIN)/$(PROGRAM) $(LIBS)

.PHONY : clean
clean :
	$(RM) -f $(MAKE-DIR)/$(PROGRAM) $(BIN)/$(PROGRAM) $(CUR-OBJS)


$(BUILD)/%.o : $(TESTS)/%.cpp
	@$(CC) $(CFLAGS) -c $< -o $@