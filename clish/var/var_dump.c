/*
 * var_dump.c
 */

#include "private.h"
#include "clish/action.h"
#include "lub/xml2c.h"

#ifdef DEBUG

#include "lub/dump.h"

/*--------------------------------------------------------- */
void clish_var_dump(const clish_var_t *this)
{
	lub_dump_printf("var(%p)\n", this);
	lub_dump_indent();

	lub_dump_printf("name     : %s\n", LUB_DUMP_STR(this->name));
	lub_dump_printf("dynamic  : %s\n", LUB_DUMP_BOOL(this->dynamic));
	lub_dump_printf("value    : %s\n", LUB_DUMP_STR(this->value));
	clish_action_dump(this->action);

	lub_dump_undent();
}

/*--------------------------------------------------------- */

#endif /* DEBUG */

void clish_var_xml2c(clish_var_t *this)
{
	char *esc_name = xml2c_esc(clish_var__get_name(this));
	bool_t dynamic = clish_var__get_dynamic(this);
	char *esc_value = xml2c_esc(clish_var__get_value(this));
	clish_action_t *action;

	printf("var = clish_var_new(\"%s\");\n", XML2C_STR(esc_name)); /* name */
	printf("lub_bintree_insert(&shell->var_tree, var);\n"); /* Insert VAR to list */
	printf("clish_var__set_dynamic(var, %s);\n", XML2C_BOOL(dynamic)); /* dynamic */
	if (esc_value)
		printf("clish_var__set_value(var, \"%s\");\n", XML2C_STR(esc_value)); /* value */
	action = clish_var__get_action(this);
	if (action) {
		printf("action = clish_var__get_action(var);\n");
		clish_action_xml2c(action);
	}

	printf("\n");

	lub_string_free(esc_name);
	lub_string_free(esc_value);
}
