		.data

Count:		2
Result:		0


		.text
main:
		lw		r1,r0,Count
		beqz	r1,Finish
		add		r2,r0,r0
loop:
		add		r2,r2,r1
		subi	r1,r1,1
		bnez	r1,loop
Finish:
		sw		r2,r0,Result