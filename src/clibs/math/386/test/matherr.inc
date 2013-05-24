
%define DOMAIN 1
%define SING 2
%define OVERFLOW 3
%define UNDERFLOW 4
%define TLOSS 5
%define STACKFAULT 7

[extern trigdomain]
[extern clearmath]
[extern wrapmath]
[extern ___matherr]
[extern sinularerr]
[extern mnegerr]
[extern mnegeerr]
[extern domainerr]
[extern checkedscale]
[extern checkinvalid]
[extern overflow]
[extern poptemp]

%imacro popone 0.nolist
    fstp tword [poptemp]
%endmacro
%imacro poptwo 0.nolist
    fstp tword [poptemp]
    fstp tword [poptemp]
%endmacro
