Public Function AddComplex(a As Complex, b As Complex) As Complex

AddComplex.re = a.re + b.re
AddComplex.im = a.im + b.im

End Function

Public Function MultiplyComplex(a As Complex, b As Complex) As Complex

MultiplyComplex.re = a.re * b.re - a.im * b.im
MultiplyComplex.im = a.re * b.im + a.im * b.re

End Function

Public Function Conjugate(a As Complex) As Complex

Conjugate.re = a.re
Conjugate.im = -a.im

End Function

Public Function Modulo(a As Complex) As Double

Modulo = Sqr(a.re ^ 2 + a.im ^ 2)

End Function

Public Function Argument(a As Complex) As Double

Dim i As Integer
Dim v(1 To 4) As Double

If (z.re = 0) And (z.im = 0) Then
    Argument = -10
    End Function
End If

v(1) = ArcSin(z.im / Modulo(z))
v(2) = mcPI - v(1)

v(3) = ArcCos(z.re / Modulo(z))
v(4) = -1 * v(3)

For i = 1 To 4

    While v(i) > mcPI
        v(i) = v(i) - 2 * mcPI
    Wend

    While v(i) < mcPI
        v(i) = v(i) + 2 * mcPI
    Wend

Next i

If v(1) = v(3) Then Argument = v(1)
If v(2) = v(3) Then Argument = v(2)

If v(1) = v(4) Then Argument = v(1)
If v(2) = v(4) Then Argument = v(2)

End Function

' Code by : Steven Roland Bazinet (ArcSin function only)

Private Function ArcSin(vntSine As Variant) As Double
On Error GoTo ERROR_ArcSine

Const cOVERFLOW = 6

Dim blnEditPassed As Boolean
Dim dblTemp       As Double

    blnEditPassed = False
    If IsNumeric(vntSine) Then
        If vntSine >= -1 And vntSine <= 1 Then
            blnEditPassed = True
                                                                
            dblTemp = Sqr(-vntSine * vntSine + 1)
            If dblTemp = 0 Then
                ArcSin = Sgn(vntSine) * pi / 2
            Else
                ArcSin = Atn(vntSine / dblTemp)
            End If
        End If
    End If

EXIT__ArcSine:
    If Not blnEditPassed Then Err.Raise cOVERFLOW
    Exit Function

ERROR_ArcSine:
    On Error GoTo 0
    blnEditPassed = False
    Resume EXIT__ArcSine

End Function

' Code by : Me, based (very much!) on Steven R Bazinet's code

Private Function ArcCos(vntcos As Variant) As Double
On Error GoTo ERROR_ArcSine

Const cOVERFLOW = 6

Dim blnEditPassed As Boolean
Dim dblTemp       As Double

    blnEditPassed = False
    If IsNumeric(vntcos) Then
        If vntcos >= -1 And vntcos <= 1 Then
            blnEditPassed = True
                                                                
            dblTemp = Sqr(-vntcos * vntcos + 1)
            If dblTemp = 0 Then
                ArcCos = Sgn(vntcos) * pi / 2
            Else
                ArcCos = Atn(dblTemp / vntcos)
            End If
        End If
    End If

EXIT__ArcCos:
    If Not blnEditPassed Then Err.Raise cOVERFLOW
    Exit Function

ERROR_ArcCos:
    On Error GoTo 0
    blnEditPassed = False
    Resume EXIT__ArcSine

End Function