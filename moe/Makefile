objDir := build
cflags := -x c++ -std=c++17 -Wall -Werror -Wextra -Wno-error=unused-parameter -g
lflags := -lpthread
CC := clang++
incsLibs := clib/ mlib/ third_party/antlr4/ ./ 
exename := compiler 
testPrerequisiteTablePath := testFileTable.txt 
testPrerequisite := $(shell cat $(testPrerequisiteTablePath))
# opti := -O2

uniq = $(if $1,$(firstword $1) $(call uniq,$(filter-out $(firstword $1),$1)))
files := $(patsubst .%,%,$(patsubst %.cpp,%,$(shell find . -name '*.cpp')))
headerfiles := $(shell find . -name '*.h') $(shell find . -name '*.hpp')
objs := $(addprefix $(objDir),$(addsuffix .o, $(files)))
objsDir := $(call uniq,$(dir $(objs)))
allGenFiles := $(objDir) $(exename)
incs := $(addprefix -I,$(incsLibs))
macroDef := $(addprefix -D,$(macros))

$(exename) : $(objs)
	$(info $(cflags))
	$(CC) $(lflags) $(objs) -o $(exename) 
	
$(objDir)/%.o : %.cpp $(headerfiles)
	$(CC) -c $(incs) $(cflags) $(opti) $(macroDef) $< -o $@

$(objs): | $(objsDir)

$(objsDir) : 
	mkdir -p $(objsDir)

.PHONY : clean gdb run see test
clean :
	find . -name '*.log' | xargs rm -rf
	rm -rf $(allGenFiles) $(objsDir)

run : $(exename) 
	./$(exename) $(args)

gdb : $(exename) 
	gdb -ex="catch throw" --args ./$(exename) $(args)

test : $(testPrerequisite) 
	mkdir -p build
	$(CC) -c $(incs) $(cflags) $(opti) -D TESTING $(macroDef) main.cpp -o build/main.o
	$(CC) $(lflags) $(testPrerequisite) build/main.o -o $(exename) 
	./$(exename) $(args)

see : 
	$(info $(files))
	$(info $(objs))
	$(info $(objsDir))
