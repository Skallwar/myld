#include <stdlib.h>
#include <string.h>

#include <assert.h>

#include "elflib.h"

static elf32_t *elf_new(const char *path, vect_t *phdr_vect);
static void elf_copy(elf32_t *elf, Elf32_Ehdr *ehdr, vect_t *phdr_vect);
static size_t elf_size(vect_t *phdr_vect);
static uint32_t elf_entry(elf32_t *elf);

elf32_t *elf_build(const char *path, Elf32_Ehdr *ehdr, vect_t *phdr_vect)
{
    assert(path);
    assert(ehdr);
    assert(phdr_vect);

    elf32_t *elf = elf_new(path, phdr_vect);
    assert(elf);

    elf_copy(elf, ehdr, phdr_vect);

    return elf;
}

static elf32_t *elf_new(const char *path, vect_t *phdr_vect)
{
    assert(path);
    assert(phdr_vect);

    elf32_t *elf = malloc(sizeof(*elf));
    assert(elf);

    elf->path = path;
    elf->size = elf_size(phdr_vect);
    elf->buf = calloc(elf->size, sizeof(*elf->buf));
    assert(elf->buf);

    return elf;
}

static uint32_t elf_entry(elf32_t *elf)
{
    return 0x1000;
}

static void elf_copy(elf32_t *elf, Elf32_Ehdr *ehdr, vect_t *phdr_vect)
{
    assert(elf);
    assert(ehdr);
    assert(phdr_vect);

    memcpy(elf->buf, ehdr, sizeof(*ehdr));
    ehdr = elf_ehdr(elf);

    ehdr->e_type = ET_EXEC;
    ehdr->e_phoff = sizeof(*ehdr);
    ehdr->e_entry = elf_entry(elf);

    phdr_t *phdr_t = NULL;
    while ((phdr_t = vect_pop(phdr_vect))) {
        shdr_t *shdr_t = NULL;
        ++ehdr->e_phnum;
        memcpy(elf->buf + ehdr->e_phoff + ehdr->e_phnum * ehdr->e_phentsize, phdr_t->phdr, sizeof(*phdr_t->phdr));
        while ((shdr_t = vect_pop(phdr_t->shdr_vect))) {
            memcpy(elf->buf + shdr_t->mod->sh_offset,
                   shdr_t->elf->buf + shdr_t->old->sh_offset,
                   shdr_t->old->sh_size);
        }
    }
}

static size_t elf_size(vect_t *phdr_vect)
{
    assert(phdr_vect);

    phdr_t *phdr_t = vect_get(phdr_vect, phdr_vect->size - 1);
    assert(phdr_t);

    return phdr_t->phdr->p_offset + 0x1000;
}
