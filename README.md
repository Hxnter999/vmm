## Work in progress
- Brought to you by AMD-V :D
- Although the project can run and works fine, there are issues with certain mechanisms as mentioned in the following todo-list which need some changes to be fixed, currently these features are turned off to remain stable.


## Todo list
- Fixes:
    - Fix msr handling for synthetic msrs, currently injecting #GP crashes the system
    - Implement exception handling within the host to safely reflect any potential exceptions back into the guest including msrs instead of strictly injecting an exception or returning nothing
    - Fix unload routine to work for usermode calls, currently only works for cpl0 unlaod calls.
    - Fix npt splitting mechanism and ensure proper tlb flushing when hiding pages, perhaps through `invlpgb` to broadcast an npt flush

- QOL:
    - Hide vm overhead if necessary
    - Add a logger class, fmtlib formatting style
	
## Customization
Everything regarding the result of the guest and the host is within `src/vmm.cpp`, theres separate functions that setup the guest and the host. If you want to customize the guest, perhaps loop at [this function](https://github.com/Hxnter999/vmm/blob/main/source/vmm.cpp#L101).

