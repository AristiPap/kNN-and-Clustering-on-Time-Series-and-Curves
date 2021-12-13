$(BUILD)/%.o : $(SRC)/%.cpp
	@$(CC) $(CFLAGS) -c $< -o $@

$(BUILD)/%.o : $(SRC)/utils/%.cpp
	@$(CC) $(CFLAGS) -c $< -o $@

$(BUILD)/%.o : $(SRC)/LSH/%.cpp
	@$(CC) $(CFLAGS) -c $< -o $@

$(BUILD)/%.o : $(SRC)/HyperCube/%.cpp
	@$(CC) $(CFLAGS) -c $< -o $@	

$(BUILD)/%.o : $(SRC)/Clustering/%.cpp
	@$(CC) $(CFLAGS) -c $< -o $@	

$(BUILD)/%.o : $(SRC)/utils/Fred/%.cpp
	@$(CC) $(CFLAGS) -c $< -o $@	
	
$(BUILD)/%.o : $(SRC)/CurveLSH/%.cpp
	@$(CC) $(CFLAGS) -c $< -o $@
	
$(BUILD)/%.o : $(SRC)/CurveCluster/%.cpp
	@$(CC) $(CFLAGS) -c $< -o $@