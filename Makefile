# Project-specific settings
PROJECT := Project3

# Identify compiler to use
CXX := c++

# Flags to ALWAYs use
CFLAGS_all := -Wall -Wextra -std=c++20

# Flags based on compilation type.
#   Default flags turn on optimizations
#   Use "make debug" to turn on debugger flag
#   Use "make grumpy" to get extra warnings during compilation
CFLAGS := -O3 -DNDEBUG $(CFLAGS_all)
CFLAGS_debug := -g $(CFLAGS_all)
CFLAGS_grumpy := -pedantic -Wconversion -Weffc++ $(CFLAGS_all)

default: $(PROJECT)
all: $(PROJECT)

debug:	CFLAGS := $(CFLAGS_debug)
debug:	$(PROJECT)

grumpy:	CFLAGS := $(CFLAGS_grumpy)
grumpy:	$(PROJECT)

tests: $(PROJECT)
	@echo "Running tests..."
	@cd tests && ./run_tests.sh
	@echo "Tests completed."

# Always run the tests, even if nothing has changed
.PHONY: tests serve

# List header files here that should trigger full recompilation when they change.
KEY_FILES := util.hpp
# List source files here
SOURCE := $(PROJECT).o ASTNode.o Error.o State.o Type.o Value.o WAT.o

$(PROJECT):	$(SOURCE) $(KEY_FILES)
	$(CXX) $(CFLAGS) -o $(PROJECT) $(SOURCE)

%.o: %.cpp
	$(CXX) -c $(CFLAGS) -o $@ $<

serve: tests
	cd tests && python -m http.server

clean:
	rm -f $(PROJECT) *.o tests/current/output-*.txt tests/*.wat tests/*.wasm

# Debugging information
print-%: ; @echo '$(subst ','\'',$*=$($*))'
