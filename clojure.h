#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <sys/time.h>
#include <math.h>
#include <gc.h>
#include <stdarg.h>
#include <ctype.h>
#include <editline/readline.h>
#include <editline/history.h>

//type 15种类型 同像性
enum{
one,  //void 0
MALTYPE_SYMBOL, //1
MALTYPE_KEYWORD, //2
MALTYPE_INTEGER, //3
MALTYPE_FLOAT, //4
MALTYPE_STRING, //5
MALTYPE_TRUE, //6
MALTYPE_FALSE, //7
MALTYPE_NIL, //8
MALTYPE_LIST, //9
MALTYPE_VECTOR, //10
MALTYPE_HASHMAP, //11
MALTYPE_FUNCTION, //12 // native function
MALTYPE_CLOSURE, //13  // user custom function
MALTYPE_ERROR, //14
MALTYPE_ATOM //15
};

#define UNREADABLY 0
#define READABLY 1

// special form (special symbol)
#define SYMBOL_DEFBANG "def!"
#define SYMBOL_LETSTAR "let*"
#define SYMBOL_DO "do"
#define SYMBOL_IF "if"
#define SYMBOL_FNSTAR "fn*"
#define SYMBOL_QUOTE "quote"
#define SYMBOL_QUASIQUOTE "quasiquote"
#define SYMBOL_QUASIQUOTEEXPAND "quasiquoteexpand"
#define SYMBOL_UNQUOTE "unquote"
#define SYMBOL_SPLICE_UNQUOTE "splice-unquote"
#define SYMBOL_DEFMACROBANG "defmacro!"
#define SYMBOL_MACROEXPAND "macroexpand"
#define SYMBOL_TRYSTAR "try*"
#define SYMBOL_CATCHSTAR "catch*"

#define PROMPT_STRING "user> "

#define STRING_BUFFER_SIZE 256

#define PRINT_NIL "nil"
#define PRINT_TRUE "true"
#define PRINT_FALSE "false"

#define INTEGER_BUFFER_SIZE 16
#define SYMBOL_BUFFER_SIZE 32
#define FUNCTION_BUFFER_SIZE 256
// #define STRING_BUFFER_SIZE 256
#define LIST_BUFFER_SIZE 10240
#define ERROR_BUFFER_SIZE 128

// token type 
#define TOKEN_SPECIAL_CHARACTER 1  //9种符号 ( { [ ' ` ~ @ ~@ ^
#define TOKEN_STRING 2
#define TOKEN_INTEGER 3
#define TOKEN_FLOAT 4
#define TOKEN_SYMBOL 5
#define TOKEN_COMMENT 6
#define TOKEN_KEYWORD 7
#define TOKEN_TRUE 8
#define TOKEN_FALSE 9
#define TOKEN_NIL 10

#define SYMBOL_NIL "nil"
#define SYMBOL_TRUE "true"
#define SYMBOL_FALSE "false"
#define SYMBOL_QUOTE "quote"
#define SYMBOL_QUASIQUOTE "quasiquote"
#define SYMBOL_UNQUOTE "unquote"
#define SYMBOL_SPLICE_UNQUOTE "splice-unquote"
#define SYMBOL_DEREF "deref"
#define SYMBOL_WITH_META "with-meta"

/* simplify references to void pointers */
typedef void *gptr;
/* linked list is constructed of pairs */
typedef struct pair {
  gptr data;
  struct pair *next;
} pair;

/* a list is just a pointer to the pair at the head of the list */
typedef pair *list;
/* a hashmap is just a list with alternating key/value pairs */
typedef pair *hashmap;

typedef struct ns {
  hashmap mappings;
} ns;

typedef struct Env {
  struct Env *outer;
  hashmap data;
} Env;

typedef struct maltype maltype;

typedef struct MalClosure {
  Env *env;
  maltype *parameters;
  maltype *more_symbol;
  maltype *definition;
} MalClosure;
/*四个字段 
 vector mal_vector;  TODO: implement a real vector
 hashmap mal_hashmap; TODO: implement a real hashmap */
