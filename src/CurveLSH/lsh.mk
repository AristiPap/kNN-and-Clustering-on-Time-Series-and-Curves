# lsh make file

PROGRAM := search

CFLAGS += -D NN

CUR-OBJS += $(BUILD)/curve_lsh.o 

include $(SRC)/rules.inc 

$(PROGRAM): $(BIN)/$(PROGRAM)
	cp $(BIN)/$(PROGRAM) $(MAKE-DIR)

$(BIN)/$(PROGRAM): $(OBJS) $(CUR-OBJS) $(INCLUDE)
	$(CC) $(CFLAGS) $(OBJS) $(CUR-OBJS) -o $(BIN)/$(PROGRAM) $(LIBS)

.PHONY : clean
clean :
	$(RM) -f $(MAKE-DIR)/$(PROGRAM) $(BIN)/$(PROGRAM) $(CUR-OBJS)

	