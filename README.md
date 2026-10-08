

Compile the Rootkit Instructions:
g++ -shared -o evil.dll .\evil.c \n
g++ .\iat_hooking_attempt.c -o .\iat_hooking_attempt
