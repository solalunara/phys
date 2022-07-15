#!/bin/bash

for class in "$@"
do
	class_capital=$(echo "$class" | tr 'a-z' 'A-Z')
	echo -e "#ifndef "$class_capital"_H\n#define "$class_capital"_H\n\n#pragma once\n\nstruct "$class"\n{\n\n};\n\n#endif" > header/"$class".h
	echo -e "#include \""$class".h\"\n" > source/"$class".cpp
done
