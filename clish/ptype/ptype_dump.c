/*
 * ptype_dump.c
 */
#include "private.h"
#include "lub/xml2c.h"

#ifdef DEBUG

#include "lub/dump.h"

/*--------------------------------------------------------- */
void clish_ptype_dump(clish_ptype_t * this)
{
	lub_dump_printf("ptype(%p)\n", this);
	lub_dump_indent();
	lub_dump_printf("name       : %s\n", clish_ptype__get_name(this));
	lub_dump_printf("text       : %s\n", LUB_DUMP_STR(clish_ptype__get_text(this)));
	lub_dump_printf("pattern    : %s\n", LUB_DUMP_STR(this->pattern));
	lub_dump_printf("method     : %s\n",
		clish_ptype_method__get_name(this->method));
	lub_dump_printf("postprocess: %s\n",
		clish_ptype_preprocess__get_name(this->preprocess));
	lub_dump_undent();
}

/*--------------------------------------------------------- */

#endif /* DEBUG */

static const char *method_macros[] = {
	"CLISH_PTYPE_REGEXP",
	"CLISH_PTYPE_INTEGER",
	"CLISH_PTYPE_UNSIGNEDINTEGER",
	"CLISH_PTYPE_SELECT"
};

static const char *preprocess_macros[] = {
	"CLISH_PTYPE_NONE",
	"CLISH_PTYPE_TOUPPER",
	"CLISH_PTYPE_TOLOWER"
};

void clish_ptype_xml2c(clish_ptype_t *this)
{
	char *esc_name = xml2c_esc(clish_ptype__get_name(this));
	char *esc_help = xml2c_esc(clish_ptype__get_text(this));
	char *esc_pattern = xml2c_esc(this->pattern);

	printf("clish_shell_find_create_ptype(shell,\n");
	printf("\t\"%s\",\n", XML2C_STR(esc_name)); /* name */
	printf("\t\"%s\",\n", XML2C_STR(esc_help)); /* help */
	printf("\t\"%s\",\n", XML2C_STR(esc_pattern)); /* pattern */
	printf("\t%s,\n", xml2c_enum(this->method, method_macros)); /* method */
	printf("\t%s\n", xml2c_enum(this->preprocess, preprocess_macros)); /* preprocess */
	printf(");\n\n");

	lub_string_free(esc_name);
	lub_string_free(esc_help);
	lub_string_free(esc_pattern);
}
