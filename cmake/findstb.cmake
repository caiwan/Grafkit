# Guick and dirty way to find stb lib
# caiwan/IR

set(STB_ROOT_DIR CACHE PATH "STB root directory")

find_path(STB_INCLUDE_DIR
	NAMES
		stb.h
	HINTS
		${STB_ROOT_DIR}/
)
