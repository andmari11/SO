	.file	"hello2.c"
	.text
	.globl	a
	.data
	.align 4
	.type	a, @object
	.size	a, 4
a:
	.long	7
	.globl	b
	.align 4
	.type	b, @object
	.size	b, 4
b:
	.long	9
	.section	.rodata
.LC0:
	.string	"Hello world"
.LC1:
	.string	"%s \t a= %d b= %d\n"
	.text
	.globl	main
	.type	main, @function
main:
.LFB0:
	.cfi_startproc
	pushq	%rbp
	.cfi_def_cfa_offset 16
	.cfi_offset 6, -16
	movq	%rsp, %rbp
	.cfi_def_cfa_register 6
	subq	$16, %rsp
	leaq	.LC0(%rip), %rax
	movq	%rax, -16(%rbp)
	movl	$0, -4(%rbp)
	jmp	.L2
.L3:
	movl	b(%rip), %ecx
	movl	a(%rip), %edx
	movq	-16(%rbp), %rax
	movq	%rax, %rsi
	leaq	.LC1(%rip), %rax
	movq	%rax, %rdi
	movl	$0, %eax
	call	printf@PLT
	movl	a(%rip), %eax
	addl	$1, %eax
	movl	%eax, a(%rip)
	movl	b(%rip), %edx
	movl	a(%rip), %eax
	cmpl	%eax, %edx
	cmovle	%edx, %eax
	movl	%eax, a(%rip)
	addl	$1, -4(%rbp)
.L2:
	cmpl	$4, -4(%rbp)
	jle	.L3
	movl	$0, %eax
	leave
	.cfi_def_cfa 7, 8
	ret
	.cfi_endproc
.LFE0:
	.size	main, .-main
	.ident	"GCC: (Debian 13.2.0-4) 13.2.0"
	.section	.note.GNU-stack,"",@progbits
