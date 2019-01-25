#include <stdlib.h>
#include <elf.h>

#include <assert.h>

#include "elflib.h"

static struct phdr_info *phdr_from_shdr(Elf32_Shdr *shdr);
static uint32_t phdr_set_flags(uint32_t shdr_flags);
static void phdr_add_shdr(struct phdr_info *phdr_info, Elf32_Shdr *shdr);
static uint32_t phdr_shdr_pading(struct phdr_info *phdr_info, Elf32_Shdr *shdr);

Elf32_Phdr *elf_phdr(elf32_t *elf, uint16_t n)
{
    assert(elf);

    Elf32_Ehdr *ehdr = elf_ehdr(elf);

    assert(n <= ehdr->e_phnum);

    return (Elf32_Phdr *)(elf->buf + ehdr->e_phoff + n * ehdr->e_phentsize);
}

vect_t *phdr_gen(void)
{
    static vect_t *phdr_vect = NULL;

    if(!phdr_vect) {
        phdr_vect = vect_new();
    }

    vect_t *shdr_vect = shdr_list(NULL);

    assert(shdr_vect);

    for (uint16_t i = 0; i < shdr_vect->size; ++i) {
        Elf32_Shdr *shdr = vect_get(shdr_vect, i);
        struct phdr_info *phdr_info = NULL;

        for (uint16_t j = 0; j < phdr_vect->size; ++j) {
            phdr_info = vect_get(phdr_vect, j);

            if (phdr_info->phdr->p_flags == phdr_set_flags(shdr->sh_flags)) {
                phdr_add_shdr(phdr_info, shdr);
                break;
            }

            phdr_info = NULL;
        }

        if (!phdr_info) {
            vect_append(phdr_vect, phdr_from_shdr(shdr));

        }
    }

    return phdr_vect;
}

static struct phdr_info *phdr_from_shdr(Elf32_Shdr *shdr)
{
    struct phdr_info *phdr_info = malloc(sizeof(*phdr_info));
    assert(phdr_info);

    phdr_info->phdr = malloc(sizeof(*phdr_info->phdr));
    assert(phdr_info->phdr);

    phdr_info->shdr_vect = vect_new();
    assert(phdr_info->shdr_vect);

    vect_append(phdr_info->shdr_vect, shdr);

    phdr_info->phdr->p_type = PT_LOAD;
    phdr_info->phdr->p_flags = phdr_set_flags(shdr->sh_flags);

    if(shdr->sh_type == SHT_PROGBITS) {
        phdr_info->phdr->p_filesz = shdr->sh_size;
    }

    phdr_info->phdr->p_memsz = shdr->sh_size;

    return phdr_info;
}

static uint32_t phdr_set_flags(uint32_t shdr_flags)
{
    uint32_t phdr_flags = PF_R;

    if(shdr_flags & SHF_WRITE) {
        phdr_flags += PF_W;
    }

    if (shdr_flags & SHF_EXECINSTR) {
        phdr_flags += PF_X;
    }

    return phdr_flags;
}

static void phdr_add_shdr(struct phdr_info *phdr_info, Elf32_Shdr *shdr)
{
    vect_append(phdr_info->shdr_vect, shdr);

    if(shdr->sh_type == SHT_PROGBITS) {
        phdr_info->phdr->p_filesz += shdr->sh_size;
    }

    phdr_info->phdr->p_memsz += shdr->sh_size;
}

static uint32_t phdr_shdr_pading(struct phdr_info *phdr_info, Elf32_Shdr *shdr)
{
    uint32_t padding = 0;

    while (phdr_info->phdr->p_filesz % ++padding);

    return padding;
}
