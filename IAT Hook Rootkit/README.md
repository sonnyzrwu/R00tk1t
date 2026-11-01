
This Rootkit Prototype Hooks the IAT of "explorer.exe", specifically hooking the API call CreateProcessW.
We confirm that we have successfully hooked the victim's IAT.

IN THE FUTURE:
1. The rootkit itself doesn't have a payload, it is simply a blank round. Developing other modules for Key logging and C2 exfiltration.
2. CreateProcessW hooked in DLL=api-ms-win-core-processthreads-l1-1-0.dll. However, upon double clicking any executable, CreateProcessW isn't necessarily invoked after double clicking, as this is confirmed via MessageBoxA. Try hooking something else?
