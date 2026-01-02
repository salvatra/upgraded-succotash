#ifndef QUERY4_H
#define QUERY4_H

#include <glib.h>
#include <stdio.h>
#include <core/dataset.h>

typedef struct q4_struct Q4Struct;

// Alterado: Recebe apenas Dataset
Q4Struct *init_Q4_structure(const Dataset *ds);
void destroy_Q4_structure(Q4Struct *q4);

// Alterado: Recebe Dataset em vez de hash table de passageiros
void query4(Q4Struct *q4_data, const Dataset *ds,
            const char *date_begin, const char *date_end,
            FILE *output, int isSpecial);

#endif