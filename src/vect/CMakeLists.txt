add_library(vect vect.c)

add_executable(vect.test
    vect_test.c
    vect.c
)

target_link_libraries(vect.test
    criterion
    -fsanitize=address
    -fno-omit-frame-pointer
)

target_compile_options(vect.test PRIVATE -g -fsanitize=address -fno-omit-frame-pointer -lcriterion)
