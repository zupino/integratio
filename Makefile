CC := g++ # This is the main compiler
# CC := clang --analyze # and comment out the linker last line for sanity
# Cross Compile for Raspberry
# CC := arm-linux-gnueabihf-g++
SRCDIR := src
BUILDDIR := build
TARGET := bin/integratio
TEST := tests

SRCEXT := cpp
SOURCES := $(shell find $(SRCDIR) -type f -name *.$(SRCEXT))
OBJECTS := $(patsubst $(SRCDIR)/%,$(BUILDDIR)/%,$(SOURCES:.$(SRCEXT)=.o))
CFLAGS := -g -DBOOST_LOG_DYN_LINK -std=c++11 # -Wall
LIB := -lpthread -ltins -lboost_log -lboost_log_setup -lboost_filesystem -lboost_thread -lboost_date_time -lboost_system
# Added to use backtrace function, to get function stack names
# LINKOPT := -rdynamic
INC := -I include -I /usr/local/include

$(TARGET): $(OBJECTS)
	@echo " Linking...";
	@echo " $(CC) $(LINKOPT) $^ -o $(TARGET) $(LIB)"; $(CC) $(LINKOPT) $^ -o $(TARGET) $(LIB)
	@echo " Setting capabilities for integratio.."; sudo setcap CAP_NET_RAW=+eip $(TARGET)
	@echo " Setting capabilities for GDB.."; sudo setcap CAP_NET_RAW=+eip /usr/bin/gdb
	@echo " Setting capabilities for Shell.."; sudo setcap CAP_NET_RAW=+eip /bin/bash


$(BUILDDIR)/%.o: $(SRCDIR)/%.$(SRCEXT)
	@mkdir -p $(BUILDDIR)
	@echo " $(CC) $(CFLAGS) $(INC) -c -o $@ $<"; $(CC) $(CFLAGS) $(INC) -c -o $@ $<

clean:
	@echo " Cleaning...";
	@echo " $(RM) -r $(BUILDDIR) $(TARGET)"; $(RM) -r $(BUILDDIR) $(TARGET)
	@echo " $(RM) -r .cache"; $(RM) -r .cache
	@echo " $(RM) -r $(TEST)/__pycache__"; $(RM) -r $(TEST)/__pycache__
	@echo " $(RM) -r $(TEST)/.cache"; $(RM) -r $(TEST)/.cache
	@echo " Cleaning capabilities for GDB.."; sudo setcap CAP_NET_RAW=-eip /usr/bin/gdb
	@echo " Cleaning capabilities for Shell.."; sudo setcap CAP_NET_RAW=-eip /bin/bash

.PHONY: clean

all: $(TARGET)