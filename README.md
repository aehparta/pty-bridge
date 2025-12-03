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

# Example
**Terminal 1**
```sh
user:~/pty-bridge$ sudo ./pty-bridge -v 3 -d /dev/ttyUSB0 
serial port /dev/ttyUSB0 at 9600 baud
pty: /dev/ttyPTYBR0 -> /dev/pts/10
pty: /dev/ttyPTYBR1 -> /dev/pts/11
pty: /dev/ttyPTYBR2 -> /dev/pts/12
```

**Terminal 2**
```sh
user:~$ echo "Hello!" > /dev/ttyPTYBR0
```

**Terminal 3**
```sh
Welcome to minicom 2.9

OPTIONS: I18n 
Port /dev/ttyPTYBR1, 16:47:19

Press CTRL-A Z for help on special keys

Hello!
```

**Terminal 4**
```sh
screen /dev/ttyPTYBR2 115200

...

Hello!
```
