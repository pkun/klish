/*
 * view.c
 *
 * This file provides the implementation of a view class
 */
#include "private.h"
#include "lub/argv.h"
#include "lub/string.h"
#include "lub/ctype.h"
#include "lub/list.h"

#include <assert.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>

/*-------------------------------------------------------- */
int clish_view_compare(const void *first, const void *second)
{
	const clish_view_t *f = (const clish_view_t *)first;
	const clish_view_t *s = (const clish_view_t *)second;
	return strcmp(f->name, s->name);
}

/*-------------------------------------------------------- */
static void clish_view_init(clish_view_t * this, const char *name)
{
	this->name = lub_string_dup(name);
	this->prompt = NULL;
	this->depth = 0;
	this->restore = CLISH_RESTORE_NONE;
	this->access = NULL;

	/* Init COMMAND list */
	this->cmds = lub_list_new(clish_command_compare, clish_command_delete);

	/* Init VAR list */
	this->vars = lub_list_new(clish_var_compare, clish_var_delete);

	/* Initialise the list of NAMESPACEs.
	 * It's important to add new items to the
	 * tail of list.
	 */
	this->nspaces = lub_list_new(NULL, clish_nspace_delete);

	/* Initialize hotkey structures */
	this->hotkeys = clish_hotkeyv_new();
}

/*--------------------------------------------------------- */
static void clish_view_fini(clish_view_t * this)
{
	/* Free COMMAND list */
	lub_list_free_all(this->cmds);

	/* Free VAR list */
	lub_list_free_all(this->vars);

	/* Free NAMESPACE list */
	lub_list_free_all(this->nspaces);

	/* Free hotkey structures */
	clish_hotkeyv_delete(this->hotkeys);

	lub_string_free(this->name);
	lub_string_free(this->prompt);
	lub_string_free(this->access);
}

/*--------------------------------------------------------- */
clish_view_t *clish_view_new(const char *name)
{
	clish_view_t *this = malloc(sizeof(clish_view_t));

	if (this)
		clish_view_init(this, name);
	return this;
}

/*-------------------------------------------------------- */
void clish_view_delete(void *data)
{
	clish_view_t *this = (clish_view_t *)data;
	clish_view_fini(this);
	free(this);
}

/*--------------------------------------------------------- */
/* Create new command and add it to list of commands */
clish_command_t *clish_view_new_command(clish_view_t *this,
	const char *name, const char *help)
{
	clish_command_t *cmd = clish_command_new(name, help);
	assert(cmd);
	clish_command__set_pview(cmd, this);
	if (!lub_list_add_uniq(this->cmds, cmd)) {
		clish_command_delete(cmd);
		cmd = NULL;
	}
	return cmd;
}

/*--------------------------------------------------------- */
static int cmd_resolve(const void *key, const void *data)
{
	const char *line = (const char *)key;
	const clish_command_t *d = (const clish_command_t *)data;
	const char *name = clish_command__get_name(d);
	size_t eq = lub_string_equal_part(line, name, BOOL_FALSE);
	if (eq == strlen(name))
		if ((line[eq] == '\0') || lub_ctype_isspace(line[eq]))
			return 0;
	return lub_string_nocasecmp(line, name);
}

/*--------------------------------------------------------- */
/* This method identifies the command (if any) which provides
 * the longest match with the specified line of text.
 *
 * NB this comparison is case insensitive.
 *
 * this - the view instance upon which to operate
 * line - the command line to analyse 
 *
 * Begin to search from tail of sorted list of cmds. So
 * the first match will be the longest command.
 */
clish_command_t *clish_view_resolve_prefix(clish_view_t *this,
	const char *line)
{
	lub_list_node_t *iter;
	clish_command_t *res = NULL;

	res = lub_list_rfind(this->cmds, cmd_resolve, line);

	/* Iterate elements. It's important to iterate
	 * items starting from tail because the next
	 * NAMESPACE has higher priority than previous one
	 * in a case then the both NAMESPACEs have the
	 * commands with the same name.
	 */
	for (iter = lub_list__get_tail(this->nspaces);
		iter; iter = lub_list_node__get_prev(iter)) {
		clish_nspace_t *nspace = (clish_nspace_t *)lub_list_node__get_data(iter);
		clish_command_t *cmd = clish_nspace_resolve_prefix(nspace, line);
		/* Choose the longest match */
		res = clish_command_choose_longest(res, cmd);
	}

	return res;
}

#if 0 // WORK
clish_command_t *clish_view_resolve_prefix(clish_view_t *this,
	const char *line, bool_t inherit)
{
	clish_command_t *result = NULL, *cmd;
	char *buffer = NULL;
	lub_argv_t *argv;
	unsigned int i;

	/* create a vector of arguments */
	argv = lub_argv_new(line, 0);

	for (i = 0; i < lub_argv__get_count(argv); i++) {
		/* set our buffer to be that of the first "i" arguments  */
		lub_string_cat(&buffer, lub_argv__get_arg(argv, i));

		/* set the result to the longest match */
		cmd = clish_view_find_command(this, buffer, inherit);

		/* job done */
		if (!cmd)
			break;
		result = cmd;

		/* ready for the next word */
		lub_string_cat(&buffer, " ");
	}

	/* free up our dynamic storage */
	lub_string_free(buffer);
	lub_argv_delete(argv);

	return result;
}



