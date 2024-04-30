#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <math.h>
#include <limits.h>

struct obj_t {
    int id;
    float x;
    float y;
};

struct cluster_t {
    int size;
    int capacity;
    struct obj_t *obj;
};

const int CLUSTER_CHUNK = 10;

void init_cluster(struct cluster_t *c, int cap) {
    assert(c != NULL);
    assert(cap >= 0);

    c->size = 0;
    c->capacity = cap;

    if (cap != 0)
        c->obj = malloc(cap * sizeof(struct obj_t));
}

void clear_cluster(struct cluster_t *c) {
    free(c->obj);
    c->size = 0;
    c->capacity = 0;
}

struct cluster_t *resize_cluster(struct cluster_t *c, int new_cap) {
    assert(c);
    assert(c->capacity >= 0);
    assert(new_cap >= 0);

    if (c->capacity >= new_cap)
        return c;

    size_t size = sizeof(struct obj_t) * new_cap;

    void *arr = realloc(c->obj, size);
    if (arr == NULL)
        return NULL;

    c->obj = (struct obj_t*)arr;
    c->capacity = new_cap;
    return c;
}

void append_cluster(struct cluster_t *c, struct obj_t obj) {
    assert(c != NULL);
    if (c->size >= c->capacity)
        c = resize_cluster(c, c->capacity + CLUSTER_CHUNK);

    c->obj[c->size] = obj;
    c->size++;
}

float obj_distance(struct obj_t *o1, struct obj_t *o2) {
    assert(o1 != NULL);
    assert(o2 != NULL);

    return sqrtf(pow(o1->x - o2->x, 2) + pow(o1->y - o2->y, 2));
}

float cluster_distance(struct cluster_t *c1, struct cluster_t *c2) {
    assert(c1 != NULL);
    assert(c1->size > 0);
    assert(c2 != NULL);
    assert(c2->size > 0);

    float dist_cl_min = INFINITY;
    float dist_cl;

    for (int i = 0; i < c1->size; i++)
        for (int j = 0; j < c2->size; j++){

            dist_cl = obj_distance(&c1->obj[i],&c2->obj[j]);
            if (dist_cl_min > dist_cl) {
                dist_cl_min = dist_cl;
            }

        }
    return dist_cl_min;
}

void find_neighbours(struct cluster_t *carr, int narr, int *c1, int *c2) {
    assert(narr > 0);

    float dist_min = INFINITY;
    float dist;

    for (int i = 0; i < narr; i++) {
        for (int j = i + 1; j < narr; j++) {
            dist = cluster_distance(&carr[i], &carr[j]);
            if (dist_min > dist) {
                dist_min = dist;
                *c1 = i;
                *c2 = j;
            }
        }
    }
}

void merge_clusters(struct cluster_t *c1, struct cluster_t *c2) {
    assert(c1 != NULL);
    assert(c2 != NULL);

    for (int i = 0; i < c2->size; i++){
        append_cluster(c1,c2->obj[i]);
    }

    // sort_cluster(c1); // Uncomment if sorting is required after merging
}

int remove_cluster(struct cluster_t *carr, int narr, int idx) {
    assert(idx < narr);
    assert(narr > 0);

    int new_arr = narr - 1;
    clear_cluster(&carr[idx]);
    for (int i = idx; i < new_arr; i++)
    {
        carr[i] = carr[i + 1];
    }

    return new_arr;
}

void print_cluster(struct cluster_t *c, FILE *file) {
    for (int i = 0; i < c->size; i++)
    {
        if (i) fputc(' ', file);
        fprintf(file, "%d[%g,%g]", c->obj[i].id, c->obj[i].x, c->obj[i].y);
    }
    fputc('\n', file);
}


int load_clusters(char *filename, struct cluster_t **arr) {
    assert(arr != NULL);

    FILE *file = fopen(filename, "r");
    if (file == NULL)
        return -3;

    int size;
    if (fscanf(file, "count=%d", &size) != 1){
         return -2;
     }

    *arr = malloc(size * sizeof(struct cluster_t));

    for (int i = 0; i < size; i++)
    {
        init_cluster(&(*arr)[i], 1);
        struct obj_t obj;
        int check = fscanf(file, "%d %f %f", &obj.id, &obj.x, &obj.y);
        if (check != 3) {
            return -2;
        }

        if (!((obj.id == (int)obj.id) && (obj.x == (int)obj.x) && (obj.y == (int)obj.y))) {
            for (int error = 0; error <= i; error++){
                clear_cluster(&(*arr)[error]);
            }
            size = -2;
            break;
        }

        if (obj.x < 0 || obj.x > 10000 || obj.y < 0 || obj.y > 10000) {
            for (int error = 0; error < i; error++) {
                clear_cluster(&(*arr)[i]);
            }
            size = -2;
        }
        append_cluster(&(*arr)[i], obj);
    }
    fclose(file);

    return size;
}

void print_clusters(struct cluster_t *carr, int narr, char* filename) {
    printf("Clusters:\n");
    FILE *file = fopen(filename, "w");
    if (file == NULL) {
        fprintf(stderr, "Error: Unable to open file %s.\n", filename);
        return;
    }

    for (int i = 0; i < narr; i++)
    {
        printf("cluster %d: ", i);
        fprintf(file, "cluster %d: ", i);
        print_cluster(&carr[i], file);  // Pass the file pointer to print_cluster

        printf("\n");
        fprintf(file, "\n");
    }

    fclose(file);
}


int parse_args(int argc, char *argv[], char **filename, int *N) {
    *filename = argv[1];

    if (argc == 2) {
        *N = 1;
    }
    else if (argc < 2 || argc > 3){
        return -1;
    }
    else
    {
        *N = atoi(argv[2]);

        float f_N = atof(argv[2]);
        if (f_N != *N){
            return -1;
        }

        if (*N < 0)
            return -1;

        if (*N == 0){
            return -2;
        }

    }

    return 0;
}


int main(int argc, char *argv[]) {
    struct cluster_t *clusters;

    char *filename;
    int N;
    int check = parse_args(argc, argv, &filename, &N);

    if (check == -1 || check == -2){
        fprintf(stderr, "Error: Invalid arguments.\n");
        return 1;
    }

    int size = load_clusters(filename, &clusters);

    if (size == -1 || size == -2 || size == -3) {
        fprintf(stderr, "Error: Unable to load clusters from file.\n");
        return 1;
    }

    int c1, c2;
    while (size > N)
    {
        find_neighbours(clusters, size, &c1, &c2);
        merge_clusters(&clusters[c1], &clusters[c2]);
        remove_cluster(clusters, size, c2);

        size--;
    }

    print_clusters(clusters, size, "output.txt");

    for (int i = 0; i < size; i++) {
        free(clusters[i].obj);
    }

    free(clusters);
}
