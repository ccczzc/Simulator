		.data
A:			1
B:			2
C:			3
Result:		0


		.text
main:
		lw		r1,r0,A
		lw		r2,r0,B
		add		r3,r1,r2
		addi	r3,r3,1
		lw		r4,r0,C
		add		r5,r3,r4
		sw		r5,r0,Result