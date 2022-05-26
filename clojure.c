#include "clojure.h"
// list.c
list list_make(gptr data_ptr) { return list_push(NULL, data_ptr); }

list list_push(list lst, gptr data_ptr) {
  pair *n = GC_malloc(sizeof(pair));
  n->data = data_ptr;
  n->next = lst;
  return n;
}
//获取node data
// gptr list_peek(list lst) {
//   return (lst ? lst->data : NULL);
// }

// list list_pop(list lst) {
//   return (lst ? lst->next : NULL);
// }
long list_count(list lst) {
  /* handle empty case */
  if (!lst) { return 0; }
  long counter = 1;
  while (lst->next) {
    counter++;
    lst = lst->next;
  }
  return counter;
}

list list_reverse(list lst) {
  /* list is not empty and has more than one element */
  if (lst && lst->next) {
    pair *prev = NULL, *next = NULL, *current = lst;
    while (current) {
      /* stash current value of next pointer --> */
      next = current->next;
      /* reverse the next pointer on current pair <-- */
      current->next = prev;
      /* move on to next pair and repeat --> */
      prev = current;
      current = next;
    }
    lst = prev; /* head of list is in prev when current = NULL */
  }
  return lst;
}

list list_concatenate(list lst1, list lst2) {
  list new_lst = NULL;
  list iterator = NULL;
  while (lst2) {
    gptr val = lst2->data;
    new_lst = list_push(new_lst, val);
    lst2 = lst2->next;
  }
  new_lst = list_reverse(new_lst);
  lst1 = list_reverse(lst1);
  iterator = lst1;
  while (iterator) {
    gptr val = iterator->data;
    new_lst = list_push(new_lst, val);
    iterator = iterator->next;
  }
  lst1 = list_reverse(lst1);
  return new_lst;
}

gptr list_nth(list lst, int n) {
  int idx = 0;
  while (lst) {
    if (n == idx) { return lst->data; }
    idx++;
    lst = lst->next;
  }
  return NULL;
}

gptr list_first(list lst) {
  return lst?lst->data:NULL;
}

list list_rest(list lst) {
  if (lst) {
    return lst->next;
  } else {
    return NULL;
  }
}

list list_copy(list lst) {
  if (!lst) { return NULL; }
  list new_lst = NULL;
  while (lst) {
    new_lst = list_push(new_lst, lst->data);
    lst = lst->next;
  }
  return new_lst;
}
//传入查询函数 查询
long list_findf(list lst, char *keystring, char *(*fn)(gptr)) {
  /* handle empty case */
  if (!lst) {
    return -1;
  }
  list current = lst;
  while (current) {
    /* apply fn to the data to get a string */
    char *item = fn(current->data);
    if (strcmp(keystring, item) == 0) {
      return (current - lst); /* return the index of the first match */
    } else {
      current = current->next; /* skip the next item in the list to*/
    }
  }
  return -1; /* not found */
}
// end list.c
// hashmap.c
// hashmap hashmap_make(char* keystring, gptr data_ptr) {
//   list map = list_make(data_ptr);
//   map = list_push(map, keystring);
//   return map;
// }

hashmap hashmap_put(hashmap map, char *keystring, gptr data_ptr) {
  map = list_push(map, data_ptr);
  map = list_push(map, keystring);
  return map;
}

gptr hashmap_get(hashmap map, char *keystring) {
  /* handle empty case */
  if (!map) {
    return NULL;
  }
  list lst = map;
  while (lst) {
    if (strcmp(keystring, (char *)lst->data) == 0) {
      return (lst->next)
          ->data; /* return next item in list which is the value */
    } else {
      lst = (lst->next)->next; /* skip the next item in the list to get to the
                                  next key */
    }
  }
  return NULL; /* not found */
}

gptr hashmap_getf(hashmap map, char *keystring, char *(*fn)(gptr)) {
  /* handle empty case */
  if (!map) {
    return NULL;
  }
  list lst = map;
  while (lst) {
    /* apply fn to the data to get a string */
    char *item = fn(lst->data);
    if (strcmp(keystring, item) == 0) {
      return (lst->next)
          ->data; /* return next item in list which is the value */
    } else {
      lst = (lst->next)->next; /* skip the next item in the list to get to the
                                  next key */
    }
  }
  return NULL; /* not found */
}

hashmap hashmap_updatef(hashmap map, char *keystring, gptr value,
                        char *(*fn)(gptr)) {
  /* handle empty case */
  if (!map) {
    return NULL;
  }
  list lst = map;
  while (lst) {
    /* apply fn to the data to get a string */
    char *item = fn(lst->data);
    if (strcmp(keystring, item) == 0) {
      (lst->next)->data =
          value;  /* update the next item in list which is the value */
      return map; /* update made */
    } else {
      lst = (lst->next)->next; /* skip the next item in the list to get to the
                                  next key */
    }
  }
  return NULL; /* no update */
}
// end hashmap.c
// type.c

maltype THE_TRUE = {MALTYPE_TRUE, 0, 0, {0}};
maltype THE_FALSE = {MALTYPE_FALSE, 0, 0, {0}};
maltype THE_NIL = {MALTYPE_NIL, 0, 0, {0}};

inline int is_sequential(maltype *val) {
  return (val->type == MALTYPE_LIST || val->type == MALTYPE_VECTOR);
}

inline int is_self_evaluating(maltype *val) {
  return (val->type == MALTYPE_KEYWORD || val->type == MALTYPE_INTEGER ||
          val->type == MALTYPE_FLOAT || val->type == MALTYPE_STRING ||
          val->type == MALTYPE_TRUE || val->type == MALTYPE_FALSE ||
          val->type == MALTYPE_NIL);
}

inline int is_list(maltype *val) { return (val->type == MALTYPE_LIST); }

inline int is_vector(maltype *val) { return (val->type == MALTYPE_VECTOR); }

inline int is_hashmap(maltype *val) { return (val->type == MALTYPE_HASHMAP); }

inline int is_nil(maltype *val) { return (val->type == MALTYPE_NIL); }

inline int is_string(maltype *val) { return (val->type == MALTYPE_STRING); }

inline int is_integer(maltype *val) { return (val->type == MALTYPE_INTEGER); }

inline int is_float(maltype *val) { return (val->type == MALTYPE_FLOAT); }

inline int is_number(maltype *val) {
  return (val->type == MALTYPE_INTEGER || val->type == MALTYPE_FLOAT);
}

inline int is_true(maltype *val) { return (val->type == MALTYPE_TRUE); }

inline int is_false(maltype *val) { return (val->type == MALTYPE_FALSE); }

inline int is_symbol(maltype *val) { return (val->type == MALTYPE_SYMBOL); }

inline int is_keyword(maltype *val) { return (val->type == MALTYPE_KEYWORD); }

inline int is_atom(maltype *val) { return (val->type == MALTYPE_ATOM); }

inline int is_error(maltype *val) { return (val->type == MALTYPE_ERROR); }

inline int is_callable(maltype *val) {
  return (val->type == MALTYPE_FUNCTION || val->type == MALTYPE_CLOSURE);
}

inline int is_function(maltype *val) { return (val->type == MALTYPE_FUNCTION); }

inline int is_closure(maltype *val) { return (val->type == MALTYPE_CLOSURE); }

inline int is_macro(maltype *val) { return (val->is_macro); }

maltype *make_symbol(char *value) {
  maltype *mal_val = GC_MALLOC(sizeof(*mal_val));
  mal_val->type = MALTYPE_SYMBOL;
  mal_val->value.mal_symbol = value;
  mal_val->metadata = NULL;
  return mal_val;
}

maltype *make_integer(long value) {
  maltype *mal_val = GC_MALLOC(sizeof(*mal_val));
  mal_val->type = MALTYPE_INTEGER;
  mal_val->value.mal_integer = value;
  mal_val->metadata = NULL;
  return mal_val;
}

maltype *make_float(double value) {
  maltype *mal_val = GC_MALLOC(sizeof(*mal_val));
  mal_val->type = MALTYPE_FLOAT;
  mal_val->value.mal_float = value;
  mal_val->metadata = NULL;
  return mal_val;
}

maltype *make_keyword(char *value) {
  maltype *mal_val = GC_MALLOC(sizeof(*mal_val));
  mal_val->type = MALTYPE_KEYWORD;
  mal_val->value.mal_keyword = value;
  mal_val->metadata = NULL;
  return mal_val;
}

maltype *make_string(char *value) {
  maltype *mal_val = GC_MALLOC(sizeof(*mal_val));
  mal_val->type = MALTYPE_STRING;
  mal_val->value.mal_string = value;
  mal_val->metadata = NULL;
  return mal_val;
}

maltype *make_list(list value) {
  maltype *mal_val = GC_MALLOC(sizeof(*mal_val));
  mal_val->type = MALTYPE_LIST;
  mal_val->value.mal_list = value;
  mal_val->metadata = NULL;
  return mal_val;
}

maltype *make_vector(list value) {
  maltype *mal_val = GC_MALLOC(sizeof(*mal_val));
  mal_val->type = MALTYPE_VECTOR;
  mal_val->value.mal_list = value;
  mal_val->metadata = NULL;
  return mal_val;
}

maltype *make_hashmap(list value) {
  maltype *mal_val = GC_MALLOC(sizeof(*mal_val));
  mal_val->type = MALTYPE_HASHMAP;
  mal_val->value.mal_list = value;
  mal_val->metadata = NULL;
  return mal_val;
}

maltype *make_atom(maltype *value) {
  maltype *mal_val = GC_MALLOC(sizeof(*mal_val));
  mal_val->type = MALTYPE_ATOM;
  mal_val->value.mal_atom = value;
  mal_val->metadata = NULL;
  return mal_val;
}

maltype *make_function(maltype *(*fn)(list args)) {
  maltype *mal_val = GC_MALLOC(sizeof(*mal_val));
  mal_val->type = MALTYPE_FUNCTION;
  mal_val->value.mal_function = fn;
  mal_val->is_macro = 0;
  mal_val->metadata = NULL;
  return mal_val;
}

maltype *make_closure(Env *env, maltype *parameters, maltype *definition,
                      maltype *more_symbol) {
  maltype *mal_val = GC_MALLOC(sizeof(*mal_val));
  mal_val->type = MALTYPE_CLOSURE;
  mal_val->metadata = NULL;

  /* Allocate memory for embedded struct */
  MalClosure *mc = GC_MALLOC(sizeof(*mc));
  mc->env = env;
  mc->parameters = parameters;
  mc->definition = definition;
  mc->more_symbol = more_symbol;

  mal_val->is_macro = 0;
  mal_val->value.mal_closure = mc;
  return mal_val;
}

inline maltype *make_true() { return &THE_TRUE; }

inline maltype *make_false() { return &THE_FALSE; }

inline maltype *make_nil() { return &THE_NIL; }

maltype *make_error(char *msg) {
  maltype *mal_string = GC_MALLOC(sizeof(*mal_string));
  mal_string->type = MALTYPE_STRING;
  mal_string->value.mal_string = msg;

  maltype *mal_val = GC_MALLOC(sizeof(*mal_val));
  mal_val->type = MALTYPE_ERROR;
  mal_val->value.mal_error = mal_string;
  mal_val->metadata = NULL;
  return mal_val;
}

maltype *make_error_fmt(char *fmt, ...) {
  va_list argptr;
  va_start(argptr, fmt);

  char *buffer = GC_MALLOC(sizeof(*buffer) * ERROR_BUFFER_SIZE);

  long n = vsnprintf(buffer, ERROR_BUFFER_SIZE, fmt, argptr);
  va_end(argptr);

  if (n > ERROR_BUFFER_SIZE) {
    va_start(argptr, fmt);

    buffer = GC_REALLOC(buffer, sizeof(*buffer) * n);
    vsnprintf(buffer, n, fmt, argptr);

    va_end(argptr);
  }
  return make_error(buffer);
}

maltype *wrap_error(maltype *value) {
  maltype *mal_error = GC_MALLOC(sizeof(*mal_error));
  mal_error->type = MALTYPE_ERROR;
  mal_error->metadata = NULL;
  mal_error->value.mal_error = value;
  return mal_error;
}

maltype *copy_type(maltype *value) {
  maltype *new_val = GC_MALLOC(sizeof(*new_val));
  new_val->type = value->type;
  new_val->is_macro = value->is_macro;
  new_val->value = value->value;
  new_val->metadata = value->metadata;
  return new_val;
}

// end type.c
// reader.c

Reader *reader_make(long token_capacity) {
  Reader *reader = GC_MALLOC(sizeof(*reader));
  reader->max_tokens = token_capacity;
  reader->position = 0;
  reader->token_count = 0;
  reader->token_data = GC_MALLOC(sizeof(Token *) * token_capacity);
  reader->error = NULL;
  return reader;
}

