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
            shdr_t *shdr_t = malloc(sizeof(*shdr_t));
            assert(shdr_t);

            shdr_t->old = elf_shdr(elf, i);

            if (shdr_t->old->sh_type == SHT_PROGBITS || shdr_t->old->sh_type == SHT_NOBITS) {
                shdr_t->mod = shdr_cpy(shdr_t->old);
                vect_append(shdr_vect, shdr_t);
            }
            else {
                free(shdr_t);
            }
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
