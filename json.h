#pragma once

#include <stddef.h>
#include <stdbool.h>
#include "enum.h"
#include "node.h"

char *json_node_to_string(json_node node);
char *json_node_type_name(json_node node);
void json_print_node(json_node node);
void json_pretty_print_node(json_node node);
