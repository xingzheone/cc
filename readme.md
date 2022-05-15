Clojure interpreter learning

gcc clojure.c -ledit -lgc


多级指针exercise https://gcc.godbolt.org/
一级
```c
#include<stdio.h>
int sq1(char* num) {
    // printf("ca: %s \n",num);
    num="feng";
    return 2;
}
void feng1(){
    char* one="a";
    sq1(one);
    // printf("c: %s \n",one);
}
.LC0:
        .string "feng"
sq1(char*):
        push    rbp
        mov     rbp, rsp
        mov     QWORD PTR [rbp-8], rdi
        mov     QWORD PTR [rbp-8], OFFSET FLAT:.LC0
        mov     eax, 2
        pop     rbp
        ret
.LC1:
        .string "a"
feng1():
        push    rbp
        mov     rbp, rsp
        sub     rsp, 16
        mov     QWORD PTR [rbp-8], OFFSET FLAT:.LC1
        mov     rax, QWORD PTR [rbp-8]
        mov     rdi, rax
        call    sq1(char*)
        nop
        leave
        ret
```
二级
```c
#include<stdio.h>
int sq(char** num) {
    *num="feng";
    return 2;
}
void feng(){
    char* one="a";
    sq(&one);
    printf("c: %s \n",one);
}
.LC0:
        .string "feng"
sq(char**):
        push    rbp
        mov     rbp, rsp
        mov     QWORD PTR [rbp-8], rdi
        mov     rax, QWORD PTR [rbp-8]
        mov     QWORD PTR [rax], OFFSET FLAT:.LC0
        mov     eax, 2
        pop     rbp
        ret
.LC1:
        .string "a"
.LC2:
        .string "c: %s \n"
feng():
        push    rbp
        mov     rbp, rsp
        sub     rsp, 16
        mov     QWORD PTR [rbp-8], OFFSET FLAT:.LC1
        lea     rax, [rbp-8]
        mov     rdi, rax
        call    sq(char**)
        mov     rax, QWORD PTR [rbp-8]
        mov     rsi, rax
        mov     edi, OFFSET FLAT:.LC2
        mov     eax, 0
        call    printf
        nop
        leave
        ret

```
三级
```c
int sq(char*** num) {
    **num="feng";
    return 2;
}
void feng(){
    char* one="a";
    char** p=&one;
    sq(&p);
}
```
```s
.LC0:
        .string "feng"
sq(char***):
        push    rbp
        mov     rbp, rsp
        mov     QWORD PTR [rbp-8], rdi
        mov     rax, QWORD PTR [rbp-8]
        mov     rax, QWORD PTR [rax]
        mov     QWORD PTR [rax], OFFSET FLAT:.LC0
        mov     eax, 2
        pop     rbp
        ret
.LC1:
        .string "a"
feng():
        push    rbp
        mov     rbp, rsp
        sub     rsp, 16
        mov     QWORD PTR [rbp-8], OFFSET FLAT:.LC1
        lea     rax, [rbp-8]
        mov     QWORD PTR [rbp-16], rax
        lea     rax, [rbp-16]
        mov     rdi, rax
        call    sq(char***)
        nop
        leave
        ret
```