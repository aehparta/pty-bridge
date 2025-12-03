```
Pseudoterminal bridge
Bridges data between pseudoterminals (ptys) and a physical serial port.

Options:
  -h, --help                  display this help and exit
  -v, --ptys=VALUE            count of ptys to create
                              (default: 2)
  -d, --device=VALUE          serial port device
  -b, --baudrate=VALUE        serial port baudrate, accepts: 1200, 2400,
                              9600, 19200, 38400, 57600, 115200
                              (default: 9600)

Create 3 ptys bridged with /dev/ttyUSB0:
  bridge -v 3 -d /dev/ttyUSB0 -b 9600

Create 4 bridged ptys without real serial port:
  bridge -v 4

Note: created ptys are symlinked to /dev/ttyPTYBR* if the program is run as root.
```
