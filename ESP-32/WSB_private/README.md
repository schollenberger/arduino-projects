# WSB_private

## Description

A custom Ardiomp/ESP-32 library to store secrets outside the module source code.
It is required for other ESP-32 modules in this repository.

## Installation

Edit the file `WSB_private.h` and fill in the credentials for which placeholders 
are given in this file.

Copy this directory under the library of your Arduino IDE 
(e.g. .../Documents/Arduino/libraries).

To include the secrets into your program add the line:
```
#include <WSB_private.h>
```
