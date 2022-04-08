#ifndef NODE_H
#define NODE_H 1

#include <stddef.h>
#include <stdbool.h>
typedef enum {
	NODE_TYPE_ERROR = -1,
	NODE_TYPE_NULL,
	NODE_TYPE_STRING,
	NODE_TYPE_BOOLEAN,
	NODE_TYPE_NUMBER,
	NODE_TYPE_ARRAY,
	NODE_TYPE_MAP,
} NodeType;

typedef struct Node         Node;
typedef struct NodeArray    NodeArray;
typedef struct NodeMapEntry NodeMapEntry;
typedef struct NodeMap      NodeMap;

struct NodeArray {
    size_t length;
    Node *nodes;
};

struct NodeMapEntry {
    char *key;
    Node *value;
};

struct NodeMap {
    size_t length;
    NodeMapEntry *entries;
};

struct Node {
    NodeType type;
    union {
        char *string;
        char boolean;
        double number;
        NodeArray *array;
        NodeMap *map;
    } as;
};

char *node_type_name(NodeType type);

Node *node_create_array(void);
void node_array_append(Node array, Node node);

Node *node_create_map(void);
void node_map_set(Node map, char *key, Node node);

Node node_create_null();
Node node_create_boolean(bool b);
Node node_create_number(double d);
Node node_create_string(char *str);
Node node_create_error(char *str);

void node_free(Node node);

#endif