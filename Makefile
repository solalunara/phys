libs = -lglfw -lpng -lwebp -lfreetype -lm -lstdc++
root = /home/fvp/prog/phys
includes = -I/usr/include/freetype2 -I$(root)/src
args = $(libs) $(includes)
obj = $(root)/obj
src = $(shell find $(root) -name '*.cpp' -o -name '*.c')
hdrs = $(shell find $(root) -name '*.h' -o -name '*.hpp')
obj_files = $(patsubst $(root)/%.cpp,$(obj)/%.o,$(patsubst $(root)/%.c,$(obj)/%.o,$(src)))

all: $(obj_files)
	$(CC) -fsanitize=address -g -o main $(obj_files) $(args)

$(obj)/%.o: %.cpp $(hdrs)
	mkdir -p $(dir $@)
	$(CC) -fsanitize=address -g -c $< -o $@ $(args)

$(obj)/%.o: %.c
	mkdir -p $(dir $@)
	$(CC) -fsanitize=address -g -c $< -o $@ $(args)

$(obj):
	mkdir -p $(obj)

clean: 
	rm -rf $(obj)
	rm -f main