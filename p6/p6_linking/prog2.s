	.file	"prog2.c"
	.text
.globl main
	.type	main, @function
main:
	pushl	%ebp
	movl	%esp, %ebp
	andl	$-16, %esp
	subl	$32, %esp
	movl	$-2, 4(%esp)
	movl	$4, (%esp)
	call	power
	movl	%eax, 28(%esp)
	cmpl	$2, 28(%esp)
	jne	.L2
	movl	num_err, %eax
	jmp	.L3
.L2:
	movl	$0, %eax
.L3:
	leave
	ret
	.size	main, .-main
	.ident	"GCC: (GNU) 4.4.7 20120313 (Red Hat 4.4.7-16)"
	.section	.note.GNU-stack,"",@progbits
