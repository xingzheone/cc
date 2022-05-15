#include <string.h>
#include <stdio.h>
#include <assert.h>
#include <iconv.h>
#include <wchar.h>  //unicode utf-8
#include <locale.h>

void *my_memcpy(void *dest, void *src, unsigned count)
{
    if (dest == NULL || src == NULL)
    {
        return NULL;
    }
    char *pdest = (char *)dest;
    char *psrc = (char *)src;
    while (count--)
    {
        *pdest++ = *psrc++;
    }
    return dest;
}

char *my_strchr(const char *s, int c)
{
    printf("%c,%d\n", c, c);
    if (s == NULL)
    {
        return NULL;
    }
    while (*s != '\0')
    {
        if (*s == (char)c)
        {
            return (char *)s;
        }
        s++;
    }
    return NULL;
}

char * stpcpy23(char *dest, const char *src)
{
  size_t len = strlen (src);
  return memcpy (dest, src, len + 1) + len;
}
int my_strlen(const char* str)
{
   assert(str);
   int count = 0;
    while (*str)
    {
       count++;
        str++;
    }
    return count;
}

size_t my_strlen2(const char* str) {
    const char* end = str;
    while (*end++); 
    return end - str - 1;
}
// #define BITOP(a,b,op) \
//  ((a)[(size_t)(b)/(8*sizeof *(a))] op (size_t)1<<((size_t)(b)%(8*sizeof *(a))))

// size_t strcspn1(const char *s, const char *c) {
// 	const char *a = s;
// 	size_t byteset[32/sizeof(size_t)];
// 	if (!c[0] || !c[1]) return __strchrnul(s, *c)-a;
// 	memset(byteset, 0, sizeof byteset);
// 	for (; *c && BITOP(byteset, *(unsigned char *)c, |=); c++);
// 	for (; *s && !BITOP(byteset, *(unsigned char *)s, &); s++);
// 	return s-a;
// }

char *strpbrk1(const char *s, const char *b)
{
	s += strcspn(s, b);
	return *s ? (char *)s : 0;
}

// 二级指针传参 练习
int sq(char** num) {
    *num="feng";
    return 2;
}
void feng(){
    char* one="a";
    sq(&one);
    printf("c: %s \n",one);
}
int sq1(char* num) {
    printf("ca: %s \n",num);
    num="feng";
    return 2;
}
void feng1(){
    char* one="a";
    sq1(one);
    printf("c: %s \n",one);
}

void cm(char *p, int num) {
	p = (char *)malloc(sizeof(char)*num);
}
void cm2(char **p, int num) {
	// p = (char *)malloc(sizeof(char)*num); //不会报错.
	*p = (char *)malloc(sizeof(char)*num);
}


// http://c.biancheng.net/c/assert/ 
// 在操作指针时，一定要保证在指针有效内存空间内操作,不然 Segmentation fault  段错误。 被操作系统kill.
// c编译器内置宏 https://zhuanlan.zhihu.com/p/409044316
int main(void)
{
    // assert(NULL);
    printf("文件: %s,函数: %s %s %s \n",__FILE__ , __func__,__ASSERT_FUNCTION,__DATE__); //几个c编译器内置变量
    char destination[25];
    char *blank = " ", *c = "C++", *Borland = "Borland";

    strcpy(destination, Borland);
    strcat(destination, blank);
    strcat(destination, c);

    printf("is: %s\n", strchr(destination, 'a'));
    printf("is2: %s\n", my_strchr(destination, 'a+lskjdsafsfsfa'));
    printf("%s\n", destination);

// 字符串分割 
    char str[80] = "This is - www.runoob.com - website";
    // const char s[2] = "-";
    char* s = "-";
    char *token;
    /* 获取第一个子字符串 */
    token = strtok(str, s);
    /* 继续获取其他的子字符串 */
    while (token != NULL)
    {
        printf("%s\n", token);
        token = strtok(NULL, s);
    }
//调用 strcat 必须保证目标地址有足够的已分配的内存用于存储结果。通常需要用 malloc 提前分配内存 或 定义一个足够大的静态 char[] 。
    // char* one1="hao",one2=" ca";
    char one1[100]="hao";
    // char *one1=malloc(100);
    char* one2=" ca";
    printf("strcat: %s \n",strcat(one1,one2));
    printf("%d \n",strlen("wanglaowu"));
    printf("%d \n",strlen("行者"));  // strlen 计算的是字节 不是字符。
    printf("%d \n",wcslen("行者"));  // strlen 计算的是字节 不是字符。

    static char *const terminating_characters = " ,[](){};\n";
    const char* identifier="fen[g (+ 3 2)";
    char *end = strpbrk(identifier, terminating_characters);
    printf("strpbrk: %s \n",end);
    feng1();
    feng();
    char* str1 = NULL;
    printf("addr1: %p \n",&str1);
    printf("addr1: %p \n",str1);
	  cm(&str1, 20);
    printf("addr2: %p \n",str1);
    // printf("addr3: %p \n",&str1);
	  // cm2(str1, 20); //不会报错...
	  cm2(&str1, 20);
    printf("addr4: %p \n",str1);
    return 0;
}
// gcc str.c  && ./a.out