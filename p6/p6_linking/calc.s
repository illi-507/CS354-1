	.file	"calc.c"
.globl num_err
	.bss
	.align 4
	.type	num_err, @object
	.size	num_err, 4
num_err:
	.zero	4
	.local	num_calc
	.comm	num_calc,4,4
	.text
.globl power
	.type	power, @function
power:
	pushl	%ebp
	movl	%esp, %ebp
	subl	$16, %esp
	cmpl	$0, 12(%ebp)
	jns	.L2
	movl	num_err, %eax
	addl	$1, %eax
	movl	%eax, num_err
	movl	$2, %eax
	jmp	.L3
.L2:
	movl	num_calc, %eax
	addl	$1, %eax
	movl	%eax, num_calc
	movl	$1, -8(%ebp)
	movl	$0, -4(%ebp)
	jmp	.L4
.L5:
	movl	-8(%ebp), %eax
	imull	8(%ebp), %eax
	movl	%eax, -8(%ebp)
	addl	$1, -4(%ebp)
.L4:
	movl	-4(%ebp), %eax
	cmpl	12(%ebp), %eax
	jl	.L5
	movl	-8(%ebp), %eax
.L3:
	leave
	ret
	.size	power, .-power
	.ident	"GCC: (GNU) 4.4.7 20120313 (Red Hat 4.4.7-16)"
	.section	.note.GNU-stack,"",@progbits
