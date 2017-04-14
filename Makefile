CC := g++ # This is the main compiler
# CC := clang --analyze # and comment out the linker last line for sanity
# Cross Compile for Raspberry
# CC := arm-linux-gnueabihf-g++
SRCDIR := src
BUILDDIR := build
TARGET := bin/integratio
 
SRCEXT := cpp
SOURCES := $(shell find $(SRCDIR) -type f -name *.$(SRCEXT))
OBJECTS := $(patsubst $(SRCDIR)/%,$(BUILDDIR)/%,$(SOURCES:.$(SRCEXT)=.o))
CFLAGS := -g -DBOOST_LOG_DYN_LINK -std=c++11 # -Wall
LIB := -lpthread -ltins -lboost_log -lboost_log_setup -lboost_filesystem -lboost_thread -lboost_date_time -lboost_system
# Added to use backtrace function, to get function stack names
# LINKOPT := -rdynamic
INC := -I include -I /usr/local/include

$(TARGET): $(OBJECTS)
	@echo " Linking..."
	@echo " $(CC) $(LINKOPT) $^ -o $(TARGET) $(LIB)"; $(CC) $(LINKOPT) $^ -o $(TARGET) $(LIB)

$(BUILDDIR)/%.o: $(SRCDIR)/%.$(SRCEXT)
	@mkdir -p $(BUILDDIR)
	@echo " $(CC) $(CFLAGS) $(INC) -c -o $@ $<"; $(CC) $(CFLAGS) $(INC) -c -o $@ $<

clean:
	@echo " Cleaning..."; 
	@echo " $(RM) -r $(BUILDDIR) $(TARGET)"; $(RM) -r $(BUILDDIR) $(TARGET)

.PHONY: clean