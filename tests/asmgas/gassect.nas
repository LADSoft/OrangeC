	.section one
	.byte 1
	.section one, 4
	.byte 101
	.section one,0
	.byte 2

	.pushsection one,4
	.byte 102
	.popsection
	.byte 3
	.subsection 4
	.byte 103

	.pushsection one
	.byte 4
	.previous
	.byte 104
	.previous
	.byte 5
        .previous
        .byte 105
        .previous
        .byte 6
	.popsection
	.byte 106


	.text 7
mm:
	.byte 200
	.text 3
	.byte 100
	.text
	.byte 0
	.text 0
	.byte 2
	mov mm,%eax

	.data 7
	.byte 201
	.data 3
	.byte 101
	.data
	.byte 1
	.data 0
	.byte 2