Reader *reader_append(Reader *reader, Token *token) {
  if (reader->token_count < reader->max_tokens) {
    reader->token_data[reader->token_count] = token;
    reader->token_count++;
  } else {
    /* TODO: expand the storage more intelligently */
    reader->max_tokens *= 2;
    reader = GC_REALLOC(reader, sizeof(*reader) * reader->max_tokens);
    reader->token_data[reader->token_count] = token;
    reader->token_count++;
  }
  return reader;
}

Token *reader_peek(const Reader *reader) {
  return (reader->token_data[reader->position]);
}

Token *reader_next(Reader *reader) {
  Token *tok = reader->token_data[reader->position];
  if (reader->position == -1) {
    return NULL;
  } else if (reader->position < reader->token_count) {
    (reader->position)++;
    return tok;
  } else {
    reader->position = -1;
    return tok;
  }
}

/* NOTE: needed for debugging the reader only */
void reader_print(Reader *reader) {
  Token *tok;
  //需要-1 不然报 Segmentation Fault
  for (long i = 0; i < reader->token_count - 1; i++) {
    tok = reader_next(reader);
    switch (tok->type) {
    case TOKEN_SPECIAL_CHARACTER:
      printf("special character: %s", tok->data);
      break;
    case TOKEN_STRING:
      printf("string: %s", tok->data);
      break;
    case TOKEN_INTEGER:
      printf("integer: %s", tok->data);
      break;
    case TOKEN_FLOAT:
      printf("float: %s", tok->data);
      break;
    case TOKEN_SYMBOL:
      printf("symbol: %s", tok->data);
      break;
    case TOKEN_COMMENT:
      printf("comment: \"%s\"", tok->data);
      break;
    case TOKEN_KEYWORD:
      printf("keyword: %s", tok->data);
      break;
    case TOKEN_TRUE:
      printf("true: %s", tok->data);
      break;
    case TOKEN_FALSE:
      printf("false: %s", tok->data);
      break;
    case TOKEN_NIL:
      printf("nil: %s", tok->data);
      break;
    }
    printf("\n");
    /* print an error for any tokens with an error string */
    tok->error ? printf(" - %s", tok->error) : 0;
  }
  reader->position = 0; //恢复
}

maltype *read_str(char *token_string) {
  Reader *reader = tokenize(token_string);
  if (reader->error) {
    return make_error_fmt("Reader error: %s", reader->error);
  } else if (reader->token_count == 0) {
    return make_nil();
  } else {
    reader_print(reader);
    return read_form(reader);
  }
}
// 2022年4月30日 20点37分
/* allocate enough space for a Reader
TODO: over-allocates space
*/
Reader *tokenize(char *token_string) {
  Reader *reader = reader_make(strlen(token_string));
  for (char *next = token_string; *next != '\0';) {
    Token *token = NULL;
    //下面的break 注意是 switch 内的.不是for 的.
    switch (*next) {
    case ' ':
    case ',':  /* skip whitespace */
    case 0x0A: /* newline */
      next++;
      token = NULL; /* no token for whitespace */
      break;
      /* single character token */
    case '[':
    case '\\':
    case ']':
    case '{':
    case '}':
    case '(':
    case ')':
    case '\'':
    case '@':
    case '`':
    case '^':
      next = read_fixed_length_token(next, &token, 1);
      break;
      /* single or double character token */
    case '~':
      if (*(next + 1) == '@') {
        next = read_fixed_length_token(next, &token, 2);
      } else {
        next = read_fixed_length_token(next, &token, 1);
      }
      break;
      /* read string of characters within double quotes */
    case '"':
      next = read_string_token(next, &token);
      break;
      /* read a comment - all remaining input until newline */
    case ';':
      next = read_comment_token(next, &token);
      token = NULL; /* skip token for comments */
      break;
      /* read an integer */
    case '0':
    case '1':
    case '2':
    case '3':
    case '4':
    case '5':
    case '6':
    case '7':
    case '8':
    case '9':
      next = read_number_token(next, &token);
      //      next = read_integer_token(next, &token);
      break;
      /* integer may be prefixed with +/- */
    case '+':
    case '-':
      if (isdigit(next[1])) {
        next = read_number_token(next, &token);
        //      next = read_integer_token(next, &token);
      } else { /* if not digits it is part of a symbol */
        next = read_symbol_token(next, &token);
      }
      break;

      /* read keyword */
    case ':':
      next = read_keyword_token(next, &token);
      break;
      /* read anything else as a symbol */
    default:
      next = read_symbol_token(next, &token);
      break;
    }
    if (!token) {
      /* if no token was read (whitespace or comments) continue the loop */
      continue;
    } else {
      if (token->error) {
        /* report any errors with an early return */
        reader = reader_append(reader, token);
        reader->error = token->error;
        return reader;
      }
      printf("type: %d,data: %s \n", token->type, token->data);
      /* otherwise append the token and continue */
      reader = reader_append(reader, token);
    }
  }
  return reader;
}

char *read_fixed_length_token(char *current, Token **ptoken, int n) {
  *ptoken = token_allocate(current, n, TOKEN_SPECIAL_CHARACTER, NULL);
  return (current + n);
}

char *read_terminated_token(char *current, Token **ptoken, int token_type) {
  // printf("terminated: %s \n",current);
  static char *const terminating_characters = " ,[](){};\n";
  /* search for first terminating character */
  char *end = strpbrk(current, terminating_characters);
  /* if terminating character is not found it implies the end of the string */
  long token_length = !end ? strlen(current) : (end - current);
  /* next token starts with the terminating character */
  *ptoken = token_allocate(current, token_length, token_type, NULL);
  return (current + token_length);
}
// identifier
char *read_symbol_token(char *current, Token **ptoken) {
  char *next = read_terminated_token(current, ptoken, TOKEN_SYMBOL);
  /* check for reserved symbols */
  if (strcmp((*ptoken)->data, SYMBOL_NIL) == 0) {
    (*ptoken)->type = TOKEN_NIL;
  } else if (strcmp((*ptoken)->data, SYMBOL_TRUE) == 0) {
    (*ptoken)->type = TOKEN_TRUE;
  } else if (strcmp((*ptoken)->data, SYMBOL_FALSE) == 0) {
    (*ptoken)->type = TOKEN_FALSE;
  }
  /* TODO: check for invalid characters */
  return next;
}

char *read_keyword_token(char *current, Token **ptoken) {
  /* TODO: check for invalid characters */
  return read_terminated_token(current + 1, ptoken, TOKEN_KEYWORD);
}

char *read_number_token(char *current, Token **ptoken) {
  int has_decimal_point = 0;
  char *next = read_terminated_token(current, ptoken, TOKEN_INTEGER);
  long token_length = next - current;

  /* first char is either digit or '+' or '-'
     check the rest consists of valid characters */
  for (long i = 1; i < token_length; i++) {

    if ((*ptoken)->data[i] == '.' && has_decimal_point) {
      (*ptoken)->error = "Invalid character reading number";
      break;
    } else if ((*ptoken)->data[i] == '.' && !has_decimal_point) {
      has_decimal_point = 1;
      (*ptoken)->type = TOKEN_FLOAT;
      break;
    } else if (!(isdigit((*ptoken)->data[i]))) {
      (*ptoken)->error = "Invalid character reading number";
      break;
    }
  }
  return next;
}
//实现字符串搜索
char *ho_strpbrk(char *s1, char *s2) {
  const char *s;
  for (; *s1; s1++) {
    for (s = s2; *s; s++) {
      if (*s == *s1)
        return (char *)s1;
    }
  }
  return NULL;
}

char *read_string_token(char *current, Token **ptoken) {

  char *start, *end, *error = NULL;
  long token_length = 0;

  start = current + 1;

  while (1) {
    end = strchr(start, '"'); /* find the next " character */

    /* handle failure to find closing quotes - implies end of input has been
     * reached */
    if (!end) {
      end = current + strlen(current);
      token_length = strlen(current);

      error = "EOF reached with unterminated string";
      break;
    }
    /* if the character preceding the " is a '\' character (escape), need to
       check if it is escaping the " and if it is then keep searching from the
       next character */
    else if (*(end - 1) == '\\') {

      char *back_ptr = end - 1;
      while (*back_ptr == '\\') {
        back_ptr--; /* back up to count the escape characters '\' */
      }

      long escape_chars = (end - 1) - back_ptr;

      if (escape_chars % 2 == 1) {
        /* odd number of '\' chars means " is not quoted */
        start = end + 1; /* so keep searching */
      } else {
        /* even number of '\' characters means we found the terminating quote
         * mark */
        token_length =
            (end - current - 1); /* quotes are excluded from string token */
        break;
      }
    } else {
      token_length =
          (end - current - 1); /* quotes are excluded from string token */
      break;
    }
  }

  char *unescaped_string = unescape_string(current + 1, token_length);
  *ptoken = token_allocate(unescaped_string, strlen(unescaped_string),
                           TOKEN_STRING, error);

  return (end + 1);
}

char *read_comment_token(char *current, Token **ptoken) {
  /* comment includes all remaining characters to the next newline */
  /* search for newline character */
  char *end = strchr(current, 0x0A);
  /* if newline is not found it implies the end of string is reached */
  long token_chars = !end ? strlen(current) : (end - current);
  *ptoken = token_allocate(current, token_chars, TOKEN_COMMENT, NULL);
  return (current + token_chars +
          1); /* next token starts with the char after the newline */
}
// read_form 的时候把 ` ' @ 等quote 规范成了(xxqutoe xxx)等标准的 list 形式.
// amtf
maltype *read_form(Reader *reader) {
  if (reader->token_count > 0) {
    Token *tok = reader_peek(reader);
    if (tok->type == TOKEN_SPECIAL_CHARACTER) {
      switch (tok->data[0]) {
      case '(':
        return read_list(reader);
        break;
      case '[':
        return read_vector(reader);
        break;
      case '{':
        return read_hashmap(reader);
        break;
      case '\'':
        /* create and return a MalType list (quote read_form) */
        return make_symbol_list(reader, SYMBOL_QUOTE);
        break;
      case '`':
        /* create and return a MalType list (quasiquote read_form) */
        return make_symbol_list(reader, SYMBOL_QUASIQUOTE);
        break;
      case '~':
        if (tok->data[1] == '@') {
          /* create and return a MalType list (splice-unquote read_form) */
          return make_symbol_list(reader, SYMBOL_SPLICE_UNQUOTE);
        } else {
          /* create and return a MalType list (unquote read_form) */
          return make_symbol_list(reader, SYMBOL_UNQUOTE);
        }
      case '@':
        /* create and return a MalType list (deref read_form) */
        return make_symbol_list(reader, SYMBOL_DEREF);
      case '^':
        /* create and return a MalType list (with-meta <second-form>
           <first-form> where first form should ne a metadata map and second
           form is somethingh that can have metadata attached */
        reader_next(reader);
        /* grab the components of the list */
        maltype *symbol = make_symbol(SYMBOL_WITH_META);
        maltype *first_form = read_form(reader);
        maltype *second_form = read_form(reader);
        /* push the symbol and the following forms onto a list */
        list lst = NULL;
        lst = list_push(lst, symbol);
        lst = list_push(lst, second_form);
        lst = list_push(lst, first_form);
        lst = list_reverse(lst);
        return make_list(lst);
      default:
        /* shouldn't happen */
        return make_error_fmt("Reader error: Unknown special character '%c'",
                              tok->data[0]);
      }
    } else { /* 当前char 不是 9种符号 ( { [ ' ` ~ @ ~@ ^ Not a special character
              */
      return read_atom(reader);
    }
  } else { /* no tokens */
    return NULL;
  }
}

maltype *read_list(Reader *reader) {
  maltype *retval = read_matched_delimiters(reader, '(', ')');
  if (is_error(retval)) {
    retval = make_error("Reader error: unbalanced parenthesis '()'");
  } else {
    retval->type = MALTYPE_LIST;
  }
  return retval;
}

maltype *read_vector(Reader *reader) {
  maltype *retval = read_matched_delimiters(reader, '[', ']');
  if (is_error(retval)) {
    retval = make_error("Reader error: unbalanced brackets '[]'");
  } else {
    retval->type = MALTYPE_VECTOR;
  }
  return retval;
}

