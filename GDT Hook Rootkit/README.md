
RMK: GDT 
Managed to successfully traverse GDT, wrote a function that tries to find an empty slot.
Obstacle: Current buffer in GDT only allows for 11 entries, all 11 are filled.
No room to install Call Gate or malicious Target Code Segment.

Next COA: Attempt to allocate a new memory buffer that is sufficiently large enough.

typedef struct _GDTR {
	USHORT limit; // 16 bits <------- MAKE THIS LARGER
	ULONG64 baseAddress; //  64 bits
} GDTR, * PGDTR;

