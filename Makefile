BIN := graphdb
SRCS := \
    src/graph.cpp \
    src/rdf.cpp \
    src/main.cpp
INCLUDES := \
	-Ithirdparty/serd

OBJDIR := build/objs
DEPDIR := build/deps

OBJS := $(patsubst %,$(OBJDIR)/%.o,$(basename $(SRCS)))
DEPS := $(patsubst %,$(DEPDIR)/%.d,$(basename $(SRCS)))

$(shell mkdir -p $(dir $(OBJS)) >/dev/null)
$(shell mkdir -p $(dir $(DEPS)) >/dev/null)

CC := gcc
CXX := g++
LD := g++

LIBS := thirdparty/serd/build/libserd-0.a
LDLIBS := $(LIBS)
FLAGS := -g -Wall -Wextra -pedantic
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

clean:
	rm -r build/ $(BIN)

build-deps: thirdparty/serd/build/libserd-0.a

thirdparty/serd/build/libserd-0.a:
	cd thirdparty/serd && ./waf configure --static && ./waf

get-deps:
	git submodule update --init --recursive
	curl -L "https://github.com/catchorg/Catch2/releases/download/v2.11.1/catch.hpp" -o thirdparty/catch.hpp

$(BIN): $(OBJS) $(LIBS)
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

-include $(DEPS)

.PHONY: get-deps build-deps build-serd clean all
