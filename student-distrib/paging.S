.text
.globl loadPageDirectory
.globl enablePaging

#;The final step is to actually enable paging. 
#;cited from osdev.org
#;http://wiki.osdev.org/Setting_Up_Paging
loadPageDirectory:
pushl %ebp
movl %esp, %ebp
movl 8(%esp), %eax
movl %eax, %cr3
movl %cr4,%eax
orl  $0x00000010,%eax
movl %eax,%cr4
movl %ebp, %esp
popl %ebp
ret


enablePaging:
push %ebp
movl %esp, %ebp
movl %cr0, %eax
orl $0x80000000, %eax
movl %eax, %cr0
movl %ebp, %esp
popl %ebp
ret

