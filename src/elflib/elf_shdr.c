#include <stdlib.h>
#include <string.h>
#include <elf.h>

#include <assert.h>

#include "elflib.h"

static Elf32_Shdr *shdr_cpy(Elf32_Shdr *shdr);

Elf32_Shdr *elf_shdr(elf32_t *elf, uint16_t n)
{
    assert(elf);

    Elf32_Ehdr *ehdr = elf_ehdr(elf);

    assert(n < ehdr->e_shnum);

    return (Elf32_Shdr *)(elf->buf + ehdr->e_shoff + n * ehdr->e_shentsize);
}

vect_t *shdr_list(elf32_t *elf)
{
    static vect_t *shdr_vect = NULL;

    if (elf) {
        if (!shdr_vect) {
            shdr_vect = vect_new();
        }

        Elf32_Ehdr *ehdr = elf_ehdr(elf);

        for (uint16_t i = 0; i < ehdr->e_shnum; ++i) {
            Elf32_Shdr *shdr = elf_shdr(elf, i);

            if (shdr->sh_type == SHT_PROGBITS || shdr->sh_type == SHT_NOBITS) {
                shdr = shdr_cpy(shdr);
                vect_append(shdr_vect, shdr);
            }
        }

    }

    return shdr_vect;
}

static Elf32_Shdr *shdr_cpy(Elf32_Shdr *shdr)
{
    Elf32_Shdr *shdr_cpy = malloc(sizeof(*shdr_cpy));

    return memcpy(shdr_cpy, shdr, sizeof(*shdr));
}