struct maltype {
  int type;
  int is_macro;
  maltype *metadata;
  union MalValue {
    long mal_integer;
    double mal_float;
    char *mal_symbol;
    char *mal_string;
    char *mal_keyword;
    list mal_list;
    maltype *(*mal_function)(list);
    MalClosure *mal_closure;
    maltype *mal_atom;
    maltype *mal_error;
  } value;
};

typedef struct Token {
  int type;
  char *data;
  char *error;
} Token;

typedef struct Reader {
  long position;      // current position in the array
  long token_count;   // number of tokens in the array
  long max_tokens;    // maximum number of tokens the array can hold
  Token **token_data; // pointer to an array of Tokens
  char *error;        // error message
} Reader;

/* interface */
list list_make(gptr data_ptr);
list list_push(list lst, gptr data_ptr);
gptr list_peek(list lst);
gptr list_nth(list lst, int n);
gptr list_first(list lst);
list list_rest(list lst);
list list_pop(list lst);
list list_reverse(list lst);
long list_count(list lst);
list list_concatenate(list lst1, list lst2);
list list_copy(list lst);
long list_findf(list lst, char *keystring, char *(*fn)(gptr));

hashmap hashmap_make(char *keystring, gptr data_ptr);
hashmap hashmap_put(hashmap map, char *keystring, gptr data_ptr);
gptr hashmap_get(hashmap map, char *keystring);
gptr hashmap_getf(hashmap map, char *keystring, char *(*fn)(gptr));
hashmap hashmap_updatef(hashmap map, char *keystring, gptr value, char *(*fn)(gptr));

Env *env_make(Env *outer, list binds, list exprs, maltype *variadic_symbol);
Env *env_set(Env *current, maltype *symbol, maltype *value);
Env *env_set_C_fn(Env *current, char *symbol_name, maltype *(*fn)(list));
maltype *env_get(Env *current, maltype *symbol);
Env *env_find(Env *current, maltype *symbol);

maltype *make_symbol(char *value);
maltype *make_integer(long value);
maltype *make_float(double value);
maltype *make_keyword(char *value);
maltype *make_string(char *value);
maltype *make_list(list value);
maltype *make_vector(list value);
maltype *make_hashmap(list value);
maltype *make_true();
maltype *make_false();
maltype *make_nil();
maltype *make_atom(maltype *value);
maltype *make_error(char *msg);
maltype *make_error_fmt(char *fmt, ...);
maltype *wrap_error(maltype *value);
maltype *make_function(maltype *(*fn)(list args));
maltype *make_closure(Env *env, maltype *parameters, maltype *definition, maltype *more_symbol);
maltype *copy_type(maltype *value);

int is_sequential(maltype *val);
int is_self_evaluating(maltype *val);
int is_list(maltype *val);
int is_vector(maltype *val);
int is_hashmap(maltype *val);
int is_nil(maltype *val);
int is_string(maltype *val);
int is_integer(maltype *val);
int is_float(maltype *val);
int is_number(maltype *val);
int is_true(maltype *val);
int is_false(maltype *val);
int is_symbol(maltype *val);
int is_keyword(maltype *val);
int is_atom(maltype *val);
int is_error(maltype *val);
int is_callable(maltype *val);
int is_function(maltype *val);
int is_closure(maltype *val);
int is_macro(maltype *val);

int is_macro_call(maltype * ast, Env * env);
maltype *macroexpand(maltype * ast, Env * env);
maltype *regularise_parameters(list * params, maltype * *more);
maltype *quasiquote(maltype * ast);
maltype *quasiquote_list(maltype * ast);
maltype *quasiquote_vector(maltype * ast);

ns *ns_make_core();
maltype *as_str(list args, int readably, char *separator);
maltype *print(list args, int readably, char *separator);
char *get_fn(gptr data);
maltype *equal_lists(maltype *lst1, maltype *lst2);
maltype *equal_hashmaps(maltype *map1, maltype *map2);