maltype *read_hashmap(Reader *reader) {
  maltype *retval = read_matched_delimiters(reader, '{', '}');
  if (is_error(retval)) {
    retval = make_error("Reader error: unbalanced braces '{}'");
  } else if (list_count(retval->value.mal_list) % 2 != 0) {
    retval = make_error("Reader error: missing value in map literal");
  } else {
    retval->type = MALTYPE_HASHMAP;
  }
  return retval;
}
// 读取3个匹配的分隔符 () {} []
maltype *read_matched_delimiters(Reader *reader, char start_delimiter,
                                 char end_delimiter) {
  /* TODO: separate implementation of hashmap and vector */
  Token *tok = reader_next(reader);
  list lst = NULL;

  if (reader_peek(reader)->data[0] == end_delimiter) {
    reader_next(reader);
    return make_list(NULL);
  } else {
    while (tok->data[0] != end_delimiter) {

      maltype *val = read_form(reader); //递归 读取了
      lst = list_push(lst, (gptr)val);

      tok = reader_peek(reader);

      if (!tok) {
        /* unbalanced parentheses */
        return make_error("");
      }
    }
    reader_next(reader); //吃掉匹配的符号

    return make_list(list_reverse(lst));
  }
}

maltype *read_atom(Reader *reader) {
  Token *tok = reader_next(reader);
  switch (tok->type) {
  // case TOKEN_SPECIAL_CHARACTER:  //这里已经在上面处理过了. 不会进入到这个条件
  //   return make_symbol(tok->data);
  //   break;
  case TOKEN_COMMENT:
    return make_error("Error: comment found in token strea");
    break;
  case TOKEN_STRING:
    return make_string(tok->data);
    break;
  case TOKEN_INTEGER:
    return make_integer(strtol(tok->data, NULL, 10));
    break;
  case TOKEN_FLOAT:
    return make_float(atof(tok->data));
    break;
  case TOKEN_SYMBOL:
    return make_symbol(tok->data);
    break;
  case TOKEN_KEYWORD:
    return make_keyword(tok->data);
    break;
  case TOKEN_TRUE:
    return make_true();
    break;
  case TOKEN_FALSE:
    return make_false();
    break;
  case TOKEN_NIL:
    return make_nil();
    break;
  }
  return make_error("Reader error: Unknown atom type");
}

maltype *make_symbol_list(Reader *reader, char *symbol_name) {
  reader_next(reader);
  list lst = NULL;
  /* push the symbol and the following form onto the list */
  lst = list_push(lst, make_symbol(symbol_name));
  lst = list_push(lst, read_form(reader));
  return make_list(list_reverse(lst));
}

Token *token_allocate(char *str, long num_chars, int type, char *error) {
  /* allocate space for the string */
  char *data = GC_MALLOC(sizeof(*data) * num_chars +
                         1);     /* include space for null byte */
  strncpy(data, str, num_chars); /* copy num_chars characters into data */
  data[num_chars] = '\0';        /* manually add the null byte */
  /* allocate space for the token struct */
  Token *token = GC_MALLOC(sizeof(*token));
  token->data = data;
  token->type = type;
  token->error = error;
  return token;
}

char *unescape_string(char *str, long length) {
  char *dest = GC_MALLOC(sizeof(*dest) * length + 1);
  long j = 0;
  for (long i = 0; i < length; i++) {
    /* look for the quoting character */
    if (str[i] == '\\') {
      switch (str[i + 1]) {
        /* replace '\"' with normal '"' */
      case '"':
        dest[j++] = '"';
        i++; /* skip extra char */
        break;
        /* replace '\n' with newline 0x0A */
      case 'n':
        dest[j++] = 0x0A;
        i++; /* skip extra char */
        break;
        /* replace '\\' with '\' */
      case '\\':
        dest[j++] = '\\';
        i++; /* skip extra char */
        break;
      default:
        /* just a '\' symbol so copy it */
        dest[j++] = '\\';
      }
    } else { /* not a quote so copy it */
      dest[j++] = str[i];
    }
  }
  dest[j] = '\0';
  return dest;
}

// end reader.c

// env.c
/* Note: caller must make sure enough exprs to match symbols */
Env *env_make(Env *outer, list symbol_list, list exprs_list,
              maltype *more_symbol) {

  Env *env = GC_MALLOC(sizeof(*env));
  env->outer = outer;
  env->data = NULL;

  while (symbol_list) {
    env = env_set(env, symbol_list->data, exprs_list->data);
    symbol_list = symbol_list->next;
    exprs_list = exprs_list->next;
  }

  /* set the 'more' symbol if there is one */
  if (more_symbol) {
    env = env_set(env, more_symbol, make_list(exprs_list));
  }
  return env;
}

Env *env_set(Env *current, maltype *symbol, maltype *value) {
  current->data = hashmap_put(current->data, symbol->value.mal_symbol, value);
  return current;
}

Env *env_find(Env *current, maltype *symbol) {
  maltype *val = hashmap_get(current->data, symbol->value.mal_symbol);
  if (val) {
    return current;
  } else if (current->outer) {
    return env_find(current->outer, symbol);
  } else {
    return NULL; /* not found */
  }
}

maltype *env_get(Env *current, maltype *symbol) {

  Env *env = env_find(current, symbol);

  if (env) {
    return hashmap_get(env->data, symbol->value.mal_symbol);
  } else {
    return make_error_fmt("'%s' not found", symbol->value.mal_symbol);
  }
}

Env *env_set_C_fn(Env *current, char *symbol_name, maltype *(*fn)(list)) {
  return env_set(current, make_symbol(symbol_name), make_function(fn));
}
// end env.c

// core.c
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

// ns

maltype *get_type(list node) {
  printf("ci: %p \n", node);
  maltype *mt = node->data;
  //  printf("ci2: %d \n",val->type);
  //  printf("ci2: %s \n",val->value);
  //  mal_prn(val);
  return make_integer(mt->type);
}

/* only needed for ffi */
#ifdef WITH_FFI
MalType *mal_dot(list);
#endif

ns *ns_make_core() {
  ns *core = GC_MALLOC(sizeof(*core));

  hashmap core_functions = NULL;
  /* extend */
  core_functions = hashmap_put(core_functions, "type", get_type);

  /* arithmetic */
  core_functions = hashmap_put(core_functions, "+", mal_add);
  core_functions = hashmap_put(core_functions, "-", mal_sub);
  core_functions = hashmap_put(core_functions, "*", mal_mul);
  core_functions = hashmap_put(core_functions, "/", mal_div);

  /* strings */
  core_functions = hashmap_put(core_functions, "prn", mal_prn);
  core_functions = hashmap_put(core_functions, "pr-str", mal_pr_str);
  core_functions = hashmap_put(core_functions, "str", mal_str);
  core_functions = hashmap_put(core_functions, "println", mal_println);
  core_functions = hashmap_put(core_functions, "read-string", mal_read_string);

  /* files */
  core_functions = hashmap_put(core_functions, "slurp", mal_slurp);

  /* lists */
  core_functions = hashmap_put(core_functions, "list", mal_list);
  core_functions =
      hashmap_put(core_functions, "empty?", mal_empty_questionmark);
  core_functions = hashmap_put(core_functions, "count", mal_count);
  core_functions = hashmap_put(core_functions, "cons", mal_cons);
  core_functions = hashmap_put(core_functions, "concat", mal_concat);
  core_functions = hashmap_put(core_functions, "nth", mal_nth);
  core_functions = hashmap_put(core_functions, "first", mal_first);
  core_functions = hashmap_put(core_functions, "rest", mal_rest);

  /* predicates */
  core_functions = hashmap_put(core_functions, "=", mal_equals);
  core_functions = hashmap_put(core_functions, "<", mal_lessthan);
  core_functions = hashmap_put(core_functions, "<=", mal_lessthanorequalto);
  core_functions = hashmap_put(core_functions, ">", mal_greaterthan);
  core_functions = hashmap_put(core_functions, ">=", mal_greaterthanorequalto);

  core_functions = hashmap_put(core_functions, "list?", mal_list_questionmark);
  core_functions = hashmap_put(core_functions, "nil?", mal_nil_questionmark);
  core_functions = hashmap_put(core_functions, "true?", mal_true_questionmark);
  core_functions =
      hashmap_put(core_functions, "false?", mal_false_questionmark);
  core_functions =
      hashmap_put(core_functions, "symbol?", mal_symbol_questionmark);
  core_functions =
      hashmap_put(core_functions, "keyword?", mal_keyword_questionmark);
  core_functions =
      hashmap_put(core_functions, "vector?", mal_vector_questionmark);
  core_functions =
      hashmap_put(core_functions, "sequential?", mal_sequential_questionmark);
  core_functions = hashmap_put(core_functions, "map?", mal_map_questionmark);
  core_functions =
      hashmap_put(core_functions, "string?", mal_string_questionmark);
  core_functions =
      hashmap_put(core_functions, "number?", mal_number_questionmark);
  core_functions = hashmap_put(core_functions, "fn?", mal_fn_questionmark);
  core_functions =
      hashmap_put(core_functions, "macro?", mal_macro_questionmark);

  /* atoms */
  core_functions = hashmap_put(core_functions, "atom", mal_atom);
  core_functions = hashmap_put(core_functions, "atom?", mal_atom_questionmark);
  core_functions = hashmap_put(core_functions, "deref", mal_deref);
  core_functions = hashmap_put(core_functions, "reset!", mal_reset_bang);
  core_functions = hashmap_put(core_functions, "swap!", mal_swap_bang);

  /* other */
  core_functions = hashmap_put(core_functions, "throw", mal_throw);
  core_functions = hashmap_put(core_functions, "apply", mal_apply);
  core_functions = hashmap_put(core_functions, "map", mal_map);

  core_functions = hashmap_put(core_functions, "symbol", mal_symbol);
  core_functions = hashmap_put(core_functions, "keyword", mal_keyword);
  core_functions = hashmap_put(core_functions, "vec", mal_vec);
  core_functions = hashmap_put(core_functions, "vector", mal_vector);
  core_functions = hashmap_put(core_functions, "hash-map", mal_hash_map);

  /* hash-maps */
  core_functions =
      hashmap_put(core_functions, "contains?", mal_contains_questionmark);
  core_functions = hashmap_put(core_functions, "assoc", mal_assoc);
  core_functions = hashmap_put(core_functions, "dissoc", mal_dissoc);
  core_functions = hashmap_put(core_functions, "get", mal_get);
  core_functions = hashmap_put(core_functions, "keys", mal_keys);
  core_functions = hashmap_put(core_functions, "vals", mal_vals);

  /* misc */
  core_functions = hashmap_put(core_functions, "time-ms", mal_time_ms);
  core_functions = hashmap_put(core_functions, "conj", mal_conj);
  core_functions = hashmap_put(core_functions, "seq", mal_seq);
  core_functions = hashmap_put(core_functions, "meta", mal_meta);
  core_functions = hashmap_put(core_functions, "with-meta", mal_with_meta);

/* only needed for ffi */
#ifdef WITH_FFI
  core_functions = hashmap_put(core_functions, ".", mal_dot);
#endif

  core->mappings = core_functions;
  return core;
}

/* core function definitons */

maltype *mal_add(list args) {
  /* Accepts any number of arguments */
  int return_float = 0;
  long i_sum = 0;
  double r_sum = 0.0;

  while (args) {
    maltype *val = args->data;
    if (!is_number(val)) {
      return make_error("'+': expected numerical arguments");
    }

    if (is_integer(val) && !return_float) {
      i_sum = i_sum + val->value.mal_integer;
    } else if (is_integer(val)) {
      r_sum = (double)i_sum + r_sum + val->value.mal_integer;
      i_sum = 0;
    } else {
      r_sum = (double)i_sum + r_sum + val->value.mal_float;
      i_sum = 0;
      return_float = 1;
    }
    args = args->next;
  }

  if (return_float) {
    return make_float(r_sum);
  } else {
    return make_integer(i_sum);
  }
}

maltype *mal_sub(list args) {
  /* Accepts any number of arguments */
  int return_float = 0;
  long i_sum = 0;
  double r_sum = 0.0;
  if (args) {
    maltype *val = args->data;
    args = args->next;
    if (!is_number(val)) {
      return make_error_fmt("'-': expected numerical arguments");
    }
    if (is_integer(val)) {
      i_sum = val->value.mal_integer;
    } else {
      r_sum = val->value.mal_float;
      return_float = 1;
    }

    while (args) {
      val = args->data;
      if (!is_number(val)) {
        return make_error_fmt("'-': expected numerical arguments");
      }

      if (is_integer(val) && !return_float) {
        i_sum = i_sum - val->value.mal_integer;
      } else if (is_integer(val)) {
        r_sum = (double)i_sum + r_sum - (double)val->value.mal_integer;
        i_sum = 0;
      } else {
        r_sum = (double)i_sum + r_sum - val->value.mal_float;
        i_sum = 0;
        return_float = 1;
      }
      args = args->next;
    }
  }

  if (return_float) {
    return make_float(r_sum);
  } else {
    return make_integer(i_sum);
  }
}

