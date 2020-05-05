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


# Targets
PROGS = newsclient inmemserver diskserver

all: $(PROGS)

newsclient: connection.o MessageHandler.o newsclient.o


inmemserver:  connection.o InMemDatabase.o MessageHandler.o server.o FileDatabase.o
	g++ $(CXXFLAGS) $(CPPFLAGS) $(LDFLAGS) -DMEMORYDB -c newsserver.cc
	g++ $(CXXFLAGS) $(CPPFLAGS) $(LDFLAGS) -o inmemserver newsserver.o connection.o InMemDatabase.o MessageHandler.o server.o FileDatabase.o

diskserver: connection.o InMemDatabase.o MessageHandler.o server.o FileDatabase.o
	g++ $(CXXFLAGS) $(CPPFLAGS) $(LDFLAGS) -DFILEDB -c newsserver.cc
	g++ $(CXXFLAGS) $(CPPFLAGS) $(LDFLAGS) -o diskserver newsserver.o connection.o InMemDatabase.o MessageHandler.o server.o FileDatabase.o



install: inmemserver newsclient diskserver
	cp -fv inmemserver newsclient diskserver ../bin




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