#endif

/*--------------------------------------------------------- */
clish_command_t *clish_view_resolve_command(clish_view_t *this,
	const char *line)
{
	clish_command_t *result = clish_view_resolve_prefix(this, line);

	if (result) {
		clish_action_t *action = clish_command__get_action(result);
		clish_config_t *config = clish_command__get_config(result);
		if (!clish_action__get_script(action) &&
			(!clish_action__get_builtin(action)) &&
			(CLISH_CONFIG_NONE == clish_config__get_op(config)) &&
			(!clish_command__get_param_count(result)) &&
			(!clish_command__get_viewname(result))) {
			/* if this doesn't do anything we've
			 * not resolved a command
			 */
			result = NULL;
		}
	}

	return result;
}

/*--------------------------------------------------------- */
/* Search for local (without NAMESPACEs) command by name */
clish_command_t *clish_view_find_command(clish_view_t *this,
	const char *name)
{
	return lub_list_find(this->cmds, clish_command_fn_find_by_name, name);
}

/*--------------------------------------------------------- */
static const clish_command_t *find_next_completion(clish_view_t *this,
	const char *iter_cmd, const char *line)
{
#if 0 // WORK
	clish_command_t *cmd;
	const char *name = "";
	lub_argv_t *largv;
	unsigned words;

	/* build an argument vector for the line */
	largv = lub_argv_new(line, 0);
	words = lub_argv__get_count(largv);

	/* account for trailing space */
	if (!*line || lub_ctype_isspace(line[strlen(line) - 1]))
		words++;

	if (iter_cmd)
		name = iter_cmd;
	while ((cmd = lub_bintree_findnext(&this->tree, name))) {
		name = clish_command__get_name(cmd);
		if (words == lub_string_wordcount(name)) {
			/* only bother with commands of which this line is a prefix */
			/* this is a completion */
			if (lub_string_nocasestr(name, line) == name)
				break;
		}
	}
	/* clean up the dynamic memory */
	lub_argv_delete(largv);

	return cmd;
#endif
return NULL;
}

/*--------------------------------------------------------- */
const clish_command_t *clish_view_find_next_completion(clish_view_t * this,
	const char *iter_cmd, const char *line,
	clish_nspace_visibility_e field, bool_t inherit)
{
	const clish_command_t *result, *cmd;
	clish_nspace_t *nspace;
	lub_list_node_t *iter;

	/* ask local view for next command */
	result = find_next_completion(this, iter_cmd, line);

	if (!inherit)
		return result;

	/* ask the imported namespaces for next command */

	/* Iterate elements */
	for(iter = lub_list__get_tail(this->nspaces);
		iter; iter = lub_list_node__get_prev(iter)) {
		nspace = (clish_nspace_t *)lub_list_node__get_data(iter);
		if (!clish_nspace__get_visibility(nspace, field))
			continue;
		cmd = clish_nspace_find_next_completion(nspace,
			iter_cmd, line, field);
		if (clish_command_diff(result, cmd) > 0)
			result = cmd;
	}

	return result;
}

/*--------------------------------------------------------- */
void clish_view_insert_nspace(clish_view_t * this, clish_nspace_t * nspace)
{
	lub_list_add(this->nspaces, nspace);
}

/*--------------------------------------------------------- */
void clish_view_clean_proxy(clish_view_t * this)
{
	lub_list_node_t *iter;

	/* Iterate elements */
	for(iter = lub_list__get_head(this->nspaces);
		iter; iter = lub_list_node__get_next(iter)) {
		clish_nspace_clean_proxy((clish_nspace_t *)lub_list_node__get_data(iter));
	}
}

/*--------------------------------------------------------- */
int clish_view_insert_hotkey(const clish_view_t *this, const char *key, const char *cmd)
{
	return clish_hotkeyv_insert(this->hotkeys, key, cmd);
}

/*--------------------------------------------------------- */
const char *clish_view_find_hotkey(const clish_view_t *this, int code)
{
	return clish_hotkeyv_cmd_by_code(this->hotkeys, code);
}

CLISH_GET(view, lub_list_t *, nspaces);
CLISH_GET(view, lub_list_t *, cmds);
CLISH_GET(view, lub_list_t *, vars);
CLISH_GET_STR(view, name);
CLISH_SET_STR_ONCE(view, prompt);
CLISH_GET_STR(view, prompt);
CLISH_SET_STR(view, access);
CLISH_GET_STR(view, access);
CLISH_SET(view, unsigned int, depth);
CLISH_GET(view, unsigned int, depth);
CLISH_SET(view, clish_view_restore_e, restore);
CLISH_GET(view, clish_view_restore_e, restore);