/* Accepts any number of arguments */
maltype *mal_mul(list args) {
  int return_float = 0;
  long i_product = 1;
  double r_product = 1.0;
  while (args) {
    maltype *val = args->data;
    if (!is_number(val)) {
      return make_error_fmt("'*': expected numerical arguments");
    }
    if (is_integer(val) && !return_float) {
      i_product *= val->value.mal_integer;
    } else if (is_integer(val)) {
      r_product *= (double)val->value.mal_integer;
      r_product *= (double)i_product;
      i_product = 1;
    } else {
      r_product *= (double)i_product;
      r_product *= val->value.mal_float;
      i_product = 1;
      return_float = 1;
    }
    args = args->next;
  }

  if (return_float) {
    return make_float(r_product);
  } else {
    return make_integer(i_product);
  }
}

maltype *mal_div(list args) {
  int return_float = 0;
  long i_product = 1;
  double r_product = 1.0;

  if (args) {
    maltype *val = args->data;
    if (!is_number(val)) {
      return make_error_fmt("'/': expected numerical arguments");
    }
    if (is_integer(val)) {
      i_product = val->value.mal_integer;
    } else {
      r_product = val->value.mal_float;
      return_float = 1;
    }
    args = args->next;
    while (args) {
      val = args->data;
      if (!is_number(val)) {
        return make_error_fmt("'/': expected numerical arguments");
      }
      /* integer division */
      if (is_integer(val) && !return_float) {
        i_product /= val->value.mal_integer;
      } /* promote integer to double */
      else if (is_integer(val)) {
        if (i_product != 1) {
          r_product = (double)i_product / (double)val->value.mal_integer;
          i_product = 1;
        } else {
          r_product /= (double)val->value.mal_integer;
        }
      }
      /* double division */
      else {
        return_float = 1;
        if (i_product != 1) {
          r_product = (double)i_product / val->value.mal_float;
          i_product = 1;
        } else {
          r_product /= val->value.mal_float;
        }
      }
      args = args->next;
    }
  }

  if (return_float) {
    return make_float(r_product);
  } else {
    return make_integer(i_product);
  }
}

maltype *mal_lessthan(list args) {
  if (!args || !args->next || args->next->next) {
    return make_error_fmt("'<': expected exactly two arguments");
  }
  maltype *first_val = args->data;
  maltype *second_val = args->next->data;
  if (!is_number(first_val) || !is_number(second_val)) {
    return make_error_fmt("'<': expected numerical arguments");
  }

  int cmp = 0;

  if (is_integer(first_val) && is_integer(second_val)) {
    cmp = (first_val->value.mal_integer < second_val->value.mal_integer);
  } else if (is_integer(first_val) && is_float(second_val)) {
    cmp = (first_val->value.mal_integer < second_val->value.mal_float);
  } else if (is_float(first_val) && is_integer(second_val)) {
    cmp = (first_val->value.mal_float < second_val->value.mal_integer);
  } else if (is_float(first_val) && is_float(second_val)) {
    cmp = (first_val->value.mal_float < second_val->value.mal_float);
  } else {
    /* shouldn't happen unless new numerical type is added */
    return make_error_fmt("'<': unknown numerical type");
  }
  if (cmp) {
    return make_true();
  } else {
    return make_false();
  }
}

maltype *mal_lessthanorequalto(list args) {
  if (!args || !args->next || args->next->next) {
    return make_error_fmt("'<=': expected exactly two arguments");
  }
  maltype *first_val = args->data;
  maltype *second_val = args->next->data;
  if (!is_number(first_val) || !is_number(second_val)) {
    return make_error_fmt("'<=': expected numerical arguments");
  }

  int cmp = 0;
  if (is_integer(first_val) && is_integer(second_val)) {
    cmp = (first_val->value.mal_integer <= second_val->value.mal_integer);
  } else if (is_integer(first_val) && is_float(second_val)) {
    cmp = (first_val->value.mal_integer <= second_val->value.mal_float);
  } else if (is_float(first_val) && is_integer(second_val)) {
    cmp = (first_val->value.mal_float <= second_val->value.mal_integer);
  } else if (is_float(first_val) && is_float(second_val)) {
    cmp = (first_val->value.mal_float < second_val->value.mal_float);
  } else {
    /* shouldn't happen unless new numerical type is added */
    return make_error_fmt("'<=': unknown numerical type");
  }

  if (cmp) {
    return make_true();
  } else {
    return make_false();
  }
}

maltype *mal_greaterthan(list args) {
  if (!args || !args->next || args->next->next) {
    return make_error_fmt("'>': expected exactly two arguments");
  }
  maltype *first_val = args->data;
  maltype *second_val = args->next->data;

  if (!is_number(first_val) || !is_number(second_val)) {
    return make_error_fmt("'>': expected numerical arguments");
  }
  int cmp = 0;
  if (is_integer(first_val) && is_integer(second_val)) {
    cmp = (first_val->value.mal_integer > second_val->value.mal_integer);
  } else if (is_integer(first_val) && is_float(second_val)) {
    cmp = (first_val->value.mal_integer > second_val->value.mal_float);
  } else if (is_float(first_val) && is_integer(second_val)) {
    cmp = (first_val->value.mal_float > second_val->value.mal_integer);
  } else if (is_float(first_val) && is_float(second_val)) {
    cmp = (first_val->value.mal_float > second_val->value.mal_float);
  } else {
    /* shouldn't happen unless new numerical type is added */
    return make_error_fmt("'>': unknown numerical type");
  }

  if (cmp) {
    return make_true();
  } else {
    return make_false();
  }
}

maltype *mal_greaterthanorequalto(list args) {
  if (!args || !args->next || args->next->next) {
    return make_error_fmt("'>=': expected exactly two arguments");
  }

  maltype *first_val = args->data;
  maltype *second_val = args->next->data;

  if (!is_number(first_val) || !is_number(second_val)) {
    return make_error_fmt("'>=': expected numerical arguments");
  }
  int cmp = 0;
  if (is_integer(first_val) && is_integer(second_val)) {
    cmp = (first_val->value.mal_integer >= second_val->value.mal_integer);
  } else if (is_integer(first_val) && is_float(second_val)) {
    cmp = (first_val->value.mal_integer >= second_val->value.mal_float);
  } else if (is_float(first_val) && is_integer(second_val)) {
    cmp = (first_val->value.mal_float >= second_val->value.mal_integer);
  } else if (is_float(first_val) && is_float(second_val)) {
    cmp = (first_val->value.mal_float >= second_val->value.mal_float);
  } else {
    /* shouldn't happen unless new numerical type is added */
    return make_error_fmt("'>=': unknown numerical type");
  }

  if (cmp) {
    return make_true();
  } else {
    return make_false();
  }
}

maltype *mal_equals(list args) {
  if (!args || !args->next || args->next->next) {
    return make_error_fmt("'=': expected exactly two arguments");
  }

  maltype *first_val = args->data;
  maltype *second_val = args->next->data;

  if (is_sequential(first_val) && is_sequential(second_val)) {
    return equal_lists(first_val, second_val);
  } else if (first_val->type != second_val->type) {
    return make_false();
  } else {

    switch (first_val->type) {

    case MALTYPE_INTEGER:

      if (first_val->value.mal_integer == second_val->value.mal_integer) {
        return make_true();
      } else {
        return make_false();
      }
      break;

    case MALTYPE_FLOAT:

      if (first_val->value.mal_float == second_val->value.mal_float) {
        return make_true();
      } else {
        return make_false();
      }
      break;

    case MALTYPE_SYMBOL:

      if (strcmp(first_val->value.mal_symbol, second_val->value.mal_symbol) ==
          0) {
        return make_true();
      } else {
        return make_false();
      }
      break;

    case MALTYPE_STRING:
      if (strcmp(first_val->value.mal_string, second_val->value.mal_string) ==
          0) {
        return make_true();
      } else {
        return make_false();
      }
      break;

    case MALTYPE_KEYWORD:
      if (strcmp(first_val->value.mal_keyword, second_val->value.mal_keyword) ==
          0) {
        return make_true();
      } else {
        return make_false();
      }
      break;

    case MALTYPE_HASHMAP:
      return equal_hashmaps(first_val, second_val);
      break;

    case MALTYPE_TRUE:
    case MALTYPE_FALSE:
    case MALTYPE_NIL:

      return make_true();
      break;

    case MALTYPE_FUNCTION:

      if (first_val->value.mal_function == second_val->value.mal_function) {
        return make_true();
      } else {
        return make_false();
      }
      break;

    case MALTYPE_CLOSURE:

      if (&first_val->value.mal_closure == &second_val->value.mal_closure) {
        return make_true();
      } else {
        return make_false();
      }
      break;
    }
  }
  return make_false();
}

maltype *mal_list(list args) { return make_list(args); }

maltype *mal_nth(list args) {
  if (!args || !args->next || args->next->next) {
    return make_error("'nth': Expected exactly two arguments");
  }
  maltype *lst = args->data;
  maltype *n = args->next->data;
  if (!is_sequential(lst)) {
    return make_error_fmt(
        "'nth': first argument is not a list or vector: '%s'\n",
        pr_str(lst, UNREADABLY));
  }
  if (!is_integer(n)) {
    return make_error_fmt("'nth': second argument is not an integer: '%s'\n",
                          pr_str(lst, UNREADABLY));
  }
  maltype *result = list_nth(lst->value.mal_list, n->value.mal_integer);
  if (result) {
    return result;
  } else {
    return make_error_fmt("'nth': index %s out of bounds for: '%s'\n",
                          pr_str(n, UNREADABLY), pr_str(lst, UNREADABLY));
  }
}

maltype *mal_first(list args) {
  if (!args || args->next) {
    return make_error("'first': expected exactly one argument");
  }
  maltype *lst = args->data;
  if (!is_sequential(lst) && !is_nil(lst)) {
    return make_error("'first': expected a list or vector");
  }
  maltype *result = list_first(lst->value.mal_list);
  if (result) {
    return result;
  } else {
    return make_nil();
  }
}

maltype *mal_rest(list args) {
  if (!args || args->next) {
    return make_error("'rest': expected exactly one argument");
  }
  maltype *lst = args->data;
  if (!is_sequential(lst) && !is_nil(lst)) {
    return make_error("'rest': expected a list or vector");
  }
  list result = list_rest(lst->value.mal_list);
  if (lst) {
    return make_list(result);
  } else {
    return make_nil();
  }
}

maltype *mal_cons(list args) {

  if (!args || (args->next && args->next->next)) {
    return make_error("'cons': Expected exactly two arguments");
  }

  maltype *lst = args->next->data;
  if (is_sequential(lst)) {
    return make_list(list_push(lst->value.mal_list, args->data));
  } else if (is_nil(lst)) {
    return make_list(list_push(NULL, args->data));
  } else {
    return make_error_fmt(
        "'cons': second argument is not a list or vector: '%s'\n",
        pr_str(lst, UNREADABLY));
  }
}

maltype *mal_concat(list args) {

  /* return an empty list for no arguments */
  if (!args) {
    return make_list(NULL);
  }

  list new_list = NULL;
  while (args) {

    maltype *val = args->data;

    /* skip nils */
    if (is_nil(val)) {
      args = args->next;
      continue;
    }
    /* concatenate lists and vectors */
    else if (is_sequential(val)) {

      list lst = val->value.mal_list;
      new_list = list_concatenate(new_list, lst);
      args = args->next;
    }
    /* raise an error for any non-sequence types */
    else {
      return make_error_fmt(
          "'concat': all arguments must be lists or vectors '%s'",
          pr_str(val, UNREADABLY));
    }
  }
  return make_list(new_list);
}

maltype *mal_count(list args) {

  if (args->next) {
    return make_error_fmt("'count': too many arguments");
  }

  maltype *val = args->data;
  if (!is_sequential(val) && !is_nil(val)) {
    return make_error_fmt("'count': argument is not a list or vector: '%s'\n",
                          pr_str(val, UNREADABLY));
  }
  return make_integer(list_count(val->value.mal_list));
}

maltype *mal_list_questionmark(list args) {

  if (args->next) {
    return make_error_fmt("'list?': too many arguments");
  }

  maltype *val = args->data;
  if (is_list(val)) {
    return make_true();
  } else {
    return make_false();
  }
}

maltype *mal_empty_questionmark(list args) {

  if (args->next) {
    return make_error_fmt("'empty?': too many arguments");
  }

  maltype *val = args->data;
  if (!is_sequential(val)) {
    return make_error_fmt("'empty?': argument is not a list or vector: '%s'\n",
                          pr_str(val, UNREADABLY));
  }

  if (!val->value.mal_list) {
    return make_true();
  } else {
    return make_false();
  }
}

maltype *mal_pr_str(list args) {
  /* Accepts any number and type of arguments */
  return as_str(args, READABLY, " ");
}

maltype *mal_str(list args) {
  /* Accepts any number and type of arguments */
  return as_str(args, UNREADABLY, "");
}

maltype *mal_prn(list args) {
  /* Accepts any number and type of arguments */
  return print(args, READABLY, " ");
}

