notes.md

```
source /home/j0sh/Documents/projects/supercon-badge/ecp5-toolchain-linux_x86_64-v1.6.2/env.sh

make                              && \
cp -t /media/j0sh/4EED-0000 *.elf && \
sync                              && \
umount /media/j0sh/4EED-0000

make && cp -t /media/j0sh/4EED-0000 *.elf && sync
make && cp -t /media/j0sh/4EED-0000 *.elf && sync && umount /media/j0sh/4EED-0000

```

attach to serial terminal:
* green to rx, white to tx
`screen /dev/ttyUSB0 115200`
don't know how to exit screen though...