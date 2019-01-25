#include <stdio.h>
#include <elf.h>

#include <assert.h>

#include "elflib.h"

static char *shdr_name(elf32_t *elf, Elf32_Shdr *shdr);
static char *shdr_type(Elf32_Shdr *shdr);
static char *phdr_type(Elf32_Phdr *phdr);

void elf_print(elf32_t *elf)
{
    assert(elf);

    Elf32_Ehdr *ehdr = elf_ehdr(elf);

    printf("ELF Magic: ");
    for (int i = 0; i < 16; ++i) {
        printf("%02x ", ehdr->e_ident[i]);
    }
    printf("\n");

    printf("\n");

    printf("ELF Sections: \n");
    for (uint16_t i = 0; i < ehdr->e_shnum; ++i) {
        Elf32_Shdr *shdr = elf_shdr(elf, i);

        char *name = shdr_name(elf, shdr);
        char *type = shdr_type(shdr);

        printf("[%i] %s %s\n", i, name, type);
    }

    printf("\n");

    printf("ELF Segments:\n");
    shdr_list(elf);
    vect_t *phdr_vect = phdr_gen();
    struct phdr_info *phdr_info = NULL;

    while((phdr_info = vect_pop(phdr_vect))) {
        char *type = phdr_type(phdr_info->phdr);

        printf("%s 0x%X\n", type, phdr_info->phdr->p_filesz);
    }
}

static char *shdr_name(elf32_t *elf, Elf32_Shdr *shdr)
{
    Elf32_Ehdr *ehdr = elf_ehdr(elf);
    Elf32_Shdr *name_section = elf_shdr(elf, ehdr->e_shstrndx);

    char *name_table = (char *)(elf->buf + name_section->sh_offset);

    return &name_table[shdr->sh_name];
}

static char *shdr_type(Elf32_Shdr *shdr) {
    switch (shdr->sh_type) {
        case SHT_NULL :
            return "NULL";
        case SHT_PROGBITS :
            return "PROGBITS";
        case SHT_SYMTAB :
            return "SYMTAB";
        case SHT_STRTAB :
            return "STRTAB";
        case SHT_RELA :
            return "RELA";
        case SHT_HASH :
            return "HASH";
        case SHT_DYNAMIC :
            return "DYNAMIC";
        case SHT_NOTE :
            return "NOTE";
        case SHT_NOBITS :
            return "NOBITS";
        case SHT_REL :
            return "REL";
        case SHT_SHLIB :
            return "SHLIB";
        case SHT_LOUSER :
            return "LOUSER";
        case SHT_HIUSER :
            return "HIUSER";
        default :
            return "Not implemented !";
    }

}

static char *phdr_type(Elf32_Phdr *phdr) {
    switch (phdr->p_type) {
        case PT_LOAD:
            return "LOAD";
        default :
            return "Not implemented !";
    }
}
