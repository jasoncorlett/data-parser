#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <stdbool.h>

#include "util.h"
#include "node.h"

char *json_node_to_string_internal(Node node, char *pad, int indent) {
    bool pretty = pad != NULL;
    
    switch (node.type) {
    case NODE_TYPE_NULL:    return copy_to_heap_string("null");
    case NODE_TYPE_STRING:  return string_quote(node.as.string);
    case NODE_TYPE_BOOLEAN: return copy_to_heap_string(node.as.boolean ? "true" : "false");
    case NODE_TYPE_NUMBER:  return copy_to_heap_string(double_to_string(node.as.number)); // FIXME: Null result?
    case NODE_TYPE_ERROR: {
        char *str = malloc(1);
        str[0] = '\0';
        string_append(&str, "[ERROR] ");
        if (node.as.string != NULL) {
            string_append(&str, node.as.string);
        }
        return str;
    }
    case NODE_TYPE_ARRAY: {
        char *str = malloc(1);
        str[0] = '\0';
        string_append(&str, "[");
        if (pretty) {
            indent++;
            string_append(&str, "\n");
        }
        if (node.as.array->length > 0) {
            if (pretty) string_indent(&str, "  ", indent);
            string_append(&str, json_node_to_string_internal(node.as.array->nodes[0], pad, indent));
            for (size_t i = 1; i < node.as.array->length; i++) {
                if (pretty) {
                    string_append(&str, ",\n");
                    string_indent(&str, pad, indent);
                }
                else {
                    string_append(&str, ",");
                }
                string_append(&str, json_node_to_string_internal(node.as.array->nodes[i], pad, indent));
            }
        }
        if (pretty) {
            string_append(&str, "\n");
            indent--;
            string_indent(&str, pad, indent);
        }
        string_append(&str, "]");
        return str;
    }
    case NODE_TYPE_MAP: {
        char *str = malloc(1);
        str[0] = '\0';

        string_append(&str, "{");
        indent++;
        if (pretty) string_append(&str, "\n");
        
        for (size_t i = 0; i < node.as.map->length; i++) {
            if (i > 0) {
                string_append(&str, ",");
                if (pretty) string_append(&str, "\n");
            }
            if (pretty) string_indent(&str, pad, indent);
            string_append(&str, string_quote(node.as.map->entries[i].key));
            string_append(&str, ":");
            string_append(&str, json_node_to_string_internal(*node.as.map->entries[i].value, pad, indent));
        }

        if (pretty) {
            string_append(&str, "\n");
            indent--;
            string_indent(&str, "  ", indent);
        }
        string_append(&str, "}");
        return str;
    };
    }
    return NULL;
}

char *json_node_to_string(Node node) {
    return json_node_to_string_internal(node, NULL, 0);
}

char *json_node_to_pretty_string(Node node) {
    return json_node_to_string_internal(node, "  ", 0);
}

void json_print_node(Node node) {
    char *str = json_node_to_string(node);
    printf("%s\n", str);
    free(str);
}

void json_pretty_print_node(Node node) {
    char *str = json_node_to_pretty_string(node);
    printf("%s\n", str);
    free(str);
}

