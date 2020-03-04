BIN := build/graphdb
TEST := build/graphdb-test

SRCS := \
    src/graph.cpp \
    src/rdf.cpp \
    src/dfa.cpp \
    src/nfa.cpp

BIN_SRCS := $(SRCS) src/main.cpp
TEST_SRCS := $(SRCS) \
    test/main.cpp \
    test/rdf.cpp \
    test/automaton.cpp

INCLUDES := \
	-Isrc \
	-Ithirdparty/serd \
	-Ithirdparty

OBJDIR := build/objs
DEPDIR := build/deps

BIN_OBJS := $(patsubst %,$(OBJDIR)/%.o,$(basename $(BIN_SRCS)))
BIN_DEPS := $(patsubst %,$(DEPDIR)/%.d,$(basename $(BIN_SRCS)))

TEST_OBJS := $(patsubst %,$(OBJDIR)/%.o,$(basename $(TEST_SRCS)))
TEST_DEPS := $(patsubst %,$(DEPDIR)/%.d,$(basename $(TEST_SRCS)))

$(shell mkdir -p $(dir $(BIN_OBJS)) >/dev/null)
$(shell mkdir -p $(dir $(BIN_DEPS)) >/dev/null)
$(shell mkdir -p $(dir $(TEST_OBJS)) >/dev/null)
$(shell mkdir -p $(dir $(TEST_DEPS)) >/dev/null)

CC := gcc
CXX := g++
LD := g++

LIBS := thirdparty/serd/build/libserd-0.a
LDLIBS := $(LIBS)
FLAGS := -g -Wall -Wextra -pedantic -Wno-sign-compare
CFLAGS := -std=c11 $(FLAGS) $(INCLUDES)
CXXFLAGS := -std=c++17 $(FLAGS) $(INCLUDES)
LDFLAGS :=
DEPFLAGS = -MT $@ -MD -MP -MF $(DEPDIR)/$*.Td

COMPILE.c = $(CC) $(DEPFLAGS) $(CFLAGS) $(CPPFLAGS) -c -o $@
COMPILE.cc = $(CXX) $(DEPFLAGS) $(CXXFLAGS) $(CPPFLAGS) -c -o $@
LINK.o = $(LD) $(LDFLAGS) $(LDLIBS) -o $@
PRECOMPILE =
POSTCOMPILE = mv -f $(DEPDIR)/$*.Td $(DEPDIR)/$*.d

all: build-deps $(BIN)

check: $(TEST)
	$(TEST)

check-vg: $(TEST)
	valgrind $(TEST)

clean:
	rm -rf build/

build-deps: thirdparty/serd/build/libserd-0.a

thirdparty/serd/build/libserd-0.a:
	cd thirdparty/serd && ./waf configure --static && ./waf

get-deps:
	mkdir -p thirdparty
	git submodule update --init --recursive
	curl -L "https://github.com/catchorg/Catch2/releases/download/v2.11.1/catch.hpp" -o thirdparty/catch.hpp

$(BIN): $(BIN_OBJS) $(LIBS)
	$(LINK.o) $^

$(TEST): $(TEST_OBJS) $(LIBS)
	$(LINK.o) $^

$(OBJDIR)/%.o: %.c
$(OBJDIR)/%.o: %.c $(DEPDIR)/%.d
	$(PRECOMPILE)
	$(COMPILE.c) $<
	$(POSTCOMPILE)

$(OBJDIR)/%.o: %.cpp
$(OBJDIR)/%.o: %.cpp $(DEPDIR)/%.d
	$(PRECOMPILE)
	$(COMPILE.cc) $<
	$(POSTCOMPILE)

$(OBJDIR)/%.o: %.cc
$(OBJDIR)/%.o: %.cc $(DEPDIR)/%.d
	$(PRECOMPILE)
	$(COMPILE.cc) $<
	$(POSTCOMPILE)

$(OBJDIR)/%.o: %.cxx
$(OBJDIR)/%.o: %.cxx $(DEPDIR)/%.d
	$(PRECOMPILE)
	$(COMPILE.cc) $<
	$(POSTCOMPILE)

.PRECIOUS: $(DEPDIR)/%.d
$(DEPDIR)/%.d: ;

-include $(BIN_DEPS) $(TEST_DEPS)

.PHONY: get-deps build-deps build-serd clean all check check-vg
