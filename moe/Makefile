files := $(patsubst .%,%,$(patsubst %.cpp,%,$(shell find . -name '*.cpp')))
objDir := build
objs := $(addprefix $(objDir),$(addsuffix .o, $(files)))
objsDir := $(dir $(objs))
allGenFiles := $(objDir) compiler 

cflags := -x c++ -std=c++17 -Wall -Werror -Wextra -g
CC := clang++-9
incs := -I./mlib/

compiler : $(objs)
	$(CC) $(objs) -o compiler
	
$(objDir)/%.o : %.cpp  
	$(CC) -c $(incs) $(cflags) $(opti) $< -o $@

$(objs): | $(objsDir)

-$(objsDir) : 
	mkdir -p $(objsDir)

.PHONY : clean gdb run
clean :
	rm -rf $(allGenFiles) $(objsDir)

run : compiler 
	./compiler $(args)

gdb : compiler
	gdb ./compiler

