__Z20test_without_launderPiPf:
LFB1540:
	pushq	%rbp
LCFI0:
	movq	%rsp, %rbp
LCFI1:
	movq	%rdi, -8(%rbp) //int* pi
	movq	%rsi, -16(%rbp) //float* pf
	movq	-8(%rbp), %rax
	movl	$9710, (%rax) //*pi = 9710;
	movq	-16(%rbp), %rax
	movss	lC0(%rip), %xmm0
	movss	%xmm0, (%rax)
	movq	-8(%rbp), %rax
	movl	(%rax), %eax //return *pi;
	popq	%rbp
LCFI2:
	ret

__Z17test_with_launderPiPf:
LFB1541:
	pushq	%rbp
LCFI3:
	movq	%rsp, %rbp
LCFI4:
	subq	$16, %rsp
	movq	%rdi, -8(%rbp) //int* pi
	movq	%rsi, -16(%rbp) //float* pf
	movq	-8(%rbp), %rax
	movl	$9710, (%rax) //*pi = 9710;
	movq	-16(%rbp), %rax //rax = pf;
	movq	%rax, %rdi //rdi = rax (rdi == pf)
	call	__ZSt7launderIfEPT_S1_
	movss	lC0(%rip), %xmm0
	movss	%xmm0, (%rax)
	movq	-8(%rbp), %rax
	movq	%rax, %rdi
	call	__ZSt7launderIiEPT_S1_
	movl	(%rax), %eax //return *pi;
	leave
LCFI5:
	ret

__ZSt7launderIfEPT_S1_:
LFB1779:
	pushq	%rbp
LCFI9:
	movq	%rsp, %rbp
LCFI10:
	movq	%rdi, -8(%rbp) //pi = pf
	movq	-8(%rbp), %rax //rax = pi
	popq	%rbp
LCFI11:
	ret

_main:
LFB1542:
	pushq	%rbp
LCFI6:
	movq	%rsp, %rbp
LCFI7:
	subq	$16, %rsp
	movl	$0, -8(%rbp) //int a = 0;
	leaq	-8(%rbp), %rdx //&a
	leaq	-8(%rbp), %rax //reinterpret_cast<float*>(&a)
	movq	%rdx, %rsi
	movq	%rax, %rdi
	call	__Z20test_without_launderPiPf
	movl	%eax, -4(%rbp) //int look = 
	movl	-8(%rbp), %eax //eax = a
	movl	%eax, %esi
	movq	__ZSt4cout@GOTPCREL(%rip), %rax
	movq	%rax, %rdi
	call	__ZNSolsEi
	leaq	lC1(%rip), %rsi
	movq	%rax, %rdi
	call	__ZStlsISt11char_traitsIcEERSt13basic_ostreamIcT_ES5_PKc
	movq	%rax, %rdx
	movl	-4(%rbp), %eax
	movl	%eax, %esi
	movq	%rdx, %rdi
	call	__ZNSolsEi
	leaq	lC1(%rip), %rsi
	movq	%rax, %rdi
	call	__ZStlsISt11char_traitsIcEERSt13basic_ostreamIcT_ES5_PKc
	leaq	-8(%rbp), %rdx //&a
	leaq	-8(%rbp), %rax //reinterpret_cast<float*>(&a)
	movq	%rdx, %rsi
	movq	%rax, %rdi
	call	__Z17test_with_launderPiPf
	movl	%eax, -4(%rbp)
	leaq	-8(%rbp), %rax
	movq	%rax, %rdi
	call	__ZSt7launderIiEPT_S1_
	movl	(%rax), %eax
	movl	%eax, %esi
	movq	__ZSt4cout@GOTPCREL(%rip), %rax
	movq	%rax, %rdi
	call	__ZNSolsEi
	leaq	lC1(%rip), %rsi
	movq	%rax, %rdi
	call	__ZStlsISt11char_traitsIcEERSt13basic_ostreamIcT_ES5_PKc
	movq	%rax, %rdx
	movl	-4(%rbp), %eax
	movl	%eax, %esi
	movq	%rdx, %rdi
	call	__ZNSolsEi
	leaq	lC1(%rip), %rsi
	movq	%rax, %rdi
	call	__ZStlsISt11char_traitsIcEERSt13basic_ostreamIcT_ES5_PKc
	movl	$0, %eax
	leave
LCFI8:
	ret
LFE1542:
	.globl __ZSt7launderIfEPT_S1_
	.weak_definition __ZSt7launderIfEPT_S1_