maltype *mal_println(list args) {
  /* Accepts any number and type of arguments */
  return print(args, UNREADABLY, " ");
}

maltype *mal_read_string(list args) {

  if (!args || args->next) {
    return make_error_fmt("'read-string': expected exactly one argument");
  }

  maltype *val = args->data;
  if (!is_string(val)) {
    return make_error_fmt("'read-string': expected a string argument '%s'",
                          pr_str(val, UNREADABLY));
  }
  return read_str(val->value.mal_string);
}

maltype *mal_slurp(list args) {

  if (args->next) {
    return make_error_fmt("'slurp': too many arguments");
  }

  maltype *filename = args->data;
  if (!is_string(filename)) {
    return make_error_fmt("'slurp': expected a string argument");
  }

  long file_length = 0;
  FILE *file = fopen(filename->value.mal_string, "rb");

  if (!file) {
    return make_error_fmt("'slurp': file not found '%s'",
                          pr_str(filename, UNREADABLY));
  }

  fseek(file, 0, SEEK_END);
  file_length = ftell(file);
  fseek(file, 0, SEEK_SET);

  char *buffer = (char *)GC_MALLOC(sizeof(*buffer) * file_length + 1);
  if (file_length != fread(buffer, sizeof(*buffer), file_length, file)) {
    return make_error_fmt("'slurp': failed to read file '%s'",
                          pr_str(filename, UNREADABLY));
  }

  fclose(file);

  buffer[file_length] = '\0';
  return make_string(buffer);
}

maltype *mal_atom(list args) {

  if (!args || args->next) {
    return make_error_fmt("'atom': expected exactly one argument");
  }

  maltype *val = args->data;
  return make_atom(val);
}

maltype *mal_atom_questionmark(list args) {

  if (!args || args->next) {
    return make_error_fmt("'atom?': expected exactly one argument");
  }

  maltype *val = args->data;

  if (is_atom(val)) {
    return make_true();
  } else {
    return make_false();
  }
}

maltype *mal_deref(list args) {

  if (!args || args->next) {
    return make_error_fmt("'deref': expected exactly one argument");
  }

  maltype *val = args->data;

  if (!is_atom(val)) {
    return make_error_fmt("'deref': value is not an atom '%s'",
                          pr_str(val, UNREADABLY));
  }

  return val->value.mal_atom;
}

maltype *mal_reset_bang(list args) {

  if (!args || args->next->next) {
    return make_error_fmt("'reset!': expected exactly two arguments");
  }

  maltype *val = args->data;

  if (!is_atom(val)) {
    return make_error_fmt("'reset!': value is not an atom '%s'",
                          pr_str(val, UNREADABLY));
  }

  val->value.mal_atom = args->next->data;
  return args->next->data;
}

maltype *mal_swap_bang(list args) {

  maltype *val = args->data;

  if (!is_atom(val)) {
    return make_error_fmt("'swap!': first argument is not an atom '%s'",
                          pr_str(val, UNREADABLY));
  }

  maltype *fn = args->next->data;

  if (!is_callable(fn)) {
    return make_error_fmt("'swap!': second argument is not callable '%s'",
                          pr_str(fn, UNREADABLY));
  }

  list fn_args = args->next->next;
  fn_args = list_push(fn_args, val->value.mal_atom);

  maltype *result = apply(fn, fn_args);

  if (is_error(result)) {
    return result;
  } else {
    val->value.mal_atom = result;
    return result;
  }
}

maltype *mal_throw(list args) {

  if (!args || args->next) {
    return make_error_fmt("'throw': expected exactly one argument");
  }

  maltype *val = args->data;

  /* re-throw an existing exception */
  if (is_error(val)) {
    return val;
  }
  /* create a new exception */
  else {
    return wrap_error(val);
  }
}

maltype *mal_apply(list args) {

  if (!args || !args->next) {
    return make_error("'apply': expected at least two arguments");
  }

  maltype *func = args->data;

  if (!is_callable(func)) {
    return make_error("'apply': first argument must be callable");
  }

  /* assemble loose arguments */
  args = args->next;
  list lst = NULL;
  while (args->next) {
    lst = list_push(lst, args->data);
    args = args->next;
  }

  maltype *final = args->data;

  if (is_sequential(final)) {
    lst = list_concatenate(list_reverse(lst), final->value.mal_list);
  } else {
    lst = list_push(lst, final);
    lst = list_reverse(lst);
  }

  return apply(func, lst);
}

maltype *mal_map(list args) {

  if (!args || !args->next || args->next->next) {
    return make_error("'map': expected two arguments");
  }

  maltype *func = args->data;

  if (!is_callable(func)) {
    return make_error("'map': first argument must be a function");
  }

  maltype *arg = args->next->data;

  if (!is_sequential(arg)) {
    return make_error("'map': second argument must be a list or vector");
  }

  list arg_list = arg->value.mal_list;
  list result_list = NULL;

  while (arg_list) {

    maltype *result = apply(func, list_make(arg_list->data));

    /* early return if error */
    if (is_error(result)) {
      return result;
    } else {
      result_list = list_push(result_list, result);
    }
    arg_list = arg_list->next;
  }
  return make_list(list_reverse(result_list));
}

maltype *mal_nil_questionmark(list args) {

  if (!args || args->next) {
    return make_error("'nil?': expected a single argument");
  }

  maltype *val = args->data;

  if (is_nil(val)) {
    return make_true();
  } else {
    return make_false();
  }
}

maltype *mal_true_questionmark(list args) {

  if (!args || args->next) {
    return make_error("'true?': expected a single argument");
  }

  maltype *val = args->data;

  if (is_true(val)) {
    return make_true();
  } else {
    return make_false();
  }
}

maltype *mal_false_questionmark(list args) {

  if (!args || args->next) {
    return make_error("'false?': expected a single argument");
  }

  maltype *val = args->data;

  if (is_false(val)) {
    return make_true();
  } else {
    return make_false();
  }
}

maltype *mal_symbol_questionmark(list args) {

  if (!args || args->next) {
    return make_error("'symbol?': expected a single argument");
  }

  maltype *val = args->data;

  if (is_symbol(val)) {
    return make_true();
  } else {
    return make_false();
  }
}

maltype *mal_symbol(list args) {

  if (!args || args->next) {
    return make_error("'symbol': expected a single argument");
  }

  maltype *val = args->data;

  if (!is_string(val)) {
    return make_error("'symbol': expected a string argument");
  } else {
    return make_symbol(val->value.mal_string);
  }
}

maltype *mal_keyword(list args) {

  if (!args || args->next) {
    return make_error("'keyword': expected a single argument");
  }

  maltype *val = args->data;

  if (!is_string(val) && !is_keyword(val)) {
    return make_error("'keyword': expected a string argument");
  } else {
    return make_keyword(val->value.mal_string);
  }
}

maltype *mal_keyword_questionmark(list args) {

  if (!args || args->next) {
    return make_error("'keyword?': expected a single argument");
  }

  maltype *val = args->data;

  if (is_keyword(val)) {
    return make_true();
  } else {
    return make_false();
  }
}

maltype *mal_vec(list args) {

  /* Accepts a single argument */

  if (!args || args->next) {
    return make_error("'vec': expected a single argument");
  }

  maltype *val = args->data;

  if (!is_vector(val) && !is_list(val) && !is_hashmap(val)) {
    return make_error("'vec': expected a vector, list or hashmap");
  }

  maltype *new_val = copy_type(val);
  new_val->type = MALTYPE_VECTOR;

  return new_val;
}

maltype *mal_vector(list args) {
  /* Accepts any number and type of arguments */
  return make_vector(args);
}

maltype *mal_vector_questionmark(list args) {

  if (!args || args->next) {
    return make_error("'vector?': expected a single argument");
  }

  maltype *val = args->data;

  if (is_vector(val)) {
    return make_true();
  } else {
    return make_false();
  }
}

maltype *mal_sequential_questionmark(list args) {

  if (!args || args->next) {
    return make_error("'sequential?': expected a single argument");
  }

  maltype *val = args->data;

  if (is_sequential(val)) {
    return make_true();
  } else {
    return make_false();
  }
}

maltype *mal_hash_map(list args) {

  if (args && list_count(args) % 2 == 1) {
    return make_error(
        "'hashmap': odd number of arguments, expected key/value pairs");
  }

  list args_iterator = args;
  while (args_iterator) {

    maltype *val = args_iterator->data;

    if (!is_keyword(val) && !is_string(val) && !is_symbol(val)) {
      return make_error("'hashmap': keys must be keywords, symbols or strings");
    }
    args_iterator = args_iterator->next;
    args_iterator = args_iterator->next;
  }

  return make_hashmap(args);
}

maltype *mal_map_questionmark(list args) {

  if (!args || args->next) {
    return make_error("'map?': expected a single argument");
  }

  maltype *val = args->data;

  if (is_hashmap(val)) {
    return make_true();
  } else {
    return make_false();
  }
}

maltype *mal_get(list args) {
  /* TODO: implement a proper hashmap */

  if (!args || args->next->next) {
    return make_error("'get': expected exactly two arguments");
  }

  maltype *map = args->data;

  if (!is_hashmap(map) && !is_nil(map)) {
    return make_error("'get': expected a map for the first argument");
  }

  maltype *result =
      hashmap_getf(map->value.mal_list, get_fn(args->next->data), get_fn);

  if (!result) {
    return make_nil();
  }

  return result;
}

maltype *mal_contains_questionmark(list args) {

  if (!args || args->next->next) {
    return make_error("'contains?': expected exactly two arguments");
  }

  maltype *map = args->data;

  if (!is_hashmap(map)) {
    return make_error("'contains?': expected a map for the first argument");
  }

  maltype *result =
      hashmap_getf(map->value.mal_list, get_fn(args->next->data), get_fn);

  if (!result) {
    return make_false();
  } else {
    return make_true();
  }
}

maltype *mal_assoc(list args) {

  if (!args || !args->next || !args->next->next) {
    return make_error("'assoc': expected at least three arguments");
  }

  maltype *map = args->data;

  if (!is_hashmap(map)) {
    return make_error("'assoc': expected a map for the first argument");
  }

  if (list_count(args->next) % 2 != 0) {
    return make_error("'assoc': expected even number of key/value pairs");
  }

  list new_lst = list_reverse(list_copy(map->value.mal_list));
  args = args->next;

  while (args) {

    /* try to update copy in-place */
    hashmap result =
        hashmap_updatef(new_lst, get_fn(args->data), args->next->data, get_fn);

    if (result) {
      new_lst = result;
    }
    /* add a new key/value pair */
    else {
      new_lst = list_push(new_lst, args->next->data);
      new_lst = list_push(new_lst, args->data);
    }
    args = args->next->next;
  }
  return make_hashmap(new_lst);
}

maltype *mal_dissoc(list args) {

  if (!args || !args->next) {
    return make_error("'dissoc': expected at least two arguments");
  }

  maltype *map = args->data;

  if (!is_hashmap(map)) {
    return make_error("'dissoc': expected a map for the first argument");
  }

  list source_list = map->value.mal_list;
  list new_list = NULL;
  args = args->next;

  while (source_list) {

    list dis_args = args;
    long dis = 0;

    while (dis_args) {

      list tmp = NULL;
      tmp = list_push(tmp, source_list->data);
      tmp = list_push(tmp, dis_args->data);
      maltype *cmp = mal_equals(tmp);

      if (is_true(cmp)) {
        dis = 1;
        break;
      }
      dis_args = dis_args->next;
    }

    if (!dis) {
      new_list = list_push(new_list, source_list->data);
      new_list = list_push(new_list, source_list->next->data);
    }
    source_list = source_list->next->next;
  }

  return make_hashmap(list_reverse(new_list));
}

maltype *mal_keys(list args) {

  if (!args || args->next) {
    return make_error("'keys': expected exactly one argument");
  }

  maltype *map = args->data;

  if (!is_hashmap(map)) {
    return make_error("'keys': expected a map");
  }

  list lst = map->value.mal_list;
  if (!lst) {
    return make_list(NULL);
  }

  list result = list_make(lst->data);
  while (lst->next->next) {

    lst = lst->next->next;
    result = list_push(result, lst->data);
  }
  return make_list(result);
}

maltype *mal_vals(list args) {

  if (!args || args->next) {
    return make_error("'vals': expected exactly one argument");
  }

  maltype *map = args->data;

  if (!is_hashmap(map)) {
    return make_error("'vals': expected a map");
  }

  list lst = map->value.mal_list;
  if (!lst) {
    return make_list(NULL);
  }

  lst = lst->next;
  list result = list_make(lst->data);
  while (lst->next) {

    lst = lst->next->next;
    result = list_push(result, lst->data);
  }
  return make_list(result);
}

