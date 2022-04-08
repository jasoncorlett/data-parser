#include <stdio.h>
#include "parser.h"
#include "node.h"
#include "json.h"

int main(int argc, char **argv) {
    if (argc < 2) {
        fprintf(stderr, "Usage: %s [pretty] '<json string>'\n", argv[0]);
        return 1;
    }

    Node json = json_parse(argv[1]);

    if (json.type == NODE_TYPE_ERROR) {
        fprintf(stderr, "ERROR: %s\n", json.as.string);
        return 1;
    }

    json_print_node(json);
    node_free(json);
}