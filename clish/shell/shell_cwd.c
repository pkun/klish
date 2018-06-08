/*
 * shell_cwd.c
 */
#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include <syslog.h>

#include "lub/string.h"
#include "private.h"

/*--------------------------------------------------------- */
void clish_shell__init_cwd(clish_shell_cwd_t *cwd)
{
	cwd->line = NULL;
	cwd->view = NULL;
	cwd->pargv = NULL;
	cwd->cmd = NULL;
	cwd->prefix = NULL;

	/* Init VARs */
	cwd->viewid = lub_list_new(clish_var_compare, clish_var_delete);
}

/*--------------------------------------------------------- */
void clish_shell__fini_cwd(clish_shell_cwd_t *cwd)
{
	lub_string_free(cwd->line);
	lub_string_free(cwd->cmd);
	if (cwd->prefix)
		lub_string_free(cwd->prefix);
	cwd->view = NULL;
	clish_pargv_delete(cwd->pargv);

	/* Free VARs  */
	lub_list_free_all(cwd->viewid);
}

/*--------------------------------------------------------- */
void clish_shell__set_cwd(clish_shell_t *this,
	const char *line, clish_view_t *view, const char *viewid, clish_context_t *context)
{
	clish_shell_cwd_t **tmp;
	size_t new_size = 0;
	unsigned int i;
	unsigned int index = clish_view__get_depth(view);
	clish_shell_cwd_t *newcwd;
	const clish_command_t *full_cmd = clish_context__get_cmd(context);

	/* Create new element */
	newcwd = malloc(sizeof(*newcwd));
	assert(newcwd);
	clish_shell__init_cwd(newcwd);

	/* Resize the cwd vector */
	if (index >= this->cwdc) {
		new_size = (index + 1) * sizeof(clish_shell_cwd_t *);
		tmp = realloc(this->cwdv, new_size);
		assert(tmp);
		this->cwdv = tmp;
		/* Initialize new elements */
		for (i = this->cwdc; i <= index; i++) {
			clish_shell_cwd_t *cwd = malloc(sizeof(*cwd));
			assert(cwd);
			clish_shell__init_cwd(cwd);
			this->cwdv[i] = cwd;
		}
		this->cwdc = index + 1;
	}

	/* Fill the new cwd entry */
	newcwd->line = line ? lub_string_dup(line) : NULL;
	newcwd->view = view;
	newcwd->pargv = clish_pargv_clone(clish_context__get_pargv(context));
	if (full_cmd) {
		const clish_command_t *cmd = clish_command__get_cmd(full_cmd);
		newcwd->cmd = lub_string_dup(clish_command__get_name(cmd));
		if (cmd != full_cmd) {
			const char *full_cmd_name = clish_command__get_name(full_cmd);
			const char *cmd_name = clish_command__get_name(cmd);
			int len = strlen(full_cmd_name) - strlen(cmd_name);
			if (len > 1)
				newcwd->prefix = lub_string_dupn(full_cmd_name, len - 1);
		}
	}
	clish_shell__expand_viewid(viewid, newcwd->viewid, context);
	clish_shell__fini_cwd(this->cwdv[index]);
	free(this->cwdv[index]);
	this->cwdv[index] = newcwd;
	this->depth = index;
}

/*--------------------------------------------------------- */
char *clish_shell__get_cwd_line(const clish_shell_t *this, unsigned int index)
{
	if (index >= this->cwdc)
		return NULL;

	return this->cwdv[index]->line;
}

/*--------------------------------------------------------- */
clish_pargv_t *clish_shell__get_cwd_pargv(const clish_shell_t *this, unsigned int index)
{
	if (index >= this->cwdc)
		return NULL;

	return this->cwdv[index]->pargv;
}

/*--------------------------------------------------------- */
char *clish_shell__get_cwd_cmd(const clish_shell_t *this, unsigned int index)
{
	if (index >= this->cwdc)
		return NULL;

	return this->cwdv[index]->cmd;
}

/*--------------------------------------------------------- */
char *clish_shell__get_cwd_prefix(const clish_shell_t *this, unsigned int index)
{
	if (index >= this->cwdc)
		return NULL;

	return this->cwdv[index]->prefix;
}

/*--------------------------------------------------------- */
char *clish_shell__get_cwd_full(const clish_shell_t * this, unsigned int depth)
{
	char *cwd = NULL;
	unsigned int i;

	for (i = 1; i <= depth; i++) {
		const char *str =
			clish_shell__get_cwd_line(this, i);
		/* Cannot get full path */
		if (!str) {
			lub_string_free(cwd);
			return NULL;
		}
		if (cwd)
			lub_string_cat(&cwd, " ");
		lub_string_cat(&cwd, "\"");
		lub_string_cat(&cwd, str);
		lub_string_cat(&cwd, "\"");
	}

	return cwd;
}

/*--------------------------------------------------------- */
clish_view_t *clish_shell__get_cwd_view(const clish_shell_t * this, unsigned int index)
{
	if (index >= this->cwdc)
		return NULL;

	return this->cwdv[index]->view;
}

/*--------------------------------------------------------- */
int clish_shell__set_socket(clish_shell_t * this, const char * path)
{
	if (!this || !path)
		return -1;

	konf_client_free(this->client);
	this->client = konf_client_new(path);

	return 0;
}

CLISH_SET_STR(shell, lockfile);
CLISH_GET_STR(shell, lockfile);
CLISH_SET(shell, int, log_facility);
CLISH_GET(shell, int, log_facility);
CLISH_GET(shell, konf_client_t *, client);
