#include <stdio.h>
#include "parser.h"

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
    json_free_node(json);
}