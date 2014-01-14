.globl zfunction
zfunction:
        push %ebp 
        mov %esp, %ebp
        mov 8(%ebp), %eax
        and $0xFFC01FFF, %eax
        and $0x0FFFFFFF, %eax
        shl $0x4, %eax
        and $0xFFFFFFF0, %eax
        pop %ebp
        ret
