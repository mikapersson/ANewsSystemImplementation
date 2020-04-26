# Define the compiler and the linker. The linker must be defined since
# the implicit rule for linking uses CC as the linker. g++ can be
# changed to clang++.
CXX = g++
CC  = $(CXX)

# Generate dependencies in *.d files
DEPFLAGS = -MT $@ -MMD -MP -MF $*.d

# Define preprocessor, compiler, and linker flags. Uncomment the # lines
# if you use clang++ and wish to use libc++ instead of GNU's libstdc++.
# -g is for debugging.
CPPFLAGS =  -std=c++11 -I.
CXXFLAGS =  -O2 -Wall -Wextra -pedantic-errors -Wold-style-cast
CXXFLAGS += -std=c++11
CXXFLAGS += -g
CXXFLAGS += $(DEPFLAGS)
LDFLAGS =   -g
#CPPFLAGS += -stdlib=libc++
#CXXFLAGS += -stdlib=libc++
#LDFLAGS +=  -stdlib=libc++

# Targets
PROGS = newsclient inmemserver diskserver

all: $(PROGS)

inmemserver: inmemserver.o connection.o InMemDatabase.o MessageHandler.o server.o
newsclient: connection.o MessageHandler.o newsclient.o
diskserver: diskserver.o MessageHandler.o connection.o FileDatabase.o server.o

install: inmemserver newsclient diskserver
	cp -fv inmemserver newsclient diskserver ../bin


# test1: test1.o FileDatabase.o


clear:
	rm -rfv FileDatabase
	rm -rfv ../bin/FileDatabase

# Phony targets
.PHONY: all clean distclean

SRC = $(wildcard *.cc)

# Standard clean
clean:
	rm -f $(SRC:.cc=.o) $(PROGS)

distclean: clean
	-rm -f $(SRC:.cc=.d)




# Include the *.d files
-include $(SRC:.cc=.d)
