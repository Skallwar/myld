cmake_minimum_required (VERSION 3.0)
project (MyLd)

add_compile_options(-g -Wall -Wextra)

add_subdirectory(src)

add_executable(myld
    src/main.c
)

target_link_libraries(myld
    elflib
    vect
    -fsanitize=address -fno-omit-frame-pointer  #ASAN
)
