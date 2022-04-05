objDir := build
cflags := -x c++ -std=c++17 -Wall -Werror -Wextra -g
CC := clang++-9
incsLibs := ./clib/ ./mlib/
exename := compiler
# opti := -O2

files := $(patsubst .%,%,$(patsubst %.cpp,%,$(shell find . -name '*.cpp')))
headerfiles := $(shell find . -name '*.hpp')
objs := $(addprefix $(objDir),$(addsuffix .o, $(files)))
objsDir := $(dir $(objs))
allGenFiles := $(objDir) $(exename) 
incs := $(addprefix -I,$(incsLibs))

$(exename) : $(objs)
	$(CC) $(objs) -o $(exename) 
	
$(objDir)/%.o : %.cpp $(headerfiles)
	$(CC) -c $(incs) $(cflags) $(opti) $< -o $@

$(objs): | $(objsDir)

$(objsDir) : 
	mkdir -p $(objsDir)

.PHONY : clean gdb run
clean :
	find . -name '*.log' | xargs rm -rf
	rm -rf $(allGenFiles) $(objsDir)

run : $(exename) 
	./$(exename) $(args)

gdb : $(exename) 
	gdb -ex="catch throw" --args ./$(exename) $(args)

