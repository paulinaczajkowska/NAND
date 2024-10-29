/* AKSO - zadanie 1             */
/* Autor: Paulina Czajkowska    */

#include <stdlib.h>
#include <errno.h>
#include "nand.h"

typedef struct nand {
    // Wejście do bramki NAND
    ssize_t num_in;
    bool const** sig_in;
    struct nand** nand_in;

    // Wyjście z bramki NAND
    ssize_t num_out;
    struct nand** nand_out;

    bool is_on_stack;
    bool is_visited;

    bool sig_out;
    unsigned int max_path_len;

} nand_t;

nand_t* nand_new(unsigned n) {

    nand_t* g = (nand_t*)malloc(sizeof(nand_t));

    if (g == NULL) {
        free(g);
        errno = ENOMEM;
        return NULL;
    }

    g->num_in = n;
    if (n != 0) {
        g->sig_in = (bool const**)malloc(n * sizeof(bool const*));
        g->nand_in = (nand_t**)malloc(n * sizeof(nand_t*));
        if (g->sig_in == NULL || g->nand_in == NULL) {
            if (g->sig_in != NULL) {
                free(g->sig_in);
            }
            if (g->nand_in != NULL) {
                free(g->nand_in);
            }
            free(g);
            errno = ENOMEM;
            return NULL;
        }
    }
    else {
        g->sig_in = NULL;
        g->nand_in = NULL;
    }
    g->nand_out = NULL;

    for (ssize_t i = 0; i < n; ++i) {
        g->sig_in[i] = NULL;
        g->nand_in[i] = NULL;
    }

    g->is_on_stack = false;
    g->is_visited = false;
    g->sig_out = false;
    g->max_path_len = 0;
    g->num_out = 0;

    return g;
}

void nand_delete(nand_t *g) {

    if (g != NULL) {

        for (int i = 0; i < g->num_in; ++i) {
            nand_t* tmp = g->nand_in[i];
            if (tmp != NULL && tmp->num_out > 0) {
                for (int j = 0; j < tmp->num_out; ++j) {
                    while (tmp->nand_out[j] == g) {
                        tmp->nand_out[j] = tmp->nand_out[tmp->num_out - 1];
                        tmp->num_out--;
                        if (tmp->num_out == 0) {
                            break;
                        }
                    }
                }
                if (tmp->num_out <= 0) {
                    free(tmp->nand_out);
                    tmp->nand_out = NULL;
                }
                else{
                    tmp->nand_out = (nand_t**)realloc(tmp->nand_out, (tmp->num_out) * sizeof(nand_t*));
                }
            }
        }

        for (int i = 0; i < g->num_out; ++i) {
            nand_t* tmp = g->nand_out[i];
            for (int j = 0; j < tmp->num_in; ++j) {
                if (tmp->nand_in[j] == g) {
                    tmp->nand_in[j] = NULL;
                }
            }
        }

        if (g->sig_in != NULL) {
            free(g->sig_in);
        }
        if (g->nand_in != NULL) {
            free(g->nand_in);
        }
        if (g->nand_out != NULL) {
            free(g->nand_out);
        }
        free(g);
    }
}

int nand_connect_nand(nand_t *g_out, nand_t *g_in, unsigned k) {

    if (g_out == NULL || g_in == NULL || k >= g_in->num_in) {
        errno = EINVAL;
        return -1;
    }

    if (g_out->nand_out == NULL) {
        g_out->nand_out = (nand_t**)malloc(sizeof(nand_t*));
        if (g_out->nand_out == NULL) {
            errno = ENOMEM;
            return -1;
        }
    }
    else {
        nand_t** tmp = g_out->nand_out;
        tmp = (nand_t**)realloc(g_out->nand_out, (g_out->num_out + 1) * sizeof(nand_t*));
        if (tmp == NULL) {
            errno = ENOMEM;
            return -1;
        }
        g_out->nand_out = tmp;
    }

    g_out->num_out++;
    g_out->nand_out[g_out->num_out - 1] = g_in;

    // Jeśli jakaś bramka była podłączona do g_in to ją odłaczamy
    if (g_in->nand_in[k] != NULL) {
        nand_t* tmp = g_in->nand_in[k];
        if (tmp->num_out == 1) {
            free(tmp->nand_out);
            tmp->nand_out = NULL;
            tmp->num_out--;
        }
        else {
            for (int i = 0; i < tmp->num_out; ++i) {
                if (tmp->nand_out[i] == g_in) {
                    tmp->nand_out[i] = tmp->nand_out[tmp->num_out - 1];
                    tmp->nand_out = (nand_t**)realloc(tmp->nand_out, (tmp->num_out - 1) * sizeof(nand_t*));
                    tmp->num_out--;
                    break;
                }
            }
        }
    }

    g_in->sig_in[k] = NULL;
    g_out->nand_out[g_out->num_out - 1] = g_in;
    g_in->nand_in[k] = g_out;

    return 0;
}

