# lsh make file

PROGRAM := cluster

CUR-OBJS += $(BUILD)/CurveCluster.o

include $(SRC)/rules.inc 

$(PROGRAM): $(BIN)/$(PROGRAM)
	cp $(BIN)/$(PROGRAM) $(MAKE-DIR)

$(BIN)/$(PROGRAM): $(OBJS) $(CUR-OBJS) $(INCLUDE)
	$(CC) $(CFLAGS) $(OBJS) $(CUR-OBJS) -o $(BIN)/$(PROGRAM) $(LIBS)

.PHONY : clean
clean :
	$(RM) -f $(MAKE-DIR)/$(PROGRAM) $(BIN)/$(PROGRAM) $(CUR-OBJS)