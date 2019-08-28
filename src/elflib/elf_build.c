#include <stdlib.h>
#include <string.h>

#include <assert.h>

#include "elflib.h"

static elf32_t *elf_new(const char *path, size_t size);
static void elf_build_ehdr(elf32_t *elf, Elf32_Ehdr *ehdr);
static void elf_build_data(elf32_t *elf, vect_t *phdr_vect);
static size_t elf_size(vect_t *phdr_vect);
static uint32_t elf_entry(elf32_t *elf);

elf32_t *elf_new(const char *path, size_t size)
{
    assert(path);
    assert(size);

    elf32_t *elf = malloc(sizeof(*elf));
    assert(elf);

    elf->path = path;
    elf->size = size;
    elf->buf = calloc(elf->size, sizeof(*elf->buf));
    assert(elf->buf);

    return elf;
}

void elf_free(elf32_t *elf)
{
    assert(elf);

    free(elf->buf);
    free(elf);
}

elf32_t *elf_build(const char *path, Elf32_Ehdr *ehdr, vect_t *phdr_vect)
{
    assert(path);
    assert(ehdr);
    assert(phdr_vect);

    size_t size = elf_size(phdr_vect);

    elf32_t *elf = elf_new(path, size);
    assert(elf);

    elf_build_ehdr(elf, ehdr);
    elf_build_data(elf, phdr_vect);

    return elf;
}

static void elf_build_ehdr(elf32_t *elf, Elf32_Ehdr *ehdr)
{
    assert(elf);
    assert(ehdr);

    Elf32_Ehdr *ehdr_new = elf_ehdr(elf);

    memcpy(ehdr_new, ehdr, ehdr->e_ehsize);

    ehdr_new->e_type = ET_EXEC;

    ehdr_new->e_entry = elf_entry(elf);

    ehdr_new->e_phoff = sizeof(Elf32_Ehdr);
    ehdr_new->e_shoff = 0;

    ehdr_new->e_phentsize = sizeof(Elf32_Phdr);
    ehdr_new->e_phnum = 0;

    ehdr_new->e_shentsize = 0;
    ehdr_new->e_shnum = 0;

    ehdr_new->e_shstrndx = 0;
}

static void elf_build_data(elf32_t *elf, vect_t *phdr_vect)
{
    assert(elf);
    assert(phdr_vect);

    Elf32_Ehdr *ehdr = elf_ehdr(elf);
    ehdr->e_phnum = phdr_vect->size;

    for (uint16_t i = 0; i < ehdr->e_phnum; ++i) {
        phdr_t *phdr_t = vect_get(phdr_vect, i);
        /* Place program header */
        size_t pos = ehdr->e_phoff + i * ehdr->e_phentsize;
        memcpy(elf->buf + pos, phdr_t->phdr, ehdr->e_phentsize);

        vect_t *shdr_vect = phdr_t->shdr_vect;
        for (uint16_t j = 0; j < shdr_vect->size; ++j) {
            shdr_t *shdr_t = vect_get(shdr_vect, j);
            memcpy(elf->buf + shdr_t->shdr->sh_addr, shdr_t->data, shdr_t->shdr->sh_size);
        }

    }
}

static uint32_t elf_entry(elf32_t *elf)
{
    return 0x1000;
}

static size_t elf_size(vect_t *phdr_vect)
{
    assert(phdr_vect);

    phdr_t *phdr_t = vect_get(phdr_vect, phdr_vect->size - 1);
    assert(phdr_t);

    return phdr_t->phdr->p_offset + 0x1000;
}
