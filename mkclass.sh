#!/bin/bash

for class in "$@"
do
	class_capital=$(echo "$class" | tr 'a-z' 'A-Z')
	class_capital_first=$(echo "${class:0:1}" | tr 'a-z' 'A-Z')$(echo "${class:1}")
	echo -e "#ifndef "$class_capital"_H\n#define "$class_capital"_H\n\n#pragma once\n\nstruct "$class_capital_first"\n{\n\n};\n\n#endif" > header/"$class".h
	echo -e "#include \""$class".h\"\n" > source/"$class".cpp
done