/* reader object */
Reader *reader_make(long token_capacity);
Reader *reader_append(Reader *reader, Token *token);
Token *reader_peek(const Reader *reader);
Token *reader_next(Reader *reader);
Token *reader_get_at(const Reader *reader, long i);
void reader_print(Reader *reader);

/* tokenizing the input */
Reader *tokenize(char *token_string);
char *read_fixed_length_token(char *current, Token **ptoken, int n);
char *read_string_token(char *current, Token **ptoken);
char *read_comment_token(char *current, Token **ptoken);
// char* read_integer_token(char* current, Token** ptoken);
char *read_number_token(char *current, Token **ptoken);
char *read_symbol_token(char *current, Token **ptoken);
char *read_keyword_token(char *current, Token **ptoken);


/* reading the tokens into types */
maltype *read_str(char *token_string);
maltype *read_form(Reader *reader);
maltype *read_atom(Reader *reader);
maltype *read_list(Reader *reader);
maltype *read_vector(Reader *reader);
maltype *read_hashmap(Reader *reader);

/* utility functions */
char *read_terminated_token(char *current, Token **ptoken, int type);
maltype *read_matched_delimiters(Reader *reader, char start_delimiter, char end_delimiter);
maltype *make_symbol_list(Reader *reader, char *symbol_name);
Token *token_allocate(char *str, long num_chars, int type, char *error);
char *unescape_string(char *str, long length);

char *pr_str(maltype *mal_val, int readably);
char *pr_str_list(list lst, int readably, char *start_delimiter, char *end_delimiter, char *separator);
char *escape_string(char *str);
char *snprintfbuf(long initial_size, char *fmt, ...);

// ns core native fn
/* forward references to main file */
maltype *apply(maltype *fn, list args);

/* core ns functions */
maltype *mal_add(list);
maltype *mal_sub(list);
maltype *mal_mul(list);
maltype *mal_div(list);

maltype *mal_prn(list);
maltype *mal_println(list);
maltype *mal_pr_str(list);
maltype *mal_str(list);
maltype *mal_read_string(list);
maltype *mal_slurp(list);

maltype *mal_list(list);
maltype *mal_list_questionmark(list);
maltype *mal_empty_questionmark(list);
maltype *mal_count(list);
maltype *mal_cons(list);
maltype *mal_concat(list);
maltype *mal_nth(list);
maltype *mal_first(list);
maltype *mal_rest(list);

maltype *mal_equals(list);
maltype *mal_lessthan(list);
maltype *mal_lessthanorequalto(list);
maltype *mal_greaterthan(list);
maltype *mal_greaterthanorequalto(list);

maltype *mal_atom(list);
maltype *mal_atom_questionmark(list);
maltype *mal_deref(list);
maltype *mal_reset_bang(list);
maltype *mal_swap_bang(list);

maltype *mal_throw(list);
maltype *mal_apply(list);
maltype *mal_map(list);

maltype *mal_nil_questionmark(list);
maltype *mal_true_questionmark(list);
maltype *mal_false_questionmark(list);
maltype *mal_symbol_questionmark(list);
maltype *mal_keyword_questionmark(list);
maltype *mal_symbol(list);
maltype *mal_keyword(list);

maltype *mal_vec(list);
maltype *mal_vector(list);
maltype *mal_vector_questionmark(list);
maltype *mal_sequential_questionmark(list);
maltype *mal_hash_map(list);
maltype *mal_map_questionmark(list);
maltype *mal_assoc(list);
maltype *mal_dissoc(list);
maltype *mal_get(list);
maltype *mal_contains_questionmark(list);
maltype *mal_keys(list);
maltype *mal_vals(list);
maltype *mal_string_questionmark(list);
maltype *mal_number_questionmark(list);
maltype *mal_fn_questionmark(list);
maltype *mal_macro_questionmark(list);

maltype *mal_time_ms(list);
maltype *mal_conj(list);
maltype *mal_seq(list);
maltype *mal_meta(list);
maltype *mal_with_meta(list);
