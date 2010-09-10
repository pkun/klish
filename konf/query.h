#ifndef _konf_query_h
#define _konf_query_h

typedef enum
{
  KONF_QUERY_OP_NONE,
  KONF_QUERY_OP_OK,
  KONF_QUERY_OP_ERROR,
  KONF_QUERY_OP_SET,
  KONF_QUERY_OP_UNSET,
  KONF_QUERY_OP_STREAM,
  KONF_QUERY_OP_DUMP
} konf_query_op_t;

typedef struct konf_query_s konf_query_t;

int konf_query_parse(konf_query_t *query, int argc, char **argv);
int konf_query_parse_str(konf_query_t *query, char *str);
konf_query_t *konf_query_new(void);
void konf_query_free(konf_query_t *query);
char *konf_query__get_pwd(konf_query_t *query, unsigned index);
int konf_query__get_pwdc(konf_query_t *query);
void konf_query_dump(konf_query_t *query);
konf_query_op_t konf_query__get_op(konf_query_t *query);
char * konf_query__get_path(konf_query_t *query);
const char * konf_query__get_pattern(konf_query_t *instance);
const char * konf_query__get_line(konf_query_t *instance);
unsigned short konf_query__get_priority(konf_query_t *instance);
bool_t konf_query__get_splitter(konf_query_t *instance);
bool_t konf_query__get_seq(konf_query_t *instance);
unsigned short konf_query__get_seq_num(konf_query_t *instance);

#endif
