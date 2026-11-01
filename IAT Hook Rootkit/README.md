
This Rootkit Prototype Hooks the IAT of "explorer.exe", specifically hooking the API call CreateProcessW.
We confirm that we have successfully hooked the victim's IAT.

IN THE FUTURE:
The rootkit itself doesn't have a payload, it is simply a blank round. Developing other modules for Key logging and C2 exfiltration.
