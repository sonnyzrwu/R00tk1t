
Detonated on Windows 10 and Windows 11 victim machines, blue screened.

Stop Code: SYSTEM THREAD EXCEPTION NOT HANDLED (0x7E)

Tricky part is finding the SSDT itself. 32 bit OS exports such objects.
64 bit doesn't. 

Next courses of action: Either find another way to look for SSDT, or debug FindSSDT(). Another option is to simply set a breakpoint at load, and then step through to find the cause.
