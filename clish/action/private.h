/*
 * action.h
 */

#include "clish/action.h"

struct clish_action_s {
	char *script;
	const clish_sym_t *builtin;
	char *shebang;
	bool_t lock;
	bool_t interrupt;
	bool_t interactive;
	bool_t permanent; // if true then ACTION will be executed on dryrun
};
