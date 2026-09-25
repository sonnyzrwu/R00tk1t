#include <stdint.h>
#include <stdio.h>
// The IDT (Interrupt Descriptor Table) is a data structure used by the x86 architecture to implement interrupt handling. 
// It contains an array of descriptors, each of which defines how to handle a specific interrupt or exception. 
// Each descriptor is 16 bytes in size on 64-bit systems, and the IDT can contain up to 256 vectors (0-255).
// IDT hooking in the context of Windows kernel programming refers to the practice of modifying the IDT to 
// redirect interrupt handling to custom code. This can be used for legitimate purposes, such as implementing custom interrupt handlers, 
// but it can also be used maliciously by rootkits to intercept and manipulate system behavior.
// Microsoft has implemented various security mechanisms to prevent unauthorized modifications to the IDT,
// such as Kernel Patch Protection (KPP) and Control Flow Guard (CFG). These mechanisms are designed to protect the integrity of the kernel
// and prevent malicious code from tampering with critical system structures like the IDT. 
// Below is just a simple example of how to calculate the addresses of each vector in the IDT without actually modifying it.



// The base address of the IDT
#define IDT_BASE UINT64_C(0xfffff8068108f000)
// The number of vectors in the IDT
#define IDT_VECTOR_COUNT 256u
// The size of each vector in the IDT in 64 bit systems is 16 bytes (0x10)
#define IDT_VECTOR_SIZE UINT64_C(0x10)

void initialize_idt(void)
{
    for (unsigned int vector = 0; vector < IDT_VECTOR_COUNT; ++vector) {
        uint64_t slot_address = IDT_BASE + (uint64_t)vector * IDT_VECTOR_SIZE;
        printf("Vector %02X: descriptor slot address 0x%016llX\n",
               vector, (unsigned long long)slot_address);
    }
}

int main(void)
{
    // On x64 Windows, kernel pages are mapped as supervisor only, user-mode cannot access at this privelege level.
    // Therefore, we cannot read the IDT directly from user-mode. Instead, we will just print the calculated addresses of each vector in the IDT.
    initialize_idt();
    return 0;
}