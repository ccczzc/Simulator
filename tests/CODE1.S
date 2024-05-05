		.data
InitValue:	1


		.text
main:
		lw		r1,r0,InitValue
		addi	r2,r0,2
		addi	r3,r0,3
		subi	r4,r0,4
		add		r5,r1,r2
		sub		r6,r3,r2