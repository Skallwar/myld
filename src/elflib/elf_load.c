#include <stdlib.h>
#include <stdbool.h>
#include <stdio.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>

#include <assert.h>

#include "elflib.h"

static inline bool elf_isvalid(elf32_t *elf);

elf32_t *elf_load(char *path)
{
    assert(path);

    struct stat *statbuf = malloc(sizeof(*statbuf));
    assert(statbuf);

    if (stat(path, statbuf) == -1) {
        free(statbuf);
        return NULL;
    }

    size_t size = statbuf->st_size;

    free(statbuf);

    elf32_t *elf = malloc(sizeof(*elf));
    assert(elf);

    elf->buf = malloc(size);
    elf->size = size;
    assert(elf->buf);

    int file = open(path, O_RDONLY);
    read(file, elf->buf, elf->size);

    close(file);

    assert(elf_isvalid(elf));

    return elf;
}

static inline bool elf_isvalid(elf32_t *elf)
{
    return elf->buf[EI_MAG0] == ELFMAG0 &&
           elf->buf[EI_MAG1] == ELFMAG1 &&
           elf->buf[EI_MAG2] == ELFMAG2 &&
           elf->buf[EI_MAG3] == ELFMAG3;
}