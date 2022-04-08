#pragma once

#include <stddef.h>
#include <stdbool.h>
#include "enum.h"
#include "node.h"

char *json_node_to_string(Node node);
char *json_node_type_name(Node node);
void json_print_node(Node node);
void json_pretty_print_node(Node node);
