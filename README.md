

Compile the IAT Rootkit Instructions:

g++ -shared -o evil.dll .\evil.c 

g++ .\iat_hooking_attempt.c -o .\iat_hooking_attempt


Compile GDT Hook Rootkit Instructions:

1. Ensure when building, to enable MASM.
   
