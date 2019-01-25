#include <criterion/criterion.h>

#include "vect.h"

Test(vect, new) {
    vect_t *vect = vect_new();

    /* No null return value */
    cr_assert(vect);
    /* One free element */
    cr_assert(vect->capacity == 1);
    cr_assert(vect->size == 0);

    vect_free(vect);
}

Test(vect, get) {
    vect_t *vect = vect_new();

    int e = 1;

    vect_push(vect, &e);

    int g = *(int *)vect_get(vect, 0);
    /* Check stored value */
    cr_assert(g == e);

    vect_free(vect);
}

Test(vect, push) {
    vect_t *vect = vect_new();

    int e = 1;
    int e2 = 2;

    vect_push(vect, &e);
    /* Check stored value */
    cr_assert(*(int *)vect->data[0] == e);
    /* Check vect size and capacity */
    cr_assert(vect->size == 1 && vect->capacity == 1);

    vect_push(vect, &e2);
    /*Check stored value */
    cr_assert(*(int *)vect->data[0] == e2);
    /* Check displacement of old value */
    cr_assert(*(int *)vect->data[1] == e);
    /* Check vect size and capacity */
    cr_assert(vect->size == 2 && vect->capacity == 2);

    vect_free(vect);
}

Test(vect, pop) {
    vect_t *vect = vect_new();

    int e = 1;
    int e2 = 2;

    vect_push(vect, &e);
    vect_push(vect, &e2);

    int *p = vect_pop(vect);
    /* Check stored value */
    cr_assert(*p == e2);
    /* Check displacement of tail value */
    cr_assert(*(int *)vect->data[vect->size - 1] == e);
    /* Check vect size and capacity */
    cr_assert(vect->size == 1 && vect->capacity == 2);


    p = vect_pop(vect);
    /* Check stored value */
    cr_assert(*p == e);
    /* Check vect size and capacity */
    cr_assert(vect->size == 0 && vect->capacity == 2);

    vect_free(vect);
}

Test(vect, append) {
    vect_t *vect = vect_new();

    int e = 1;
    int e2 = 2;

    vect_append(vect, &e);
    /* Check stored value */
    cr_assert(*(int *)vect->data[0] == e);
    /* Check vect size and capacity */
    cr_assert(vect->size == 1 && vect->capacity == 1);

    vect_append(vect, &e2);
    /* Check stored value */
    cr_assert(*(int *)vect->data[1] == e2);
    /* Check displacement of old value */
    cr_assert(*(int *)vect->data[0] == e);
    /* Check vect size and capacity */
    cr_assert(vect->size == 2 && vect->capacity == 2);

    vect_free(vect);
}

Test(vect, strip) {
    vect_t *vect = vect_new();

    int e = 1;
    int e2 = 2;

    vect_append(vect, &e);
    vect_append(vect, &e2);

    int *p = vect_strip(vect);
    /* Check stored value */
    cr_assert(*p == e2);
    /* Check vect size and capacity */
    cr_assert(vect->size == 1 && vect->capacity == 2);


    p = vect_strip(vect);
    /* Check stored value */
    cr_assert(*p == e);
    /* Check vect size and capacity */
    cr_assert(vect->size == 0 && vect->capacity == 2);

    vect_free(vect);
}

Test(vect, insert) {
    vect_t *vect = vect_new();

    int e = 1;
    int e2 = 2;
    int e3 = 3;

    vect_insert(vect, 0, &e);
    /* Check stored value */
    cr_assert(*(int *)vect->data[0] == e);
    /* Check vect size and capacity */
    cr_assert(vect->size == 1 && vect->capacity == 1);

    vect_insert(vect, 1, &e3);
    /* Check stored value */
    cr_assert(*(int *)vect->data[1] == e3);
    /* Check displacement of old value */
    cr_assert(*(int *)vect->data[0] == e);
    /* Check vect size and capacity */
    cr_assert(vect->size == 2 && vect->capacity == 2);

    vect_insert(vect, 1, &e2);
    /* Check stored value */
    cr_assert(*(int *)vect->data[1] == e2);
    /* Check displacement of old value */
    cr_assert(*(int *)vect->data[0] == e);
    cr_assert(*(int *)vect->data[2] == e3);
    /* Check vect size and capacity */
    cr_assert(vect->size == 3 && vect->capacity == 4);

    vect_free(vect);
}

Test(vect, remove) {
    vect_t *vect = vect_new();

    int e = 1;
    int e2 = 2;
    int e3 = 3;

    vect_append(vect, &e);
    vect_append(vect, &e2);
    vect_append(vect, &e3);

    int *p = vect_remove(vect, 1);
    /* Check stored value */
    cr_assert(*p == e2);
    /* Check displacement of tail value */
    cr_assert(*(int *)vect->data[vect->size - 1] == e3);
    /* Check vect size and capacity */
    cr_assert(vect->size == 2 && vect->capacity == 4);

    p = vect_remove(vect, 1);
    /* Check stored value */
    cr_assert(*p == e3);
    /* Check displacement of tail value */
    cr_assert(*(int *)vect->data[vect->size - 1] == e);
    /* Check vect size and capacity */
    cr_assert(vect->size == 1 && vect->capacity == 4);

    p = vect_remove(vect, 0);
    /* Check stored value */
    cr_assert(*p == e);
    /* Check vect size and capacity */
    cr_assert(vect->size == 0 && vect->capacity == 4);

    vect_free(vect);
}
