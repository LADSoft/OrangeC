<LinkerConfig>
<Switch Application="dlhex.exe" Flags="/mM1 /cRESET,ROM" Name="M1" SpecFile="hex.spc" Rel="0" MapMode="0">
	<Define Name="RESETBASE" Value="0x0" />
	<Define Name="RESETSIZE" Value="0x8" />
	<Define Name="CODEBASE" Value="0x8" />
	<Define Name="RAMBASE" Value="0x10000" />
	<Define Name="STACKBASE" Value="0x20000" />
	<Define Name="STACKSIZE" Value="0x400" />
</Switch>
<Switch Application="dlhex.exe" Flags="/mM2 /cRESET,ROM" Name="M2" SpecFile="hex.spc" Rel="0" MapMode="0">
	<Define Name="RESETBASE" Value="0x0" />
	<Define Name="RESETSIZE" Value="0x8" />
	<Define Name="CODEBASE" Value="0x8" />
	<Define Name="RAMBASE" Value="0x10000" />
	<Define Name="STACKBASE" Value="0x20000" />
	<Define Name="STACKSIZE" Value="0x400" />
</Switch>
<Switch Application="dlhex.exe" Flags="/mM3 /cRESET,ROM" Name="M3" SpecFile="hex.spc" Rel="0" MapMode="0">
	<Define Name="RESETBASE" Value="0x0" />
	<Define Name="RESETSIZE" Value="0x8" />
	<Define Name="CODEBASE" Value="0x8" />
	<Define Name="RAMBASE" Value="0x10000" />
	<Define Name="STACKBASE" Value="0x20000" />
	<Define Name="STACKSIZE" Value="0x400" />
</Switch>
<Switch Application="dlhex.exe" Flags="/mI1 /cRESET,ROM" Name="I1" SpecFile="hex.spc" Rel="0" MapMode="0">
	<Define Name="RESETBASE" Value="0x0" />
	<Define Name="RESETSIZE" Value="0x8" />
	<Define Name="CODEBASE" Value="0x8" />
	<Define Name="RAMBASE" Value="0x10000" />
	<Define Name="STACKBASE" Value="0x20000" />
	<Define Name="STACKSIZE" Value="0x400" />
</Switch>
<Switch Application="dlhex.exe" Flags="/mI2 /cRESET,ROM" Name="I2" SpecFile="hex.spc" Rel="0" MapMode="0">
	<Define Name="RESETBASE" Value="0x0" />
	<Define Name="RESETSIZE" Value="0x8" />
	<Define Name="CODEBASE" Value="0x8" />
	<Define Name="RAMBASE" Value="0x10000" />
	<Define Name="STACKBASE" Value="0x20000" />
	<Define Name="STACKSIZE" Value="0x400" />
</Switch>
<Switch Application="dlhex.exe" Flags="/mI4 /cRESET,ROM" Name="I4" SpecFile="hex.spc" Rel="0" MapMode="0">
	<Define Name="RESETBASE" Value="0x0" />
	<Define Name="RESETSIZE" Value="0x8" />
	<Define Name="CODEBASE" Value="0x8" />
	<Define Name="RAMBASE" Value="0x10000" />
	<Define Name="STACKBASE" Value="0x20000" />
	<Define Name="STACKSIZE" Value="0x400" />
</Switch>
<Switch Application="dlhex.exe" Flags="/mB /cRESET,ROM" Name="BIN" SpecFile="hex.spc" Rel="0" MapMode="0">
	<Define Name="RESETBASE" Value="0x0" />
	<Define Name="RESETSIZE" Value="0x8" />
	<Define Name="CODEBASE" Value="0x8" />
	<Define Name="RAMBASE" Value="0x10000" />
	<Define Name="STACKBASE" Value="0x20000" />
	<Define Name="STACKSIZE" Value="0x400" />
