#include <stdlib.h>
#include <stddef.h>
#include <string.h>
#include <assert.h>

#include "vect.h"

static void vect_double_capacity(vect_t *vect);

vect_t *vect_new()
{
    vect_t *vect = calloc(1, sizeof(*vect));
    assert(vect);

    vect->capacity = 1;
    vect->size = 0;
    vect->data = calloc(1, sizeof(*vect->data));
    assert(vect->data);

    return vect;
}

void vect_free(vect_t *vect)
{
    free(vect->data);
    free(vect);
}

static void vect_double_capacity(vect_t *vect)
{
    vect->capacity *= 2;

    vect->data = realloc(vect->data, vect->capacity * sizeof(*vect->data));
    assert(vect->data);
}

void *vect_get(vect_t *vect, size_t index)
{
    return vect->data[index];
}

void vect_push(vect_t *vect, void *data)
{
    if (vect->size + 1 > vect->capacity) {
        vect_double_capacity(vect);
    }

    memmove(vect->data + 1, vect->data, vect->size * sizeof(*vect->data));

    vect->data[0] = data;

    ++vect->size;
}

void vect_append(vect_t *vect, void *data)
{
    if (vect->size + 1 > vect->capacity) {
        vect_double_capacity(vect);
    }

    vect->data[vect->size] = data;

    ++vect->size;
}

void vect_insert(vect_t *vect, size_t index, void *data)
{
    if (vect->size + 1 > vect->capacity) {
        vect_double_capacity(vect);
    }


    memmove(vect->data + index + 1, vect->data + index,
            (vect->size - index) * sizeof(*vect->data));

    vect->data[index] = data;

    ++vect->size;
}

void *vect_pop(vect_t *vect)
{
    if (!vect->size) {
        return NULL;
    }

    void *data = vect->data[0];

    memmove(vect->data, vect->data + 1, (vect->size - 1) * sizeof(*vect->data));

    --vect->size;

    return data;
}

void *vect_strip(vect_t *vect)
{
    if (!vect->size) {
        return NULL;
    }

    return vect->data[--vect->size];
}

void *vect_remove(vect_t *vect, size_t index)
{
    if (!vect->size) {
        return NULL;
    }

    void *data = vect->data[index];

    memmove(vect->data + index, vect->data + index + 1,
            (vect->size - index - 1) * sizeof(*vect->data));

    --vect->size;

    return data;
}
