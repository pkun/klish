/*
 * command.h
 */

#include "clish/command.h"

struct clish_command_s {
	char *name;
	char *text;
	char *viewname;
	char *viewid;
	char *detail;
	char *escape_chars;
	char *regex_chars;
	char *access;
	char *alias_view;
	char *alias;
	clish_paramv_t *paramv;
	clish_action_t *action;
	clish_config_t *config;
	clish_param_t *args;
	const struct clish_command_s *link;
	clish_view_t *pview;
	bool_t dynamic; /* Is command dynamically created */
	bool_t internal; /* Is command internal? Like the "startup" */
};
