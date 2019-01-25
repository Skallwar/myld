#ifndef VECT_H
#define VECT_H

#include <stddef.h>

struct vect {
    size_t capacity, size;
    void **data;
};

typedef struct vect vect_t;

/* Initialize a new vect */
vect_t *vect_new(void);
/* Delete the vect */
void vect_free(vect_t *vect);

/* Retrive element at specified index */
void *vect_get(vect_t *vect, size_t index);

/* Push a new element at the begining of the vect */
void vect_push(vect_t *vect, void *data);
/* Add a new element at the end of the vect */
void vect_append(vect_t *vect, void *data);
/* Insert element at a specified index */
void vect_insert(vect_t *vect, size_t index, void *data);

/* Pop the first element of the vect */
void *vect_pop(vect_t *vect);
/* Pop the last element of the list */
void *vect_strip(vect_t *vect);
/* Remove element at specified index */
void *vect_remove(vect_t *vect, size_t index);

#endif
