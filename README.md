## Work in progress
- Brought to you by AMD-V :D


## Todo list
- Fixes:
    - Fix msr handling for synthetic msrs, currently injecting #GP crashes the system
    - Implement exception handling within the host to safely reflect any potential exceptions back into the guest including msrs instead of strictly injecting an exception or returning nothing
    - Fix unload routine to work for usermode calls, currently only works through `sc stop ...`
    - Fix npt splitting mechanism and ensure proper tlb flushing when hiding pages, perhaps through `invlpgb` to broadcast an npt flush

- QOL:
    - Hide vm overhead if necessary
    - Add a logger class, fmtlib formatting style