int nand_connect_signal(bool const *s, nand_t *g, unsigned k) {

    if (g == NULL || s == NULL || k >= g->num_in) {
        errno = EINVAL;
        return -1;
    }

    // Jeśli jakaś bramka była podłączona do g to ją odłaczamy
    if (g->nand_in[k] != NULL) {
        nand_t* tmp = g->nand_in[k];
        if (tmp->num_out == 1) {
            free(tmp->nand_out);
            tmp->nand_out = NULL;
            tmp->num_out--;
        }
        else {
            for (int i = 0; i < tmp->num_out; ++i) {
                if (tmp->nand_out[i] == g) {
                    tmp->nand_out[i] = tmp->nand_out[tmp->num_out - 1];
                    tmp->nand_out = (nand_t**)realloc(tmp->nand_out, (tmp->num_out - 1) * sizeof(nand_t*));
                    tmp->num_out--;
                    break;
                }
            }
        }
    }

    g->sig_in[k] = s;
    g->nand_in[k] = NULL;
    return 0;
}

ssize_t evaluate_gate(nand_t* g, bool* s) {

    if (g->is_visited) {
        if (g->is_on_stack) {
            errno = ECANCELED;
            return -1;
        }
        *s = g->sig_out;
        return g->max_path_len;
    }

    g->is_on_stack = true;
    g->is_visited = true;

    for (ssize_t i = 0; i < g->num_in; ++i) {
        if (g->nand_in[i] == NULL && g->sig_in[i] == NULL) { // errno
            return -1;
        }
        bool out;
        if (g->nand_in[i] != NULL) {
            ssize_t len;
            len = evaluate_gate(g->nand_in[i], &out) + 1;
            if (len == 0) {
                return -1;
            }
            g->max_path_len = (g->max_path_len > len) ? g->max_path_len : len;
            if (!out) {
                g->sig_out = true;
            }
        }
        else {
            g->max_path_len = (g->max_path_len > 1) ? g->max_path_len : 1;
            if (!(*(g->sig_in[i]))) {
                g->sig_out = true;
            }
        }
    }

    *s = g->sig_out;
    g->is_on_stack = false;
    return g->max_path_len;
}

void clean_gate(nand_t* g) {

    g->is_on_stack = false;
    g->is_visited = false;
    g->sig_out = false;
    g->max_path_len = 0;

    for (ssize_t i = 0; i < g->num_in; ++i) {
        if (g->nand_in[i] != NULL) {
            if ((g->nand_in[i])->is_visited) {
                clean_gate(g->nand_in[i]);
            }
        }
    }
}

void clean_gates(nand_t** g, size_t m) {

    for (size_t i = 0; i < m; ++i) {
        clean_gate(g[i]);
    }
}

ssize_t nand_evaluate(nand_t **g, bool *s, size_t m) {

    if (g == NULL || s == NULL || m == 0) { // m <= 0
        errno = EINVAL;
        return -1;
    }

    for (size_t i = 0; i < m; ++i) {
        if (g[i] == NULL) {
            errno = EINVAL;
            return -1;
        }
    }

    ssize_t max_len = 0;
    ssize_t len;
    for (size_t i = 0; i < m; ++i) {
        len = evaluate_gate(g[i], s + i);
        if (len == -1) {
            errno = ECANCELED;
            clean_gates(g, m);
            return -1;
        }
        max_len = (max_len > len) ? max_len : len;
    }

    clean_gates(g, m);
    return max_len;
}

ssize_t nand_fan_out(nand_t const *g) {

    if (g == NULL) {
        errno = EINVAL;
        return -1;
    }

    return g->num_out;
}

void* nand_input(nand_t const *g, unsigned k) {

    if (g == NULL || k >= g->num_in) {
        errno = EINVAL;
        return NULL;
    }

    if (g->nand_in[k] == NULL && g->sig_in[k] == NULL) {
        errno = 0;
        return NULL;
    }

    if (g->nand_in[k] == NULL) {
        return (bool*)g->sig_in[k];
    }

    return g->nand_in[k];
}

nand_t* nand_output(nand_t const *g, ssize_t k) {

    if (g == NULL || k >= g->num_out || k < 0) {
        return NULL;
    }

    return g->nand_out[k];
}