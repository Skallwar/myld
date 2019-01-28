#include <stdio.h>
#include <stdlib.h>
#include <elf.h>

#include <assert.h>

#include "elflib.h"

static uint32_t phdr_shdr_allign(phdr_t *phdr_t, shdr_t *shdr_t);
static phdr_t *phdr_new(void);
static void phdr_add_shdr(phdr_t *phdr_t, shdr_t *shdr_t);
static uint32_t phdr_flags(shdr_t *shdr_t);
static void phdr_allign(vect_t *phdr_vect);

Elf32_Phdr *elf_phdr(elf32_t *elf, uint16_t n)
{
    assert(elf);

    Elf32_Ehdr *ehdr = elf_ehdr(elf);

    assert(n < ehdr->e_phnum);

    return (Elf32_Phdr *)(elf->buf + ehdr->e_phoff + n * ehdr->e_phentsize);
}

vect_t *phdr_gen(vect_t *shdr_vect)
{
    assert(shdr_vect);

    vect_t *phdr_vect = vect_new();

    assert(shdr_vect);

    for (uint16_t i = 0; i < shdr_vect->size; ++i) {
        shdr_t *shdr_t = vect_get(shdr_vect, i);
        phdr_t *phdr_t = NULL;

        for (uint16_t j = 0; j < phdr_vect->size; ++j) {
            phdr_t = vect_get(phdr_vect, j);

            if (phdr_t->phdr->p_flags == phdr_flags(shdr_t)) {
                break;
            }

            phdr_t = NULL;
        }

        if (!phdr_t) {
            phdr_t = phdr_new();
            vect_append(phdr_vect, phdr_t);
        }

        phdr_add_shdr(phdr_t, shdr_t);
    }

    phdr_allign(phdr_vect);

    return phdr_vect;
}

static phdr_t *phdr_new(void)
{
    phdr_t *phdr_t = calloc(1, sizeof(*phdr_t));
    assert(phdr_t);

    phdr_t->phdr = calloc(1, sizeof(*phdr_t->phdr));
    assert(phdr_t->phdr);

    phdr_t->shdr_vect = vect_new();

    phdr_t->phdr->p_align = 0x1000;

    return phdr_t;
}

static void phdr_add_shdr(phdr_t *phdr_t, shdr_t *shdr_t)
{
    /* Add shdr to the vect of shdrs in this segment */
    vect_append(phdr_t->shdr_vect, shdr_t);

    Elf32_Shdr *shdr = shdr_t->shdr;

    /* Memory modification */
    uint32_t padding = phdr_shdr_allign(phdr_t, shdr_t);
    if (shdr->sh_type == SHT_PROGBITS) {
        phdr_t->phdr->p_filesz += shdr->sh_size + padding;
    }
    phdr_t->phdr->p_memsz += shdr->sh_size + padding;

    phdr_t->phdr->p_type = PT_LOAD;
    phdr_t->phdr->p_flags = phdr_flags(shdr_t);
}

static uint32_t phdr_flags(shdr_t *shdr_t)
{
    Elf32_Shdr *shdr = shdr_t->shdr;

    uint32_t shdr_flags = shdr->sh_flags;
    uint32_t phdr_flags = PF_R;

    if (shdr_flags & SHF_WRITE) {
        phdr_flags += PF_W;
    }

    if (shdr_flags & SHF_EXECINSTR) {
        phdr_flags += PF_X;
    }

    return phdr_flags;
}

/* Allign all phdr after generation is done */
static void phdr_allign(vect_t *phdr_vect)
{
    assert(phdr_vect);

    uint32_t padding = 0x1000;

    for (uint16_t i = 0; i < phdr_vect->size; ++i) {
        phdr_t *phdr_t = vect_get(phdr_vect, i);

        phdr_t->phdr->p_vaddr = padding;
        phdr_t->phdr->p_paddr = padding;
        phdr_t->phdr->p_offset = padding;

        padding += phdr_t->phdr->p_filesz;

        for (uint16_t j = 0; j < phdr_t->shdr_vect->size; ++j) {
            shdr_t *shdr_t = vect_get(phdr_t->shdr_vect, j);
            shdr_t->shdr->sh_addr += padding;
        }

        padding = padding >> 3*4;
        ++padding;
        padding = padding << 3*4;

    }
}

static uint32_t phdr_shdr_allign(phdr_t *phdr_t, shdr_t *shdr_t)
{
    assert(phdr_t);
    assert(shdr_t);

    Elf32_Shdr *shdr = shdr_t->shdr;

    if (shdr->sh_addralign == 0 || shdr->sh_addralign == 1) {
        return 0;
    }

    uint32_t start = phdr_t->phdr->p_filesz;
    uint32_t padding = 0;

    while ((start + padding++) % shdr->sh_addralign);
    --padding;

    shdr->sh_addr = start + padding;

    return padding;
}
