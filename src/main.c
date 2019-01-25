#include "elflib/elflib.h"

int main()
{
    elf32_t *elf = elf_load("example1.o");
    elf_print(elf);
}
