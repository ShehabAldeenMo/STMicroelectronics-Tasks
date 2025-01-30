	.file	"file1.c"
	.text
	.globl	Global_FirsTNumber
	.bss
	.align 4
	.type	Global_FirsTNumber, @object
	.size	Global_FirsTNumber, 4
Global_FirsTNumber:
	.zero	4
	.globl	Global_SecondNumber
	.align 4
	.type	Global_SecondNumber, @object
	.size	Global_SecondNumber, 4
Global_SecondNumber:
	.zero	4
	.section	.rodata
.LC0:
	.string	"Enter first Number = "
.LC1:
	.string	"%d"
.LC2:
	.string	"Enter second Number = "
	.text
	.globl	GetNumber
	.type	GetNumber, @function
GetNumber:
.LFB0:
	.cfi_startproc
	endbr64
	pushq	%rbp
	.cfi_def_cfa_offset 16
	.cfi_offset 6, -16
	movq	%rsp, %rbp
	.cfi_def_cfa_register 6
	leaq	.LC0(%rip), %rax
	movq	%rax, %rdi
	movl	$0, %eax
	call	printf@PLT
	leaq	Global_FirsTNumber(%rip), %rax
	movq	%rax, %rsi
	leaq	.LC1(%rip), %rax
	movq	%rax, %rdi
	movl	$0, %eax
	call	__isoc99_scanf@PLT
	leaq	.LC2(%rip), %rax
	movq	%rax, %rdi
	movl	$0, %eax
	call	printf@PLT
	leaq	Global_SecondNumber(%rip), %rax
	movq	%rax, %rsi
	leaq	.LC1(%rip), %rax
	movq	%rax, %rdi
	movl	$0, %eax
	call	__isoc99_scanf@PLT
	movl	Global_FirsTNumber(%rip), %edx
	movl	Global_SecondNumber(%rip), %eax
	addl	%edx, %eax
	popq	%rbp
	.cfi_def_cfa 7, 8
	ret
	.cfi_endproc
.LFE0:
	.size	GetNumber, .-GetNumber
	.ident	"GCC: (Ubuntu 11.4.0-1ubuntu1~22.04) 11.4.0"
	.section	.note.GNU-stack,"",@progbits
	.section	.note.gnu.property,"a"
	.align 8
	.long	1f - 0f
	.long	4f - 1f
	.long	5
0:
	.string	"GNU"
1:
	.align 8
	.long	0xc0000002
	.long	3f - 2f
2:
	.long	0x3
3:
	.align 8
4:
