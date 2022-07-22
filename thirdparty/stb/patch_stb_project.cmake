cmake_minimum_required(VERSION 3.13)
Project(stb)

# Probably we don't really need thins thing either, however there's an stb_vorbis that we need to do something about 

file(GLOB STB_SOURCE_FILES *.cpp *.c *.inc)
file(GLOB STB_HEADER_FILES *.h *.hpp *.inl)

#No test build, no need for external data, etc.
add_library(stb STATIC ${STB_SOURCE_FILES} ${STB_HEADER_FILES})

install(TARGETS stb DESTINATION lib)
install(FILES ${STB_HEADER_FILES} DESTINATION include/stb)
