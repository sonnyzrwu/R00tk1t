Update: 2026-10-29
Detonated on Windows 10 and Windows 11 victim machines, blue screened.

Stop Code: SYSTEM THREAD EXCEPTION NOT HANDLED (0x7E)

Tricky part is finding the SSDT itself. 32 bit OS exports such objects.
64 bit doesn't. 

Next courses of action: Either find another way to look for SSDT, or debug FindSSDT(). Another option is to simply set a breakpoint at load, and then step through to find the cause. Option 3 is to downgrade to an even dumber version of Windows, or make it 32 bit so you can simply read the exported SSDT from the kernel.

This guy actually managed to defeat PatchGuard!!!
https://github.com/0xflux/Hells-Hollow/blob/master/readme.md
https://fluxsec.red/hells-hollow-a-new-SSDT-hooking-technique-with-alt-syscalls-rootkit


Update: 2026-11-07, We found the SSDT!

0: kd> dq nt!KeServiceDescriptorTable L4

fffff803`e66018c0  fffff803`e54dadc0 00000000`00000000

fffff803`e66018d0  00000000`000001e9 fffff803`e54db568

0: kd> g
KeServiceDescriptorTable ServiceTable  FFFFF803E54DADC0 

KeServiceDescriptorTable Count         0000000000000000 

KeServiceDescriptorTable Limit         00000000000001e9 

KeServiceDescriptorTable ArgumentTable FFFFF803E54DB568 

Break instruction exception - code 80000003 (first chance)


Update: 2026-11-08

Obtained the error:


>sc start [NAMEOFFILE] [SC] StartService FAILED 577: Windows cannot verify the digital signature for this file. A recent hardware or software change might have installed a file that is signed incorrectly or damaged, or that might be malicious software from an unknown source.

FIND ANOTHER WAY TO DETONATE