maltype *mal_string_questionmark(list args) {

  if (!args || args->next) {
    return make_error("'string?': expected a single argument");
  }

  maltype *val = args->data;

  if (is_string(val)) {
    return make_true();
  } else {
    return make_false();
  }
}

maltype *mal_number_questionmark(list args) {

  if (!args || args->next) {
    return make_error("'number?': expected a single argument");
  }

  maltype *val = args->data;

  if (is_number(val)) {
    return make_true();
  } else {
    return make_false();
  }
}

maltype *mal_fn_questionmark(list args) {

  if (!args || args->next) {
    return make_error("'fn?': expected a single argument");
  }

  maltype *val = args->data;

  if (is_callable(val) && !is_macro(val)) {
    return make_true();
  } else {
    return make_false();
  }
}

maltype *mal_macro_questionmark(list args) {

  if (!args || args->next) {
    return make_error("'macro?': expected a single argument");
  }

  maltype *val = args->data;

  if (is_macro(val)) {
    return make_true();
  } else {
    return make_false();
  }
}

maltype *mal_time_ms(list args) {

  struct timeval tv;
  gettimeofday(&tv, NULL);
  long ms = tv.tv_sec * 1000 + tv.tv_usec / 1000.0 + 0.5;

  return make_float(ms);
}

maltype *mal_conj(list args) {
  if (!args || !args->next) {
    return make_error("'conj': Expected at least two arguments");
  }
  maltype *lst = args->data;
  if (!is_sequential(lst)) {
    return make_error_fmt(
        "'conj': first argument is not a list or vector: '%s'\n",
        pr_str(lst, UNREADABLY));
  }
  list rest = args->next;
  if (is_list(lst)) {
    list new_lst = list_reverse(list_copy(lst->value.mal_list));
    while (rest) {
      new_lst = list_push(new_lst, rest->data);
      rest = rest->next;
    }
    return make_list(new_lst);
  } else /* is_vector(lst) */
  {

    list new_lst = list_copy(lst->value.mal_list);

    while (rest) {
      new_lst = list_push(new_lst, rest->data);
      rest = rest->next;
    }
    return make_vector(list_reverse(new_lst));
  }
}

maltype *mal_seq(list args) {
  if (!args || args->next) {
    return make_error("'seq': expected exactly one argument");
  }
  maltype *val = args->data;
  if (is_sequential(val)) {
    /* empy list or vector */
    if (!val->value.mal_list) {
      return make_nil();
    } else {
      return make_list(val->value.mal_list);
    }
  } else if (is_string(val)) {
    /* empty string */
    if (*(val->value.mal_string) == '\0') {
      return make_nil();
    } else {
      char *ch = val->value.mal_string;
      list lst = NULL;
      while (*ch != '\0') {
        char *new_ch = GC_MALLOC(sizeof(*new_ch));
        strncpy(new_ch, ch, 1);
        lst = list_push(lst, make_string(new_ch));
        ch++;
      }
      return make_list(list_reverse(lst));
    }
  } else if (is_nil(val)) {
    return make_nil();
  } else {
    return make_error("'seq': expected a list, vector or string");
  }
}

maltype *mal_meta(list args) {
  if (!args || args->next) {
    return make_error("'meta': expected exactly one argument");
  }
  maltype *val = args->data;
  if (!is_sequential(val) && !is_hashmap(val) && !is_callable(val)) {
    return make_error("'meta': metadata not supported for data type");
  }
  if (!val->metadata) {
    return make_nil();
  } else {
    return val->metadata;
  }
}
maltype *mal_with_meta(list args) {
  if (!args || !args->next || args->next->next) {
    return make_error("'with-meta': expected exactly two arguments");
  }
  maltype *val = args->data;
  if (!is_sequential(val) && !is_hashmap(val) && !is_callable(val)) {
    return make_error("'with-meta': metadata not supported for data type");
  }
  maltype *metadata = args->next->data;
  maltype *new_val = copy_type(val);
  new_val->metadata = metadata;

  return new_val;
}

/* helper functions */

maltype *as_str(list args, int readably, char *separator) {
  long buffer_length = STRING_BUFFER_SIZE;
  long separator_length = strlen(separator);
  char *buffer = GC_MALLOC(sizeof(*buffer) * STRING_BUFFER_SIZE);
  long char_count = 0;

  while (args) {
    maltype *arg = args->data;
    char *str = pr_str(arg, readably);
    int len = strlen(str);

    char_count += len;
    char_count += separator_length;
    if (char_count >= buffer_length) {
      buffer = GC_REALLOC(buffer, sizeof(*buffer) * char_count + 1);
    }
    strncat(buffer, str, char_count);
    args = args->next;
    if (args) {
      strcat(buffer, separator);
    }
  }
  return make_string(buffer);
}

maltype *print(list args, int readably, char *separator) {
  while (args) {
    printf("%s", pr_str(args->data, readably));
    args = args->next;
    if (args) {
      printf("%s", separator);
    }
  }
  printf("\n");
  return make_nil();
}

maltype *equal_lists(maltype *list1, maltype *list2) {
  list first = list1->value.mal_list;
  list second = list2->value.mal_list;

  if (list_count(first) != list_count(second)) {
    return make_false();
  } else {
    while (first && second) {
      list args = NULL;
      args = list_push(args, second->data);
      args = list_push(args, first->data);
      maltype *cmp = mal_equals(args);
      if (is_false(cmp)) {
        return make_false();
        break;
      }
      first = first->next;
      second = second->next;
    }
    return make_true();
  }
}

maltype *equal_hashmaps(maltype *map1, maltype *map2) {
  list first = map1->value.mal_list;
  list second = map2->value.mal_list;

  if (!first && !second) {
    return make_true();
  }
  if (list_count(first) != list_count(second)) {
    return make_false();
  }

  while (first) {
    maltype *key1 = first->data;
    maltype *val1 = first->next->data;
    maltype *val2 = hashmap_getf(second, get_fn(key1), get_fn);

    if (!val2) {
      return make_false();
    }

    list args = NULL;
    args = list_push(args, val1);
    args = list_push(args, val2);

    maltype *cmp = mal_equals(args);

    if (is_false(cmp)) {
      return make_false();
      break;
    }
    first = first->next->next;
  }
  return make_true();
}

/* helper function for get */
char *get_fn(gptr data) {
  maltype *val = data;
  switch (val->type) {
  case MALTYPE_STRING:
    return (val->value.mal_string);
    break;
  case MALTYPE_SYMBOL:
    return (val->value.mal_symbol);
    break;
  case MALTYPE_KEYWORD:
    return (val->value.mal_keyword);
    break;
  default:
    return NULL;
  }
}
// end core.c

//  printer.c
char *pr_str(maltype *val, int readably) {
  if (!val)
    return "";
  switch (val->type) {
  case MALTYPE_SYMBOL:
    return snprintfbuf(SYMBOL_BUFFER_SIZE, "%s", val->value.mal_symbol);
    break;
  case MALTYPE_KEYWORD:
    return snprintfbuf(SYMBOL_BUFFER_SIZE, ":%s", val->value.mal_keyword);
    break;
  case MALTYPE_INTEGER:
    return snprintfbuf(SYMBOL_BUFFER_SIZE, "%ld", val->value.mal_integer);
    break;
  case MALTYPE_FLOAT:
    return snprintfbuf(SYMBOL_BUFFER_SIZE, "%lf", val->value.mal_float);
    break;
  case MALTYPE_STRING:
    if (readably) {
      return snprintfbuf(STRING_BUFFER_SIZE, "%s",
                         escape_string(val->value.mal_string));
    } else {
      return snprintfbuf(STRING_BUFFER_SIZE, "%s", val->value.mal_string);
    }
    break;
  case MALTYPE_TRUE:
    return PRINT_TRUE;
    break;
  case MALTYPE_FALSE:
    return PRINT_FALSE;
    break;
  case MALTYPE_NIL:
    return PRINT_NIL;
    break;
  case MALTYPE_LIST:
    return pr_str_list(val->value.mal_list, readably, "(", ")", " ");
    break;
  case MALTYPE_VECTOR:
    return pr_str_list(val->value.mal_list, readably, "[", "]", " ");
    break;
  case MALTYPE_HASHMAP:
    return pr_str_list(val->value.mal_list, readably, "{", "}", " ");
    break;
  case MALTYPE_FUNCTION:
    return snprintfbuf(FUNCTION_BUFFER_SIZE, "#<function::native@%p>", val->value.mal_function);
    break;
  case MALTYPE_CLOSURE: {
    maltype *definition = (val->value.mal_closure)->definition;
    maltype *parameters = (val->value.mal_closure)->parameters;
    maltype *more_symbol = (val->value.mal_closure)->more_symbol;
    list lst = parameters->value.mal_list;
    if (more_symbol) {
      lst = list_reverse(lst);
      lst = list_push(lst,
                      make_symbol(snprintfbuf(STRING_BUFFER_SIZE, "%s%s", "&", more_symbol->value.mal_symbol)));
      lst = list_reverse(lst);
    }
    if (val->is_macro) {
      return snprintfbuf(
          FUNCTION_BUFFER_SIZE, "#<function::macro: (fn* %s %s))",
          pr_str(make_list(lst), UNREADABLY), pr_str(definition, UNREADABLY));
    } else {
      return snprintfbuf(
          FUNCTION_BUFFER_SIZE, "#<function::closure: (fn* %s %s))",
          pr_str(make_list(lst), UNREADABLY), pr_str(definition, UNREADABLY));
    }
  } break;

  case MALTYPE_ATOM:

    return snprintfbuf(STRING_BUFFER_SIZE, "(atom %s)",
                       pr_str(val->value.mal_atom, readably));
    break;

  case MALTYPE_ERROR:

    return snprintfbuf(STRING_BUFFER_SIZE, "Uncaught error: %s",
                       pr_str(val->value.mal_error, UNREADABLY));
    break;

  default:
    /* can't happen unless a new MalType is added */
    return "Printer error: unknown type\n";
    break;
  }
}

char *pr_str_list(list lst, int readably, char *start_delimiter,
                  char *end_delimiter, char *separator) {
  char *list_buffer = GC_MALLOC(sizeof(*list_buffer) * LIST_BUFFER_SIZE);
  long buffer_length = LIST_BUFFER_SIZE;

  /* add the start delimiter */
  list_buffer = strcpy(list_buffer, start_delimiter);

  long len = strlen(start_delimiter);
  long count = len;

  while (lst) {
    /* concatenate next element */
    maltype *data = lst->data;
    char *str = pr_str(data, readably);

    len = strlen(str);
    count += len;

    if (count >= buffer_length) {
      buffer_length += (count + 1);
      list_buffer = GC_REALLOC(list_buffer, buffer_length);
    }
    strncat(list_buffer, str, len);
    lst = lst->next;
    if (lst) {
      len = strlen(separator);
      count += len;

      if (count >= buffer_length) {
        buffer_length += (count + 1);
        list_buffer = GC_REALLOC(list_buffer, buffer_length);
      }
      /* add the separator */
      strncat(list_buffer, separator, len);
    }
  }

  if (count >= buffer_length) {
    len = strlen(end_delimiter);
    count += len;

    buffer_length += (count + 1);
    list_buffer = GC_REALLOC(list_buffer, buffer_length);
  }

  /* add the end delimiter */
  strncat(list_buffer, end_delimiter, len);

  return list_buffer;
}

// 转义
char *escape_string(char *str) {
  long buffer_length =
      2 * (strlen(str) + 1); /* allocate a reasonable initial buffer size */
  char *buffer = GC_MALLOC(sizeof(*buffer) * buffer_length);
  strcpy(buffer, "\"");
  char *curr = str;
  while (*curr != '\0') {
    switch (*curr) {
    case '"':
      strcat(buffer, "\\\"");
      break;
    case '\\':
      strcat(buffer, "\\\\");
      break;
    case 0x0A:
      strcat(buffer, "\\n");
      break;
    default:
      strncat(buffer, curr, 1);
    }
    curr++;
    /* check for overflow and increase buffer size */
    if ((curr - str) >= buffer_length) {
      buffer_length *= 2;
      buffer = GC_REALLOC(buffer, sizeof(*buffer) * buffer_length);
    }
  }
  strcat(buffer, "\"");
  /* trim the buffer to the size of the actual escaped string */
  buffer_length = strlen(buffer);
  buffer = GC_REALLOC(buffer, sizeof(*buffer) * buffer_length + 1);
  return buffer;
}

char *snprintfbuf(long initial_size, char *fmt, ...) {
  /* this is just a wrapper for the *printf family that ensures the string is
   * long enough to hold the contents */
  va_list argptr;
  va_start(argptr, fmt);
  char *buffer = GC_MALLOC(sizeof(*buffer) * initial_size);
  long n = vsnprintf(buffer, initial_size, fmt, argptr);
  va_end(argptr);
  if (n > initial_size) {
    va_start(argptr, fmt);
    buffer = GC_REALLOC(buffer, sizeof(*buffer) * n);
    vsnprintf(buffer, n, fmt, argptr);
    va_end(argptr);
  }
  return buffer;
}
// end printer.c

