#include <stdio.h>
#include <stdlib.h>
#include <elf.h>

#include <assert.h>

#include "elflib.h"

static phdr_t *phdr_new(uint32_t flags);
static void phdr_add_shdr(phdr_t *phdr_t, shdr_t *shdr_t);
static uint32_t shdr_align(uint32_t addr, uint32_t align);
static uint32_t phdr_flags(uint32_t flags);

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

    shdr_t *shdr_t;
    while ((shdr_t = vect_pop(shdr_vect))) {
        phdr_t *phdr_t = NULL;

        /* Compute flags for this section */
        uint32_t flags = phdr_flags(shdr_t->shdr->sh_flags);

        for (uint16_t i = 0; i < phdr_vect->size; ++i) {
            phdr_t = vect_get(phdr_vect, i);

            /* Select this phdr if flags are matching */
            if (phdr_t->phdr->p_flags == flags) {
                break;
            }

            phdr_t = NULL;
        }

        /* If no segment have been found create one */
        if (!phdr_t) {
            phdr_t = phdr_new(flags);
            vect_append(phdr_vect, phdr_t);
        }

        /* Add section to segment and add segment in the vect */
        phdr_add_shdr(phdr_t, shdr_t);
    }

    return phdr_vect;
}

static phdr_t *phdr_new(uint32_t flags)
{
    static uint32_t offset = 0x2000;

    phdr_t *phdr_t = calloc(1, sizeof(*phdr_t));
    assert(phdr_t);

    Elf32_Phdr *phdr = calloc(1, sizeof(*phdr_t->phdr));
    phdr_t->phdr = phdr;
    assert(phdr_t->phdr);

    phdr_t->shdr_vect = vect_new();

    /* If it's the executable segment it's need to be at 0x1000 */
    uint32_t align;
    if (flags & PF_R && flags & PF_X) {
        align = 0x1000;
    }
    else {
        align = offset;
        offset += 0x1000;
    }
    phdr->p_offset = align;
    phdr->p_vaddr = align;
    phdr->p_paddr = align;

    phdr->p_type = PT_LOAD;
    phdr->p_flags = flags;

    phdr->p_align = 0x1000;

    return phdr_t;
}

static void phdr_add_shdr(phdr_t *phdr_t, shdr_t *shdr_t)
{
    assert(phdr_t);
    assert(shdr_t);

    /* Add shdr the vect */
    vect_append(phdr_t->shdr_vect, shdr_t);

    Elf32_Phdr *phdr = phdr_t->phdr;
    Elf32_Shdr *shdr = shdr_t->shdr;

    /* Compute new shdr position in file and in memory (alignment included) */
    uint32_t align = shdr_align(phdr->p_memsz, shdr->sh_addralign);
    shdr->sh_offset = phdr->p_offset + phdr->p_filesz;
    shdr->sh_addr = phdr->p_vaddr + phdr->p_memsz + align;

    /* Compute new size for phdr */
    if (shdr->sh_type == SHT_PROGBITS) {
        phdr->p_filesz += shdr->sh_size + align;
    }
    phdr->p_memsz += shdr->sh_size + align;
}

static uint32_t shdr_align(uint32_t addr, uint32_t align)
{
    if (align == 0 || align == 1) {
        return 0;
    }

    uint32_t offset = 0;

    while ((addr + offset) % align) {
        ++offset;
    }

    return offset;
}

static uint32_t phdr_flags(uint32_t shdr_flags)
{
    uint32_t phdr_flags = PF_R;

    if (shdr_flags & SHF_WRITE) {
        phdr_flags += PF_W;
    }

    if (shdr_flags & SHF_EXECINSTR) {
        phdr_flags += PF_X;
    }

    return phdr_flags;
}
