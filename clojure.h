
/* simplify references to void pointers */
typedef void* gptr;

/* linked list is constructed of pairs */
typedef struct pair_s {

  gptr data;
  struct pair_s *next;

} pair;

/* a list is just a pointer to the pair at the head of the list */
typedef pair* list;

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
long list_findf(list lst, char* keystring, char*(*fn)(gptr));

/* a hashmap is just a list with alternating key/value pairs */
typedef list hashmap;

hashmap hashmap_make(char* keystring, gptr data_ptr);
hashmap hashmap_put(hashmap map, char* keystring, gptr data_ptr);
gptr hashmap_get(hashmap map, char* keystring);
gptr hashmap_getf(hashmap map, char* keystring, char*(*fn)(gptr));
hashmap hashmap_updatef(hashmap map, char* keystring, gptr value, char*(*fn)(gptr));


typedef struct ns_s ns;

struct ns_s {

  hashmap mappings;

};
#define MALTYPE_SYMBOL 1
#define MALTYPE_KEYWORD 2
#define MALTYPE_INTEGER 3
#define MALTYPE_FLOAT 4
#define MALTYPE_STRING 5
#define MALTYPE_TRUE 6
#define MALTYPE_FALSE 7
#define MALTYPE_NIL 8
#define MALTYPE_LIST 9
#define MALTYPE_VECTOR 10
#define MALTYPE_HASHMAP 11
#define MALTYPE_FUNCTION 12
#define MALTYPE_CLOSURE 13
#define MALTYPE_ERROR 14
#define MALTYPE_ATOM 15

typedef struct MalType_s MalType;
typedef struct MalClosure_s MalClosure;
typedef struct Env_s Env;
typedef struct Env_s Env;

struct Env_s {

  struct Env_s* outer;
  hashmap data;

};

Env* env_make(Env* outer, list binds, list exprs, MalType* variadic_symbol);
Env* env_set(Env* current, MalType* symbol, MalType* value);
Env* env_set_C_fn(Env* current, char* symbol_name, MalType*(*fn)(list));
MalType* env_get(Env* current, MalType* symbol);
Env* env_find(Env* current, MalType* symbol);
struct MalType_s {

  int type;
  int is_macro;
  MalType* metadata;

  union MalValue {

    long mal_integer;
    double mal_float;
    char* mal_symbol;
    char* mal_string;
    char* mal_keyword;
    list mal_list;
    /* vector mal_vector;  TODO: implement a real vector */
    /* hashmap mal_hashmap; TODO: implement a real hashmap */
    MalType* (*mal_function)(list);
    MalClosure* mal_closure;
    MalType* mal_atom;
    MalType* mal_error;

  } value;
};

struct MalClosure_s {

  Env* env;
  MalType* parameters;
  MalType* more_symbol;
  MalType* definition;

};

MalType* make_symbol(char* value);
MalType* make_integer(long value);
MalType* make_float(double value);
MalType* make_keyword(char* value);
MalType* make_string(char* value);
MalType* make_list(list value);
MalType* make_vector(list value);
MalType* make_hashmap(list value);
MalType* make_true();
MalType* make_false();
MalType* make_nil();
MalType* make_atom(MalType* value);
MalType* make_error(char* msg);
MalType* make_error_fmt(char* fmt, ...);
MalType* wrap_error(MalType* value);
MalType* make_function(MalType*(*fn)(list args));
MalType* make_closure(Env* env, MalType* parameters, MalType* definition, MalType* more_symbol);
MalType* copy_type(MalType* value);

int is_sequential(MalType* val);
int is_self_evaluating(MalType* val);
int is_list(MalType* val);
int is_vector(MalType* val);
int is_hashmap(MalType* val);
int is_nil(MalType* val);
int is_string(MalType* val);
int is_integer(MalType* val);
int is_float(MalType* val);
int is_number(MalType* val);
int is_true(MalType* val);
int is_false(MalType* val);
int is_symbol(MalType* val);
int is_keyword(MalType* val);
int is_atom(MalType* val);
int is_error(MalType* val);
int is_callable(MalType* val);
int is_function(MalType* val);
int is_closure(MalType* val);
int is_macro(MalType* val);

ns* ns_make_core();
MalType* as_str(list args, int readably, char* separator);
MalType* print(list args, int readably, char* separator);
char* get_fn(gptr data);
MalType* equal_lists(MalType* lst1, MalType* lst2);
MalType* equal_hashmaps(MalType* map1, MalType* map2);


typedef struct Token_s {

  int type;
  char* data;
  char* error;

} Token;

typedef struct Reader_s {

  long position;      // current position in the array
  long token_count;   // number of tokens in the array
  long max_tokens;    // maximum number of tokens the array can hold
  Token** token_data; // pointer to an array of Tokens
  char* error;        // error message

} Reader;

/* reader object */
Reader* reader_make(long token_capacity);
Reader* reader_append(Reader* reader, Token* token);
Token* reader_peek(const Reader* reader);
Token* reader_next(Reader* reader);
Token* reader_get_at(const Reader* reader, long i);
void reader_print(Reader* reader);

/* tokenizing the input */
Reader* tokenize(char* token_string);
char* read_fixed_length_token(char* current, Token** ptoken, int n);
char* read_string_token(char* current, Token** ptoken);
char* read_comment_token(char* current, Token** ptoken);
//char* read_integer_token(char* current, Token** ptoken);
char* read_number_token(char* current, Token** ptoken);
char* read_symbol_token(char* current, Token** ptoken);
char* read_keyword_token(char* current, Token** ptoken);

/* reading the tokens into types */
MalType* read_str(char* token_string);
MalType* read_form(Reader* reader);
MalType* read_atom(Reader* reader);
MalType* read_list(Reader* reader);
MalType* read_vector(Reader* reader);
MalType* read_hashmap(Reader* reader);

/* utility functions */
char* read_terminated_token (char* current, Token** ptoken, int type);
MalType* read_matched_delimiters(Reader* reader, char start_delimiter, char end_delimiter);
MalType* make_symbol_list(Reader* reader, char* symbol_name);
Token* token_allocate(char* str, long num_chars, int type, char* error);
char* unescape_string(char* str, long length);

#define UNREADABLY 0
#define READABLY 1

char* pr_str(MalType* mal_val, int readably);
char* pr_str_list(list lst, int readably, char* start_delimiter, char* end_delimiter, char* separator);
char* escape_string(char* str);
char* snprintfbuf(long initial_size, char* fmt, ...);