//  hashmap.c
// end hashmap.c
// a_mal.c

/* forward references */
maltype *eval_ast(maltype *ast, Env *env);
maltype *eval_defbang(maltype *ast, Env **env);
void eval_letstar(maltype **ast, Env **env);
void eval_if(maltype **ast, Env **env);
maltype *eval_fnstar(maltype *ast, Env *env);
maltype *eval_do(maltype *ast, Env *env);
maltype *eval_quote(maltype *ast);
maltype *eval_quasiquote(maltype *ast);
maltype *eval_quasiquoteexpand(maltype *ast);
maltype *eval_defmacrobang(maltype *, Env **env);
maltype *eval_macroexpand(maltype *ast, Env *env);
maltype *macroexpand(maltype *ast, Env *env);
void eval_try(maltype **ast, Env **env);

maltype *eval(maltype *ast, Env *env) {
  /* Use goto to jump here rather than calling eval for tail-call elimination */
TCE_entry_point:
  if (!ast) {
    return make_nil();
  }

  ast = macroexpand(ast, env);
  if (is_error(ast)) {
    return ast;
  }
  /* not a list */
  if (!is_list(ast)) {
    return eval_ast(ast, env);
  }
  /* empty list */
  if (ast->value.mal_list == NULL) {
    return ast;
  }
  /* list */
  maltype *first = (ast->value.mal_list)->data;
  char *symbol = first->value.mal_symbol;
  if (is_symbol(first)) {
    /* handle special symbols first */
    if (strcmp(symbol, SYMBOL_DEFBANG) == 0) {
      return eval_defbang(ast, &env);
    } else if (strcmp(symbol, SYMBOL_LETSTAR) == 0) {
      /* TCE - modify ast and env directly and jump back to eval */
      eval_letstar(&ast, &env);
      if (is_error(ast)) {
        return ast;
      }
      goto TCE_entry_point;
    } else if (strcmp(symbol, SYMBOL_IF) == 0) {
      /* TCE - modify ast directly and jump back to eval */
      eval_if(&ast, &env);
      if (is_error(ast)) {
        return ast;
      }
      goto TCE_entry_point;
    } else if (strcmp(symbol, SYMBOL_FNSTAR) == 0) {
      return eval_fnstar(ast, env);
    } else if (strcmp(symbol, SYMBOL_DO) == 0) {
      /* TCE - modify ast and env directly and jump back to eval */
      ast = eval_do(ast, env);
      if (is_error(ast)) {
        return ast;
      }
      goto TCE_entry_point;
    } else if (strcmp(symbol, SYMBOL_QUOTE) == 0) {
      return eval_quote(ast);
    } else if (strcmp(symbol, SYMBOL_QUASIQUOTE) == 0) {
      ast = eval_quasiquote(ast);
      if (is_error(ast)) {
        return ast;
      }
      goto TCE_entry_point;
    } else if (strcmp(symbol, SYMBOL_QUASIQUOTEEXPAND) == 0) {

      list lst = ast->value.mal_list;
      return eval_quasiquote(make_list(lst));
    } else if (strcmp(symbol, SYMBOL_DEFMACROBANG) == 0) {
      return eval_defmacrobang(ast, &env);
    } else if (strcmp(symbol, SYMBOL_MACROEXPAND) == 0) {
      return eval_macroexpand(ast, env);
    } else if (strcmp(symbol, SYMBOL_TRYSTAR) == 0) {
      /* TCE - modify ast and env directly and jump back to eval */
      eval_try(&ast, &env);
      if (is_error(ast)) {
        return ast;
      }
      goto TCE_entry_point;
    }
  }
  /* first element is not a special symbol */
  maltype *evaluated_list = eval_ast(ast, env);

  if (is_error(evaluated_list)) {
    return evaluated_list;
  }

  /* apply the first element of the list to the arguments */
  list evlst = evaluated_list->value.mal_list;
  maltype *func = evlst->data;

  if (is_function(func)) {
    return (*func->value.mal_function)(evlst->next);
  } else if (is_closure(func)) {

    MalClosure *closure = func->value.mal_closure;
    list params = (closure->parameters)->value.mal_list;

    long param_count = list_count(params);
    long arg_count = list_count(evlst->next);

    if (param_count > arg_count) {
      return make_error("too few arguments supplied to function");
    } else if ((param_count < arg_count) && !closure->more_symbol) {
      return make_error("too many arguments supplied to function");
    } else {
      /* TCE - modify ast and env directly and jump back to eval */
      env = env_make(closure->env, params, evlst->next, closure->more_symbol);
      ast = func->value.mal_closure->definition;
      if (is_error(ast)) {
        return ast;
      }
      goto TCE_entry_point;
    }
  } else {
    return make_error_fmt("first item in list is not callable: '%s'",
                          pr_str(func, UNREADABLY));
  }
}

void prn(maltype *val) {
  char *output = pr_str(val, READABLY);
  printf("%s\n", output);
}

void rep(char *str, Env *env) { prn(eval(read_str(str), env)); }

/* declare as global so it can be accessed by mal_eval */
Env *global_env;

maltype *mal_eval(list args) {
  maltype *ast = args->data;
  return eval(ast, global_env);
}

maltype *mal_readline(list args) {
  if (!args || args->next) {
    return make_error("'readline': expected exactly one argument");
  }
  maltype *prompt = args->data;
  if (!is_string(prompt)) {
    return make_error_fmt("'readline': argument is not a string '%s'",
                          pr_str(prompt, UNREADABLY));
  }
  char *str = readline(prompt->value.mal_string);
  if (str) {
    add_history(str);
    return make_string(str);
  } else {
    return make_nil();
  }
}

maltype *eval_ast(maltype *ast, Env *env) {
  /* forward references */
  list evaluate_list(list lst, Env * env);
  list evaluate_vector(list lst, Env * env);
  list evaluate_hashmap(list lst, Env * env);
  if (is_symbol(ast)) {
    maltype *symbol_value = env_get(env, ast);
    if (symbol_value) {
      return symbol_value;
    } else {
      return make_error_fmt("var '%s' not found", pr_str(ast, UNREADABLY));
    }
  } else if (is_list(ast)) {
    list result = evaluate_list(ast->value.mal_list, env);
    if (!result || !is_error(result->data)) {
      return make_list(result);
    } else {
      return result->data;
    }
  } else if (is_vector(ast)) {
    list result = evaluate_vector(ast->value.mal_list, env);
    if (!result || !is_error(result->data)) {
      return make_vector(result);
    } else {
      return result->data;
    }
  } else if (is_hashmap(ast)) {
    list result = evaluate_hashmap(ast->value.mal_list, env);
    if (!result || !is_error(result->data)) {
      return make_hashmap(result);
    } else {
      return result->data;
    }
  } else {
    return ast;
  }
}
// def!
maltype *eval_defbang(maltype *ast, Env **env) {
  list lst = (ast->value.mal_list)->next;
  if (!lst || !lst->next || lst->next->next) {
    return make_error_fmt("'def!': expected exactly two arguments");
  }
  maltype *defbang_symbol = lst->data;
  if (!is_symbol(defbang_symbol)) {
    return make_error_fmt("'def!': expected symbol for first argument");
  }
  maltype *defbang_value = lst->next->data;
  maltype *result = eval(defbang_value, *env);
  if (!is_error(result)) {
    *env = env_set(*env, defbang_symbol, result);
  }
  return result;
}
// let*
void eval_letstar(maltype **ast, Env **env) {
  list lst = (*ast)->value.mal_list;
  if (!lst->next) {
    *ast = make_error("'let*': missing bindings list");
    return;
  }

  maltype *bindings = lst->next->data;
  maltype *forms = lst->next->next ? lst->next->next->data : make_nil();

  if (!is_sequential(bindings)) {
    *ast = make_error("'let*': first argument is not list or vector");
    return;
  }

  list bindings_list = bindings->value.mal_list;
  if (list_count(bindings_list) % 2 == 1) {
    *ast = make_error("'let*': expected an even number of binding pairs");
    return;
  }

  Env *letstar_env = env_make(*env, NULL, NULL, NULL);

  /* evaluate the bindings */
  while (bindings_list) {

    maltype *symbol = bindings_list->data;
    maltype *value = eval(bindings_list->next->data, letstar_env);

    /* early return from error */
    if (is_error(value)) {
      *ast = value;
      return;
    }
    env_set(letstar_env, symbol, value);
    bindings_list = bindings_list->next->next;
  }
  *env = letstar_env;
  *ast = forms;
  return;
}

void eval_if(maltype **ast, Env **env) {
  list lst = (*ast)->value.mal_list;
  if (!lst->next || !lst->next->next) {
    *ast = make_error("'if': too few arguments");
    return;
  }
  if (lst->next->next->next && lst->next->next->next->next) {
    *ast = make_error("'if': too many arguments");
    return;
  }
  maltype *condition = eval(lst->next->data, *env);
  if (is_error(condition)) {
    *ast = condition;
    return;
  }
  if (is_false(condition) || is_nil(condition)) {
    /* check whether false branch is present */
    if (lst->next->next->next) {
      *ast = lst->next->next->next->data;
      return;
    } else {
      *ast = make_nil();
      return;
    }
  } else {
    *ast = lst->next->next->data;
    return;
  }
}
// fn*
maltype *eval_fnstar(maltype *ast, Env *env) {
  list lst = ast->value.mal_list;
  if (!lst->next) {
    return make_error("'fn*': missing argument list");
  } else if (!lst->next->next) {
    return make_error("'fn*': missing function body");
  }
  maltype *params = lst->next->data;
  list params_list = params->value.mal_list;

  maltype *more_symbol = NULL;

  maltype *result = regularise_parameters(&params_list, &more_symbol);
  if (is_error(result)) {
    return result;
  }
  maltype *definition = lst->next->next->data;
  maltype *regular_params = make_list(params_list);

  return make_closure(env, regular_params, definition, more_symbol);
}

maltype *eval_do(maltype *ast, Env *env) {
  list lst = ast->value.mal_list;
  /* handle empty 'do' */
  if (!lst->next) {
    return make_nil();
  }
  /* evaluate all but the last form */
  lst = lst->next;
  while (lst->next) {
    maltype *val = eval(lst->data, env);
    /* return error early */
    if (is_error(val)) {
      return val;
    }
    lst = lst->next;
  }
  /* return the last form for TCE evaluation */
  return lst->data;
}

maltype *eval_quote(maltype *ast) {
  list lst = (ast->value.mal_list)->next;
  if (!lst) {
    return make_nil();
  } else if (lst->next) {
    return make_error("'quote': expected exactly one argument");
  } else {
    return lst->data;
  }
}

maltype *eval_quasiquote(maltype *ast) {
  list lst = ast->value.mal_list;
  /* no arguments (quasiquote) */
  if (!lst->next) {
    return make_nil();
  }
  /* too many arguments */
  else if (lst->next->next) {
    return make_error("'quasiquote': expected exactly one argument");
  }
  return quasiquote(lst->next->data);
}
/*
https://clojure.org/reference/reader#syntax-quote
syntax-quote (`, note the "backquote" character), Unquote (~) and
Unquote-splicing (~@) quote (') quasiquote 是scheme 的叫法
https://scheme.com/tspl4/grammar.html
' (quote <datum>)
` (quasiquote <datum>)
, (unquote <datum>)
,@ (unquote-splicing <datum>)
#' (syntax <datum>)
#` (quasisyntax <datum>)
#, (unsyntax <datum>)
#,@ (unsyntax-splicing <datum>)
*/
// todo: change quasiquote to syntax-quote
maltype *quasiquote(maltype *ast) {
  /* argument to quasiquote is self-evaluating: (quasiquote val) => val */
  if (is_self_evaluating(ast)) {
    return ast;
  }
  /* argument to quasiquote is a vector: (quasiquote [first rest]) */
  else if (is_vector(ast)) {
    return quasiquote_vector(ast);
  }
  /* argument to quasiquote is a list: (quasiquote (first rest)) */
  else if (is_list(ast)) {
    return quasiquote_list(ast);
  }
  /* argument to quasiquote is not self-evaluating and isn't sequential:
     (quasiquote val) => (quote val) */
  else {
    list lst = list_make(ast);
    lst = list_push(lst, make_symbol("quote"));
    return make_list(lst);
  }
}

