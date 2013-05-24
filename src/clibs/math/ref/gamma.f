FUNCTION GAMMP(A,X)
   Returns the incomplete gamma function P(a,x)
IF (X.LT.0. .OR. A.LE.0.)PAUSE
IF (X.LT.A+1.) THEN     //Use the series representation.
     CALL GSER(GAMSER,A,X,GLN)
     GAMMP=GAMSER
ELSE     //Use the continued fraction representation      CALL GCF(GAMMCF,A,X,GLN)
     GAMMP=1.-GAMMCF
ENDIF
RETURN
END




FUNCTION GAMMQ(A,X)
   Returns the incomplete gamma function Q(a,x) = 1 - P(a,x).
IF (X. LT. 0. .OR. A.LE.0.) PAUSE
IF(X.LT.A+1.)THEN     //Use the series representation
     CALL GSER(GAMSER,A,X,GLN)
     GAMMQ=1.-GAMSER     //and take its complement.
ELSE     //Use the continued fraction representation.
     CALL GCF(GAMMCF,A,X,GLN)
     GAMMQ=GAMMCF
ENDIF
RETURN
END



The argument GLN is returned by both the series and continued fraction, procedures containing the value ln G(a); the reason for this is so that it is available to you if you want to modify the above two procedures to give ?(a,x) and G(a,x), in addition to P(a,x) and Q(a,x) (cf. equations 6.2.1 and 6.2.3). 

The procedures GSER and GCF which implement (6.2.5) and (6.2.6) are 



SUBROUTINE GSER(GAMSER,A,X,GLN)
Returns the incomplete gamma function P(a,x) evaluated by its series-representation as
GAMSER. Also returns ln G(a) as GLN.
PARAMETER (ITMAX=100,EPS=3.E-7)
GLN=GAMMLN(A)
IF(X.LE.0.)THEN
     IF (X.LT.0.)PAUSE
     GAMSER=0.
     RETURN
ENDIF
AP=A
SUM=1./A
DEL=SUM
DO 11 N=1,ITMAX
     AP=AP+1.
     DEL=DEL*X/AP
     SUM=SUM+DEL
     IF(ABS(DEL).LT.ABS(SUM)*EPS)GO TO 1
11 CONTINUE
  PAUSE 'A too large, ITMAX too small'
1 GAMSER=SUM*EXP(-X+A*LOG(X)-GLN)
RETURN
END




SUBROUTINE GCF(GAMMCF,A,X,GLN)    Returns the incomplete gamma function Q(a,x) evaluated by its continued fraction
   representation as GAMMCF. Also returns G(a) as GLN.
PARAMETER (ITMAX=100,EPS=3.E-7)
GLN=GAMMLN(A)
GOLD=0.     //This is the previous value, tested against for convergence.
A0=1.
A1=X     //We are here setting up the A's and B's of equation (5.2.4) for evaluating the continued fraction.
B0=0.
B1=1.
FAC=1.     //FAC is the renormalization factor for preventing overflow of the partial numerators and denominators.
DO 11 N=1,ITMAX
     AN=FLOAT(N)
     ANA=AN-A
     A0=(A1+A0*ANA)*FAC     //One step of the recurrence (5.2.5).
     B0=(B1+B0*ANA)*FAC
     ANF=AN+FAC
     A1=X*A0+ANF*A1     //The next step of the recurrence (5.2.5).
     B1=X+B0+ANF*B1
     IF(A1.NE.0.)THEN     //Shall we renormalize?
          FAC=1./A1     //Yes. Set FAC so it happens.
          G=B1*FAC     //New value of answer.
          IF(ABS((G-GOLD)/G).LT.EPS)GO TO 1     //Converged? If so, exit.
          GOLD=G     //If not, save value.
     ENDIF
11 CONTINUE
  PAUSE 'A too large, ITMAX too small'
1 GAMMCF=EXP(-X+A*ALOG(X)-GLN)*G     //Put factors In front.
RETURN
END




Below is function GAMMLN used by GSER and GCF above. There are a variety of methods in use for calculating the function G(z) numerically, but none is quite as neat as the approximation derived by Lanczos. This scheme is entirely specific to the gamma function, seemingly plucked from thin air. We will not attempt to derive the approximation. 



FUNCTION GAMMLN(XX)
   Returns the value ln[G(XX)] for XX > 0.    Full accuracy is obtained for XX > 1. REAL*8 COF(6),STP,HALF,ONE,FPF,X,TMP,SER
   Internal arithmetic will be done in double precision,    a nicety that you can omit if five-figure accuracy is good enough. DATA COF,STP/76.18009173D0,-86.50532033D0,24.01409822D0,
*    -1.231739516D0,.120858003D-2,-.536382D-5,2.50662827465D0/
DATA HALF,ONE,FPF/0.5D0,1.0D0,5.5D0/
X=XX-ONE
TMP=X+FPF
TMP=(X+HALF)*LOG(TMP)-TMP
SER=ONE
DO 11 J=1,6
     X=X+ONE
     SER=SER+COF(J)/X
11 CONTINUE
GAMMLN=TMP+LOG(STP*SER)
RETURN
END

FUNCTION ERF(X)
   Returns the error function erf(X).
IF(X.LT.0.)THEN
     ERF=-GAMMP(.5,X**2)
ELSE
     ERF=GAMMP(.5,X**2)
ENDIF
RETURN
END




FUNCTION ERFC(X)
   Returns the complementary error function erfc(X).
IF( X.LT.0.)THEN
     ERFC=1.+GAMMP(.5,X**2)
ELSE
     ERFC=GAMMQ(.5,X**2)
ENDIF
RETURN
END

