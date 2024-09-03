#ifndef _DLGS_H
#define _DLGS_H

#ifdef __ORANGEC__ 
#pragma GCC system_header
#endif

/* Windows UI dialog definitions */

#define FILEOPENORD  1536
#define MULTIFILEOPENORD  1537
#define PRINTDLGORD  1538
#define PRNSETUPDLGORD  1539
#define FINDDLGORD  1540
#define REPLACEDLGORD  1541
#define FONTDLGORD  1542
#define FORMATDLGORD31  1543
#define FORMATDLGORD30  1544
#define RUNDLGORD  1545
#define PAGESETUPDLGORD  1546
#define NEWFILEOPENORD  1547
#define NEWOBJECTOPENORD  1548
#define PRINTDLGEXORD  1549
#define PAGESETUPDLGORDMOTIF  1550
#define COLORMGMTDLGORD  1551
#define NEWFILEOPENV2ORD  1552

#if (NTDDI_VERSION >= NTDDI_VISTA)
#define NEWFILEOPENV3ORD  1553
#endif /* NTDDI_VERSION >= NTDDI_VISTA */

#if (NTDDI_VERSION >= NTDDI_WIN7)
#define NEWFORMATDLGWITHLINK  1591
#define IDC_MANAGE_LINK  1592
#endif

#define ctlFirst  0x400
#define ctlLast  0x4FF

#define chx1  0x410
#define chx2  0x411
#define chx3  0x412
#define chx4  0x413
#define chx5  0x414
#define chx6  0x415
#define chx7  0x416
#define chx8  0x417
#define chx9  0x418
#define chx10  0x419
#define chx11  0x41A
#define chx12  0x41B
#define chx13  0x41C
#define chx14  0x41D
#define chx15  0x41E
#define chx16  0x41F
#define cmb1  0x470
#define cmb2  0x471
#define cmb3  0x472
#define cmb4  0x473
#define cmb5  0x474
#define cmb6  0x475
#define cmb7  0x476
#define cmb8  0x477
#define cmb9  0x478
#define cmb10  0x479
#define cmb11  0x47A
#define cmb12  0x47B
#define cmb13  0x47C
#define cmb14  0x47D
#define cmb15  0x47E
#define cmb16  0x47F
#define edt1  0x480
#define edt2  0x481
#define edt3  0x482
#define edt4  0x483
#define edt5  0x484
#define edt6  0x485
#define edt7  0x486
#define edt8  0x487
#define edt9  0x488
#define edt10  0x489
#define edt11  0x48A
#define edt12  0x48B
#define edt13  0x48C
#define edt14  0x48D
#define edt15  0x48E
#define edt16  0x48F
#define frm1  0x434
#define frm2  0x435
#define frm3  0x436
#define frm4  0x437
#define grp1  0x430
#define grp2  0x431
#define grp3  0x432
#define grp4  0x433
#define ico1  0x43C
#define ico2  0x43D
#define ico3  0x43E
#define ico4  0x43F
#define lst1  0x460
#define lst2  0x461
#define lst3  0x462
#define lst4  0x463
#define lst5  0x464
#define lst6  0x465
#define lst7  0x466
#define lst8  0x467
#define lst9  0x468
#define lst10  0x469
#define lst11  0x46A
#define lst12  0x46B
#define lst13  0x46C
#define lst14  0x46D
#define lst15  0x46E
#define lst16  0x46F
#define psh1  0x400
#define psh2  0x401
#define psh3  0x402
#define psh4  0x403
#define psh5  0x404
#define psh6  0x405
#define psh7  0x406
#define psh8  0x407
#define psh9  0x408
#define psh10  0x409
#define psh11  0x40A
#define psh12  0x40B
#define psh13  0x40C
#define psh14  0x40D
#define psh15  0x40E
#define pshHelp  0x40E
#define psh16  0x40F
#define rad1  0x420
#define rad2  0x421
#define rad3  0x422
#define rad4  0x423
#define rad5  0x424
#define rad6  0x425
#define rad7  0x426
#define rad8  0x427
#define rad9  0x428
#define rad10  0x429
#define rad11  0x42A
#define rad12  0x42B
#define rad13  0x42C
#define rad14  0x42D
#define rad15  0x42E
#define rad16  0x42F
#define rct1  0x438
#define rct2  0x439
#define rct3  0x43a
#define rct4  0x43b
#define scr1  0x490
#define scr2  0x491
#define scr3  0x492
#define scr4  0x493
#define scr5  0x494
#define scr6  0x495
#define scr7  0x496
#define scr8  0x497
#define stc1  0x440
#define stc2  0x441
#define stc3  0x442
#define stc4  0x443
#define stc5  0x444
#define stc6  0x445
#define stc7  0x446
#define stc8  0x447
#define stc9  0x448
#define stc10  0x449
#define stc11  0x44A
#define stc12  0x44B
#define stc13  0x44C
#define stc14  0x44D
#define stc15  0x44E
#define stc16  0x44F
#define stc17  0x450
#define stc18  0x451
#define stc19  0x452
#define stc20  0x453
#define stc21  0x454
#define stc22  0x455
#define stc23  0x456
#define stc24  0x457
#define stc25  0x458
#define stc26  0x459
#define stc27  0x45A
#define stc28  0x45B
#define stc29  0x45C
#define stc30  0x45D
#define stc31  0x45E
#define stc32  0x45F

typedef struct tagCRGB {
    BYTE bRed;
    BYTE bGreen;
    BYTE bBlue;
    BYTE bExtra;
} CRGB;

#endif /* _DLGS_H */
