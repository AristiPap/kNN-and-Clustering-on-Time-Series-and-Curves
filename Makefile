MAKE-DIR = $(PWD)

# disrectory flags
SRC = $(MAKE-DIR)/src
INCLUDE = $(MAKE-DIR)/include
BIN = $(MAKE-DIR)/bin
BUILD = $(MAKE-DIR)/build

# compiler flags
CC = g++
LIBS = -lm
CFLAGS = -std=c++11 -I $(INCLUDE) -O2

# objective files flags
OBJS := 

# debug switch message
DEBUG_MSG = OFF

VERBOSE_MSG = OFF
#  debug compilation set up
ifeq ($(debug), 1)
	CFLAGS += -Wall  -g3 -D DEBUG
	DEBUG_MSG = ON
endif

#  verbose run set up
ifeq ($(verbose), 1)
	CFLAGS += -D VERBOSE
	VERBOSE_MSG = ON
endif


# include for common objectives
include $(SRC)/src.inc 

# export variables to make it easier 
export MAKE-DIR CC CFLAGS LIBS OBJS SRC BUILD BIN

.PHONY : search
search :
	@echo VERBOSE: $(VERBOSE_MSG)
	@echo DEBUG: $(DEBUG_MSG)
	@$(MAKE) -C $(SRC)/CurveLSH -f lsh.mk 
	

.PHONY : clustering 
clustering:
	@echo VERBOSE: $(VERBOSE_MSG)
	@echo DEBUG: $(DEBUG_MSG)
	@$(MAKE) -C $(SRC)/Clustering -f clustering.mk 

PHONY : clean
clean :
	@$(MAKE) -C $(SRC)/CurveLSH -f lsh.mk clean
	@$(MAKE) -C $(SRC)/Clustering -f clustering.mk clean
	$(RM) -f $(OBJS)





