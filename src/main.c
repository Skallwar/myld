#include "elflib/elflib.h"

int main()
{
    elf32_t *elf = elf_load("example1.o");
    /* elf_print(elf); */
    Elf32_Ehdr *ehdr = elf_ehdr(elf);

    vect_t *shdr_vect = vect_new();
    shdr_list(elf, shdr_vect);

    vect_t *phdr_vect = phdr_gen(shdr_vect);

    elf32_t *elf_out = elf_build("out", ehdr, phdr_vect);

    elf_save(elf_out);
}
