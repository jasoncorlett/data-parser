#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <stdbool.h>

#include "util.h"
#include "enum.h"
#include "node.h"

char *json_node_to_string_internal(json_node node, char *pad, int indent) {
    bool pretty = pad != NULL;
    
    switch (node.type) {
    case JSON_NULL:    return copy_to_heap_string("null");
    case JSON_STRING:  return string_quote(node.as.string);
    case JSON_BOOLEAN: return copy_to_heap_string(node.as.boolean ? "true" : "false");
    case JSON_NUMBER:  return copy_to_heap_string(double_to_string(node.as.number)); // FIXME: Null result?
    case JSON_ERROR: {
        char *str = malloc(1);
        str[0] = '\0';
        string_append(&str, "[ERROR] ");
        if (node.as.string != NULL) {
            string_append(&str, node.as.string);
        }
        return str;
    }
    case JSON_ARRAY: {
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
    case JSON_OBJECT: {
        char *str = malloc(1);
        str[0] = '\0';

        string_append(&str, "{");
        indent++;
        if (pretty) string_append(&str, "\n");
        
        for (size_t i = 0; i < node.as.object->length; i++) {
            if (i > 0) {
                string_append(&str, ",");
                if (pretty) string_append(&str, "\n");
            }
            if (pretty) string_indent(&str, pad, indent);
            string_append(&str, string_quote(node.as.object->entries[i].key));
            string_append(&str, ":");
            string_append(&str, json_node_to_string_internal(*node.as.object->entries[i].value, pad, indent));
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

char *json_node_to_string(json_node node) {
    return json_node_to_string_internal(node, NULL, 0);
}

char *json_node_to_pretty_string(json_node node) {
    return json_node_to_string_internal(node, "  ", 0);
}

void json_print_node(json_node node) {
    char *str = json_node_to_string(node);
    printf("%s\n", str);
    free(str);
}

void json_pretty_print_node(json_node node) {
    char *str = json_node_to_pretty_string(node);
    printf("%s\n", str);
    free(str);
}

char* json_node_type_name(json_node node) {
    return json_type_name(node.type);
}
