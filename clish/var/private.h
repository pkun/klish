/*
 * var/private.h
 */
#include "clish/var.h"

struct clish_var_s {
	char *name;
	char *value;
	char *saved; /* Saved value of static variable */
	bool_t dynamic;
	clish_action_t *action;
};
