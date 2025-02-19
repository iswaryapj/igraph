#include "prpack_igraph_graph.h"
#include <stdexcept>
#include <climits>
#include <cstdlib>
#include <cstring>

#include "igraph_interface.h"

using namespace prpack;
using namespace std;

#ifdef PRPACK_IGRAPH_SUPPORT

prpack_igraph_graph::prpack_igraph_graph(const igraph_t* g, const igraph_vector_t* weights,
        bool directed) {
    const igraph_bool_t treat_as_directed = igraph_is_directed(g) && directed;
    igraph_es_t es;
    igraph_eit_t eit;
    igraph_vector_int_t neis;
    igraph_integer_t vcount = igraph_vcount(g), ecount = igraph_ecount(g);
    igraph_error_t err;
    int *p_head, *p_head_copy;
    double* p_weight = 0;

    if (vcount > INT_MAX) {
        throw std::domain_error("Too many vertices for PRPACK.");
    }
    if (ecount > (treat_as_directed ? INT_MAX : INT_MAX/2)) {
        throw std::domain_error("Too many edges for PRPACK.");
    }

    // Get the number of vertices and edges. For undirected graphs, we add
    // an edge in both directions.
    num_vs = (int) vcount;
    num_es = (int) ecount;
    num_self_es = 0;
    if (!treat_as_directed) {
        num_es *= 2;
    }

    // Allocate memory for heads and tails
    p_head = heads = new int[num_es];
    tails = new int[num_vs];
    memset(tails, 0, num_vs * sizeof(tails[0]));

    // Allocate memory for weights if needed
    if (weights != 0) {
        p_weight = vals = new double[num_es];
    }

    // Count the number of ignored edges (those with negative or zero weight)
    int num_ignored_es = 0;

    if (treat_as_directed) {
        // Select all the edges and iterate over them by the source vertices
        es = igraph_ess_all(IGRAPH_EDGEORDER_TO);

        // Add the edges
        igraph_eit_create(g, es, &eit);
        while (!IGRAPH_EIT_END(eit)) {
            igraph_integer_t eid = IGRAPH_EIT_GET(eit);
            IGRAPH_EIT_NEXT(eit);

            // Handle the weight
            if (weights != 0) {
                // Does this edge have zero or negative weight?
                if (VECTOR(*weights)[eid] <= 0) {
                    // Ignore it.
                    num_ignored_es++;
                    continue;
                }

                *p_weight = VECTOR(*weights)[eid];
                ++p_weight;
            }

            *p_head = IGRAPH_FROM(g, eid);
            ++p_head;
            ++tails[IGRAPH_TO(g, eid)];

            if (IGRAPH_FROM(g, eid) == IGRAPH_TO(g, eid)) {
                ++num_self_es;
            }
        }
        igraph_eit_destroy(&eit);
    } else {
        // Select all the edges and iterate over them by the target vertices
        err = igraph_vector_int_init(&neis, 0);
        if (err != IGRAPH_SUCCESS) {
            throw std::runtime_error("Failed to convert graph for PRPACK.");
        }

        for (int i = 0; i < num_vs; i++) {
            err = igraph_incident(g, &neis, i, IGRAPH_ALL);
            if (err != IGRAPH_SUCCESS) {
                throw std::runtime_error("Failed to convert graph for PRPACK.");
            }

            int temp = igraph_vector_int_size(&neis);

            // TODO: should loop edges be added in both directions?
            p_head_copy = p_head;
            for (int j = 0; j < temp; j++) {
                if (weights != 0) {
                    if (VECTOR(*weights)[VECTOR(neis)[j]] <= 0) {
                        // Ignore
                        num_ignored_es++;
                        continue;
                    }

                    *p_weight = VECTOR(*weights)[VECTOR(neis)[j]];
                    ++p_weight;
                }

                *p_head = IGRAPH_OTHER(g, VECTOR(neis)[j], i);
                if (i == *p_head) {
                    num_self_es++;
                }
                ++p_head;
            }
            tails[i] = p_head - p_head_copy;
        }

        igraph_vector_int_destroy(&neis);
    }

    // Decrease num_es by the number of ignored edges
    num_es -= num_ignored_es;

    // Finalize the tails vector
    for (int i = 0, sum = 0; i < num_vs; ++i) {
        int temp = sum;
        sum += tails[i];
        tails[i] = temp;
    }

    // Normalize the weights
    normalize_weights();

    // Debug
    /*
    printf("Heads:");
    for (i = 0; i < num_es; ++i) {
        printf(" %d", heads[i]);
    }
    printf("\n");
    printf("Tails:");
    for (i = 0; i < num_vs; ++i) {
        printf(" %d", tails[i]);
    }
    printf("\n");
    if (vals) {
        printf("Vals:");
        for (i = 0; i < num_es; ++i) {
            printf(" %.4f", vals[i]);
        }
        printf("\n");
    }
    printf("===========================\n");
    */
}

// PRPACK_IGRAPH_SUPPORT
#endif
