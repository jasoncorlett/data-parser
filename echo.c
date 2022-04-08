#include <stdio.h>
#include "node.h"
#include "json.h"

int main(int argc, char **argv) {
    if (argc < 2) {
        fprintf(stderr, "Usage: %s [pretty] '<json string>'\n", argv[0]);
        return 1;
    }

    Node node = json_parse(argv[1]);

    if (node.type == NODE_TYPE_ERROR) {
        fprintf(stderr, "ERROR: %s\n", node.as.string);
        return 1;
    }

    json_print_node(node);
    node_free(node);
}