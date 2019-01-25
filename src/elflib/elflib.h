#ifndef ELFLIB_H
#define ELFLIB_H

#include <stddef.h>
#include <elf.h>

#include "../vect/vect.h"

typedef struct {
    uint8_t *buf;
    size_t size;
} elf32_t;

struct phdr_info {
    Elf32_Phdr *phdr;
    vect_t *shdr_vect;
};

elf32_t *elf_load(char *path);

/* Acces */
Elf32_Ehdr *elf_ehdr(elf32_t *elf);
Elf32_Shdr *elf_shdr(elf32_t *elf, uint16_t n);
Elf32_Phdr *elf_phdr(elf32_t *elf, uint16_t n);

/* Manage */
vect_t *shdr_list(elf32_t *elf);
vect_t *phdr_gen(void);

void elf_print(elf32_t *elf);

#endif

