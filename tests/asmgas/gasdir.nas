	.ascii "bye","zzz"
	.asciz "hello"
	.string "hi", "two"
        .string8 "try", "three", "four"
        .string16 "string16"
        .string32 "string32", "five"
        .byte 1
        .byte 2,3
	.word 1
	.word 2,3
	.2byte 4
	.2byte 5,6
         .hword 7
         .hword 8,9
         .short 10
	 .short 11,12
	.4byte 1
	.4byte 2,3
	.int 4.0
	.int 5,6
	.long 7
        .long 8,9
	.8byte 1,2,3
	.8byte 4
        .fill 20,1,65
	.quad 5
	.quad 6,7
        .quad 4.0
        .nops 2,1
	.nops 20
	.single 2
	.single 4.0,8.0
        .zero 20
	.float 4
	.float 4.0,8.0
        .space 10
        .skip 10,80
        .double 2
        .double 4.0,8.0
	.equ identifier, 5
        .set identifier1, 6
        .equiv identifier2, 7
        mov $identifier, %eax
        mov $identifier1, %ecx   

#	.err
#	.error "hi"
#	.warning "bye"
#	.fail 1
#	.fail 501
	.p2align 2, 65
	.byte 4
	.balign 4
	.byte 4
	.balign 4,0x55
	.p2align 3, 65
	.word 4
	.balignw 8
	.word 4
	.balignw 8,0x55aa
	.p2align 4, 65
	.int 4
	.balignl 16
	.int 4
	.balignl 16,0x55aa1122

	.byte 4
	.p2align 2,0x55
	.p2alignw 3, 0x55aa
        .p2alignl 4, 0x55aa1122	

	.fill 40,1,0x55
	.fill 20,2,0x55aa
	.fill 10,4,0x55aa1122
        .fill 5,8,0x55aa1122
	.include "gasdir.inc"
        .if 1
	 .4byte 0x11111111
        .else
         .4byte 0x22222222
        .endif
        .if 0
	 .4byte 0x11111111
        .else
         .4byte 0x22222222
        .endif

	.rept 25
	.byte 0x44
	.endr
	.dc.d 4
	.dcb.d 3, 4
	.ds.d 4, 4
	.dc.s 4
	.dcb.s 3, 4
	.ds.s 4, 4
	.dc.x 4
	.dcb.x 3, 4
	.ds.x 4, 4
	.dc.b 4
	.dcb.b 3, 4
	.ds.b 4, 4
	.dc 4
	.dcb 3
	.ds 4
	.dc.w 4
	.dcb.w 3, 4
	.ds.w 4, 4
	.dc.a 4
	.dcb.a 3, 4
	.ds.a 4, 4
	.ds.p 4, 4
	.dc.l 4
	.dcb.l 3, 4
	.ds.l 4, 4
	.fill 20, , 'R'
	.fill 20,,0x41
	.byte 4
	.balign 4,'R'
	.byte 4
	.balign 4, 0x44
	.space 4, 'R'
	.space 4, 0x44
	.comm comm1, 4
	.lcomm comm2, 44