</Switch>
<Switch Application="dlpe.exe" Flags="/mCON /f" Name="CON32" SpecFile="pe.spc" Rel="0" MapMode="0">
	<Extension Value=".res" />
	<Extension Value=".def" />
	<Define Name="IMAGEBASE" Value="0x400000" />
	<Define Name="FILEALIGN" Value="0x200" />
	<Define Name="OBJECTALIGN" Value="0x1000" />
	<Define Name="STACKSIZE" Value="0x100000" />
	<Define Name="STACKCOMMIT" Value="0x2000" />
	<Define Name="HEAPSIZE" Value="0x100000" />
	<Define Name="HEAPCOMMIT" Value="0x0" />
	<Define Name="OSMAJOR" Value="0x4" />
        <Define Name="OSMINOR" Value="0x0" />
	<Define Name="SUBSYSMAJOR" Value="0x4" />
        <Define Name="SUBSYSMINOR" Value="0x0" />
        <Define Name="SUBSYSTEM" Value="0x0" />
</Switch>
<Switch Application="dlpe.exe" Flags="/mGUI /f" Name="GUI32" SpecFile="pe.spc" Rel="0" MapMode="0">
	<Extension Value=".res" />
	<Extension Value=".def" />
	<Define Name="IMAGEBASE" Value="0x400000" />
	<Define Name="FILEALIGN" Value="0x200" />
	<Define Name="OBJECTALIGN" Value="0x1000" />
	<Define Name="STACKSIZE" Value="0x100000" />
	<Define Name="STACKCOMMIT" Value="0x2000" />
	<Define Name="HEAPSIZE" Value="0x100000" />
	<Define Name="HEAPCOMMIT" Value="0x0" />
	<Define Name="OSMAJOR" Value="0x4" />
        <Define Name="OSMINOR" Value="0x0" />
	<Define Name="SUBSYSMAJOR" Value="0x4" />
        <Define Name="SUBSYSMINOR" Value="0x0" />
        <Define Name="SUBSYSTEM" Value="0x0" />
</Switch>
<Switch Application="dlpe.exe" Flags="/mDLL /f" Name="DLL32" SpecFile="pe.spc" Rel="0" MapMode="0">
	<Extension Value=".res" />
	<Extension Value=".def" />
	<Define Name="IMAGEBASE" Value="0x10000000" />
	<Define Name="FILEALIGN" Value="0x200" />
	<Define Name="OBJECTALIGN" Value="0x1000" />
	<Define Name="STACKSIZE" Value="0x100000" />
	<Define Name="STACKCOMMIT" Value="0x2000" />
	<Define Name="HEAPSIZE" Value="0x100000" />
	<Define Name="HEAPCOMMIT" Value="0x0" />
	<Define Name="OSMAJOR" Value="0x4" />
        <Define Name="OSMINOR" Value="0x0" />
	<Define Name="SUBSYSMAJOR" Value="0x4" />
        <Define Name="SUBSYSMINOR" Value="0x0" />
        <Define Name="SUBSYSTEM" Value="0x0" />
</Switch>
<Switch Application="dlpm.exe" Flags="" Name="PM" SpecFile="pm.spc" Rel="0" MapMode="0">
	<Define Name="STACKSIZE" Value="0x100000" />
	<Define Name="RESERVE0" Value="0x10" />
</Switch>
<Switch Application="dlle.exe" Flags="/mLE" Name="DOS32" SpecFile="le.spc" Rel="0" MapMode="1">
	<Define Name="STACKSIZE" Value="0x100000" />
</Switch>
<Switch Application="dlle.exe" Flags="/mLX" Name="DOS32LX" SpecFile="le.spc" Rel="0" MapMode="1">
	<Define Name="STACKSIZE" Value="0x100000" />
</Switch>
<Switch Application="dlmz.exe" Flags="/mREAL" Name="REAL" SpecFile="mz.spc" Rel="0" MapMode="2">
	<Define Name="STACKSIZE" Value="0x1000" />
</Switch>
<Switch Application="dlmz.exe" Flags="/mTINY" Name="TINY" SpecFile="mzt.spc" Rel="0" MapMode="2">
</Switch>
</LinkerConfig>