maltype *quasiquote_vector(maltype *ast) {
  /* forward references */
  maltype *quasiquote_list(maltype * ast);
  list args = ast->value.mal_list;
  if (args) {
    maltype *first = args->data;
    /* if first element is unquote return quoted */
    if (is_symbol(first) &&
        strcmp(first->value.mal_symbol, SYMBOL_UNQUOTE) == 0) {
      list lst = list_make(ast);
      lst = list_push(lst, make_symbol("quote"));
      return make_list(lst);
    }
  }
  /* otherwise process like a list */
  list lst = list_make(make_symbol("vec"));
  maltype *result = quasiquote_list(ast);
  if (is_error(result)) {
    return result;
  } else {
    lst = list_push(lst, result);
  }
  lst = list_reverse(lst);
  return make_list(lst);
}
// 准引用 quasiquote ` 用在宏上 里面可以 用unquote 等
maltype *quasiquote_list(maltype *ast) {
  list args = ast->value.mal_list;
  /* handle empty list: (quasiquote ()) => () */
  if (!args) {
    return make_list(NULL);
  }
  maltype *first = args->data;
  /* handle unquote: (quasiquote (unquote second)) => second */
  if (is_symbol(first) &&
      strcmp(first->value.mal_symbol, SYMBOL_UNQUOTE) == 0 && args->next) {
    if (args->next->next) {
      return make_error("'quasiquote': unquote expected exactly one argument");
    } else {
      return args->next->data;
    }
  }
  /* handle splice-unquote: (quasiquote ((splice-unquote first-second) rest))
     => (concat first-second (quasiquote rest)) */
  else if (is_list(first) && first->value.mal_list != NULL &&
           is_symbol(first->value.mal_list->data) &&
           strcmp(((maltype *)first->value.mal_list->data)->value.mal_symbol,
                  SYMBOL_SPLICE_UNQUOTE) == 0) {
    if (!first->value.mal_list->next) {
      return make_error(
          "'quasiquote': splice-unquote expected exactly one argument");
    }
    maltype *first_second = first->value.mal_list->next->data;
    list lst = list_make(make_symbol("concat"));
    lst = list_push(lst, first_second);
    maltype *rest = quasiquote(make_list(args->next));
    if (is_error(rest)) {
      return rest;
    }
    lst = list_push(lst, rest);
    lst = list_reverse(lst);
    return make_list(lst);
  }
  /* handle all other lists recursively:
  (quasiquote (first rest)) => (cons (quasiquote first) (quasiquote rest)) */
  else {
    list lst = list_make(make_symbol("cons"));
    maltype *first = quasiquote(args->data);
    if (is_error(first)) {
      return first;
    } else {
      lst = list_push(lst, first);
    }

    maltype *rest = quasiquote(make_list(args->next));
    if (is_error(rest)) {
      return rest;
    } else {
      lst = list_push(lst, rest);
    }
    lst = list_reverse(lst);
    return make_list(lst);
  }
}

// defmacro!
maltype *eval_defmacrobang(maltype *ast, Env **env) {
  list lst = (ast->value.mal_list)->next;
  if (!lst || !lst->next || lst->next->next) {
    return make_error_fmt("'defmacro!': expected exactly two arguments");
  }
  maltype *defbang_symbol = lst->data;
  if (!is_symbol(defbang_symbol)) {
    return make_error_fmt("'defmacro!': expected symbol for first argument");
  }

  maltype *defbang_value = lst->next->data;
  maltype *result = eval(defbang_value, *env);
  if (!is_error(result)) {
    result = copy_type(result);
    result->is_macro = 1;
    *env = env_set(*env, defbang_symbol, result);
  }
  return result;
}

maltype *eval_macroexpand(maltype *ast, Env *env) {
  list lst = ast->value.mal_list;
  if (!lst->next) {
    return make_nil();
  } else if (lst->next->next) {
    return make_error("'macroexpand': expected exactly one argument");
  } else {
    return macroexpand(lst->next->data, env);
  }
}
// 编译期运行 产生 ast 然后与普通的clojure代码一样
maltype *macroexpand(maltype *ast, Env *env) {
  while (is_macro_call(ast, env)) {
    list lst = ast->value.mal_list;

    maltype *macro_fn = env_get(env, lst->data);
    MalClosure *cls = macro_fn->value.mal_closure;
    maltype *more_symbol = cls->more_symbol;

    list params_list = (cls->parameters)->value.mal_list;
    list args_list = lst->next;

    env = env_make(cls->env, params_list, args_list, more_symbol);
    ast = eval(cls->definition, env);
  }
  return ast;
}

void eval_try(maltype **ast, Env **env) {
  list lst = (*ast)->value.mal_list;
  if (!lst->next) {
    *ast = make_nil();
    return;
  }
  if (lst->next->next && lst->next->next->next) {
    *ast = make_error("'try*': expected maximum of two arguments");
    return;
  }
  maltype *try_clause = lst->next->data;
  maltype *try_result = eval(try_clause, *env);
  /* no catch* clause */
  if (!is_error(try_result) || !lst->next->next) {
    *ast = try_result;
    return;
  }
  /* process catch* clause */
  maltype *catch_clause = lst->next->next->data;
  list catch_list = catch_clause->value.mal_list;
  if (!catch_list) {
    *ast = make_error("'try*': catch* clause is empty");
    return;
  }
  maltype *catch_symbol = catch_list->data;
  if (strcmp(catch_symbol->value.mal_symbol, SYMBOL_CATCHSTAR) != 0) {
    *ast = make_error("Error: catch clause is missing catch* symbol");
    return;
  }

  if (!catch_list->next || !catch_list->next->next) {
    *ast = make_error("Error: catch* clause expected two arguments");
    return;
  }

  if (!is_symbol(catch_list->next->data)) {
    *ast = make_error("Error: catch* clause expected a symbol");
    return;
  }

  /* bind the symbol to the exception */
  list symbol_list = list_make(catch_list->next->data);
  list expr_list = list_make(try_result->value.mal_error);

  Env *catch_env = env_make(*env, symbol_list, expr_list, NULL);
  *ast = catch_list->next->next->data;
  *env = catch_env;

  return;
}

list evaluate_list(list lst, Env *env) {
  list evlst = NULL;
  while (lst) {
    maltype *val = eval(lst->data, env);
    if (is_error(val)) {
      return list_make(val);
    }
    evlst = list_push(evlst, val);
    lst = lst->next;
  }
  return list_reverse(evlst);
}

list evaluate_vector(list lst, Env *env) {
  /* TODO: implement a real vector */
  list evlst = NULL;
  while (lst) {
    maltype *val = eval(lst->data, env);
    if (is_error(val)) {
      return list_make(val);
    }
    evlst = list_push(evlst, val);
    lst = lst->next;
  }
  return list_reverse(evlst);
}

list evaluate_hashmap(list lst, Env *env) {
  /* TODO: implement a real hashmap */
  list evlst = NULL;
  while (lst) {
    /* keys are unevaluated */
    evlst = list_push(evlst, lst->data);
    lst = lst->next;
    /* values are evaluated */
    maltype *val = eval(lst->data, env);
    if (is_error(val)) {
      return list_make(val);
    }
    evlst = list_push(evlst, val);
    lst = lst->next;
  }
  return list_reverse(evlst);
}
// clojure 不定参数 &
maltype *regularise_parameters(list *args, maltype **more_symbol) {
  /* forward reference */
  char *symbol_fn(gptr data);
  list regular_args = NULL;
  while (*args) {
    maltype *val = (*args)->data;
    if (!is_symbol(val)) {
      return make_error_fmt("non-symbol found in fn argument list '%s'",
                            pr_str(val, UNREADABLY));
    }
    if (val->value.mal_symbol[0] == '&') {
      /* & is found but there is no symbol */
      if (val->value.mal_symbol[1] == '\0' && !(*args)->next) {
        return make_error("missing symbol after '&' in argument list");
      }
      /* & is found and there is a single symbol after */
      else if ((val->value.mal_symbol[1] == '\0' && (*args)->next &&
                is_symbol((*args)->next->data) && !(*args)->next->next)) {
        *more_symbol = (*args)->next->data;
        break;
      }
      /* & is found and there extra symbols after */
      else if ((val->value.mal_symbol[1] == '\0' && (*args)->next &&
                (*args)->next->next)) {
        return make_error_fmt(
            "unexpected symbol after '& %s' in argument list: '%s'",
            pr_str((*args)->next->data, UNREADABLY),
            pr_str((*args)->next->next->data, UNREADABLY));
      }
      /* & is found as part of the symbol and no other symbols */
      else if (val->value.mal_symbol[1] != '\0' && !(*args)->next) {
        *more_symbol = make_symbol((val->value.mal_symbol + 1));
        break;
      }
      /* & is found as part of the symbol but there are other symbols after */
      else if (val->value.mal_symbol[1] != '\0' && (*args)->next) {
        return make_error_fmt(
            "unexpected symbol after '%s' in argument list: '%s'",
            pr_str(val, UNREADABLY), pr_str((*args)->next->data, UNREADABLY));
      }
    }
    /* & is not found - add the symbol to the regular argument list */
    else {
      if (list_findf(regular_args, val->value.mal_symbol, symbol_fn) > 0) {
        return make_error_fmt("duplicate symbol in argument list: '%s'",
                              pr_str(val, UNREADABLY));
      } else {
        regular_args = list_push(regular_args, val);
      }
    }
    *args = (*args)->next;
  }
  *args = list_reverse(regular_args);
  return make_nil();
}

char *symbol_fn(gptr data) { return (((maltype *)data)->value.mal_symbol); }

/* used by core functions but not eval as doesn't do TCE */
maltype *apply(maltype *fn, list args) {
  if (is_function(fn)) {
    maltype *(*fun_ptr)(list) = fn->value.mal_function;
    return (*fun_ptr)(args);
  } else { /* is_closure(fn) */
    MalClosure *c = fn->value.mal_closure;
    list params = (c->parameters)->value.mal_list;
    long param_count = list_count(params);
    long arg_count = list_count(args);
    if (param_count > arg_count) {
      return make_error("too few arguments supplied to function");
    } else if ((param_count < arg_count) && !c->more_symbol) {
      return make_error("too many arguments supplied to function");
    } else {
      Env *env = env_make(c->env, params, args, c->more_symbol);
      return eval(fn->value.mal_closure->definition, env);
    }
  }
}

int is_macro_call(maltype *ast, Env *env) {
  if (!is_list(ast))
    return 0;
  list lst = ast->value.mal_list; /* empty list */
  if (!lst)
    return 0;
  maltype *first = lst->data; /* first item not a symbol */
  if (!is_symbol(first)) {
    return 0;
  }
  maltype *val = env_get(env, first);
  if (is_error(val)) {
    return 0;
  } else {
    return (val->is_macro);
  }
}

int main(int argc, char **argv) {
  Env *repl_env = env_make(NULL, NULL, NULL, NULL);
  global_env = repl_env;
  ns *core_ns = ns_make_core();
  hashmap mappings = core_ns->mappings;
  while (mappings) {
    char *symbol = mappings->data;
    maltype *(*function)(list) = mappings->next->data;
    env_set_C_fn(repl_env, symbol, function);
    /* pop symbol and function from hashmap/list */
    mappings = mappings->next->next; 
  }

  env_set_C_fn(repl_env, "eval", mal_eval);
  env_set_C_fn(repl_env, "readline", mal_readline);

  /* add functions written in mal - not using rep as it prints the result */
  rep("(def! not (fn* (a) (if a false true)))", repl_env);
  rep("(def! load-file (fn* (f) (eval (read-string (str \"(do \" (slurp f) \"\nnil)\")))))", repl_env);
  rep("(defmacro! cond (fn* (& xs) (if (> (count xs) 0) (list 'if (first xs) (if (> (count xs) 1) (nth xs 1) (throw \"odd number of forms to cond\")) (cons 'cond (rest (rest xs)))))))", repl_env);


  rep("(load-file \"core.clj\")", repl_env);
  /* make command line arguments available in the environment */
  list lst = NULL;
  for (long i = 2; i < argc; i++) {
    lst = list_push(lst, make_string(argv[i]));
  }
  env_set(repl_env, make_symbol("*ARGV*"), make_list(list_reverse(lst)));
  env_set(repl_env, make_symbol("*host-language*"), make_string("fengge-c.2"));

  /* first argument on command line is filename */
  if (argc > 1) {
    char *load_command = snprintfbuf(1024, "(load-file \"%s\")", argv[1]);
    rep(load_command, repl_env);
  } else {
    rep("(println (str \"mal [\" *host-language* \"]\"))", repl_env);
    while (1) {
      char *input = readline(PROMPT_STRING);
      if (!input) { printf("\n"); return 0; } /* Check for EOF (Ctrl-D) */
      add_history(input);
      rep(input, repl_env);
      free(input);
    }
  }
  return 0;
}

// gcc clojure.c -ledit -lgc
// gcc -std=c99 -g -Wall clojure.c -ledit -lgc
// (def! one (fn* [x] (+ 2 x)))
// a lowercase letter It looks comfortable
