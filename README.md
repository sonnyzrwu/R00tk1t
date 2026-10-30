

Compile the IAT Rootkit Instructions:

//g++ -shared -o evil.dll .\evil.c 

//g++ .\iat_hooking_attempt.c -o .\iat_hooking_attempt

g++ -shared -o evil.dll .\evil.c .\process_injection.c

g++ .\iat_hooking_attempt.c -o .\iat_hooking_attempt .\process_injection.c


Compile GDT Hook Rootkit Instructions:

1. Ensure when building, to enable MASM.
   
