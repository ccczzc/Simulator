		.data
InitValue:	1


		.text
main:
		lw		r1,r0,InitValue
		addi	r2,r0,2
		addi	r1,r1,1
		add		r3,r1,r2
		addi	r4,r3,6
		add		r5,r3,r4