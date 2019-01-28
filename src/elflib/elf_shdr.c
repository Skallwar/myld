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

vect_t *shdr_list(elf32_t *elf, vect_t *shdr_vect)
{
    assert(elf);
    assert(shdr_vect);

    Elf32_Ehdr *ehdr = elf_ehdr(elf);

    for (uint16_t i = 0; i < ehdr->e_shnum; ++i) {
        Elf32_Shdr *shdr = elf_shdr(elf, i);

        if (shdr->sh_type == SHT_PROGBITS || shdr->sh_type == SHT_NOBITS) {
            shdr_t *shdr_t = malloc(sizeof(*shdr_t));
            assert(shdr_t);

            shdr_t->shdr = shdr;

            shdr_t->data = malloc(shdr->sh_size);
            assert(shdr_t->data);
            memcpy(shdr_t->data, elf->buf + shdr->sh_offset, shdr->sh_size);

            vect_append(shdr_vect, shdr_t);
        }
    }

    return shdr_vect;
}

static Elf32_Shdr *shdr_cpy(Elf32_Shdr *shdr)
{
    assert(shdr);

    Elf32_Shdr *shdr_copy = malloc(sizeof(*shdr_copy));
    assert(shdr_copy);

    return memcpy(shdr_copy, shdr, sizeof(*shdr));
}
