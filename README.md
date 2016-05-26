# LINGI2146-2016

Project : creating a jammer.

**Group H**

Main (executable) files are stored in `rxtx/main/` and `jammer/main/`.

Modifications related to the project are in `jammer/core/net/mac/{contikimac.c, csma.c}`.

**How to simulate :**

Upload the `rxtx.c` main file on two devices that will communicate, and upload the `jammer.c` main file on another device to jam the communication between the RX-TXs.
