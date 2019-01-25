#include <elf.h>

#include <assert.h>

#include "elflib.h"

Elf32_Ehdr *elf_ehdr(elf32_t *elf)
{
    return (Elf32_Ehdr *)elf->buf;
}
