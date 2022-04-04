#include "json.h"
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "parser.h"

typedef struct {
    char *name;
    char *parent;
    char *created_at;
    bool active;
    size_t path_sz;
    char **path;
} probe;

void path_append(probe *p, char *s) {
    size_t i = p->path_sz;
    p->path_sz++;
    p->path = realloc(p->path, p->path_sz * sizeof(char*));
    size_t l = strlen(s) + 1;
    p->path[i] = malloc(l);
    strncpy(p->path[i], s, l);
}

json_node probe_to_json(probe p) {
    json_node probe = *json_create_object();
    json_object_set(probe, "Name", json_create_string(p.name));
    json_object_set(probe, "Active", json_create_boolean(p.active));
    json_object_set(probe, "Created", json_create_string(p.created_at));

    if (p.parent != NULL) json_object_set(probe, "Parent", json_create_string(p.parent));
    if (p.path != NULL) {
        json_node path = *json_create_array();
        for (size_t i = 0; i < p.path_sz; i++) {
            json_array_append(path, json_create_string(p.path[i]));
        }
        json_object_set(probe, "Path", path);
    }
    return probe;
}

int main(int argc, char **argv) {
    if (argc < 2) {
        fprintf(stderr, "Usage: %s [pretty] '<json string>'\n", argv[0]);
        return 1;
    }

    json_node json = json_parse(argv[1]);

    if (json.type == JSON_ERROR) {
        fprintf(stderr, "ERROR: %s\n", json.as.string);
        return 1;
    }

    json_print_node(json);
}