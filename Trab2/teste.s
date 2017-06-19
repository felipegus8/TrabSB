.text 
.globl foo

foo:
push   %rbp
mov    %rsp,%rbp
subq    $16,%rsp

movl   $0,%r12d
movl   $1,%r13d
addl   %r13d,%r12d
movl %r12d,-4(%rbp)
cmpl $0,%edi
jnz L2
movl -4(%rbp),%eax
mov %rbp,%rsp
pop %rbp
ret 


L2:
movl -4(%rbp),%r12d
movl %edi,%r13d
imull %r13d,%r12d
movl %r12d,-4(%rbp)
movl %edi,%r12d
movl $1,%r13d
subl %r13d,%r12d
movl %r12d,%edi
cmpl $0,%edi
jnz  L2
movl -4(%rbp),%eax
mov %rbp,%rsp
pop %rbp
ret 

