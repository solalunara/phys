libs = -lglfw -lpng -lwebp -lfreetype -lm -lstdc++
root = /home/fvp/prog/phys
includes = -I/usr/include/freetype2 -I$(root)/src
args = $(libs) $(includes)
obj = $(root)/obj
src = $(shell find $(root) -name '*.cpp' -o -name '*.c')
obj_files = $(patsubst $(root)/%.cpp,$(obj)/%.o,$(patsubst $(root)/%.c,$(obj)/%.o,$(src)))

all: $(obj_files)
	$(CC) -o main $(obj_files) $(args)

$(obj)/%.o: %.cpp
	mkdir -p $(dir $@)
	$(CC) -c $< -o $@ $(args)

$(obj)/%.o: %.c
	mkdir -p $(dir $@)
	$(CC) -c $< -o $@ $(args)

$(obj):
	mkdir -p $(obj)