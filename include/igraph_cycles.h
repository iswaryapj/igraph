
#ifndef IGRAPH_CYCLES_H
#define IGRAPH_CYCLES_H

#include "igraph_decls.h"
#include "igraph_interface.h"
#include "igraph_types.h"
#include "igraph_vector_list.h"

__BEGIN_DECLS

IGRAPH_EXPORT igraph_error_t igraph_fundamental_cycles(
        const igraph_t *graph,
        const igraph_vector_t *weights,
        igraph_vector_int_list_t *result,
        igraph_integer_t start_vid,
        igraph_integer_t bfs_cutoff);

IGRAPH_EXPORT igraph_error_t igraph_minimum_cycle_basis(
        const igraph_t *graph,
        const igraph_vector_t *weights,
        igraph_vector_int_list_t *result,
        igraph_integer_t bfs_cutoff,
        igraph_bool_t complete,
        igraph_bool_t use_cycle_order);

__END_DECLS

#endif
