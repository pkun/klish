/*
 * shell_dump.c
 */
#include "private.h"

#ifdef DEBUG

#include "lub/dump.h"

/*--------------------------------------------------------- */
void clish_shell_dump(clish_shell_t * this)
{
	clish_view_t *v;
	clish_ptype_t *t;
	clish_var_t *var;
	lub_bintree_iterator_t iter;

	lub_dump_printf("shell(%p)\n", this);
	lub_dump_printf("OVERVIEW:\n%s\n", LUB_DUMP_STR(this->overview));
	lub_dump_indent();

	v = lub_bintree_findfirst(&this->view_tree);

	/* iterate the tree of views */
	for (lub_bintree_iterator_init(&iter, &this->view_tree, v);
		v; v = lub_bintree_iterator_next(&iter)) {
		clish_view_dump(v);
	}

	/* iterate the tree of types */
	t = lub_bintree_findfirst(&this->ptype_tree);
	for (lub_bintree_iterator_init(&iter, &this->ptype_tree, t);
		t; t = lub_bintree_iterator_next(&iter)) {
		clish_ptype_dump(t);
	}

	/* iterate the tree of vars */
	var = lub_bintree_findfirst(&this->var_tree);
	for (lub_bintree_iterator_init(&iter, &this->var_tree, var);
		var; var = lub_bintree_iterator_next(&iter)) {
		clish_var_dump(var);
	}

	lub_dump_undent();
}

/*--------------------------------------------------------- */

#endif /* DEBUG */

/*--------------------------------------------------------- */
void clish_shell_xml2c(clish_shell_t *this)
{
	clish_view_t *v;
	clish_ptype_t *t;
	clish_var_t *var;
	lub_bintree_iterator_t iter;

	printf("#include \"private.h\"\n"
		"#include \"lub/string.h\"\n"
		"#include <stdlib.h>\n"
		"#include <string.h>\n"
		"#include <assert.h>\n"
		"#include <errno.h>\n"
		"#include <sys/types.h>\n"
		"\n"
		);

	printf("int clish_shell_load_scheme(clish_shell_t *shell, const char *xml_path)\n"
		"{\n\n");

	/* Iterate the tree of types */
	printf("/*########## PTYPE ##########*/\n\n");
	t = lub_bintree_findfirst(&this->ptype_tree);
	for (lub_bintree_iterator_init(&iter, &this->ptype_tree, t);
		t; t = lub_bintree_iterator_next(&iter)) {
		clish_ptype_xml2c(t);
	}
#if 0
	v = lub_bintree_findfirst(&this->view_tree);
	/* iterate the tree of views */
	for (lub_bintree_iterator_init(&iter, &this->view_tree, v);
		v; v = lub_bintree_iterator_next(&iter)) {
		clish_view_dump(v);
	}

	/* iterate the tree of vars */
	var = lub_bintree_findfirst(&this->var_tree);
	for (lub_bintree_iterator_init(&iter, &this->var_tree, var);
		var; var = lub_bintree_iterator_next(&iter)) {
		clish_var_dump(var);
	}
#endif

	printf("\n}\n");
}

