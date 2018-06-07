/*
 * view.h
 */
#include "clish/view.h"
#include "lub/list.h"
#include "clish/hotkey.h"

struct clish_view_s {
	char *name;
	char *prompt;
	char *access;
	lub_list_t *cmds;
	lub_list_t *vars;
	lub_list_t *nspaces;
	clish_hotkeyv_t *hotkeys;
	unsigned int depth;
	clish_view_restore_e restore;
};
