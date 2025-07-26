# RotaryDiskKeyboard-ESP32

This software is for an Arduino Nano ESP32.
It acts as an USB keyboard and converts impulses from a rotary disk
into digits transmitted via USB to any keyboard supporting software.
It is forked from [antonmeyer](https://github.com/antonmeyer)’s
[WaehlscheibeHID](https://github.com/antonmeyer/WaehlscheibeHID) repository.

[example at youtube](https://www.youtube.com/watch?v=h1p7Oikf-b4)

Impulse contact (`nsi`) is connected to pin 12 and `nsa` to pin 13. `nsa` gets closed
as soon as the disk is revolved. `nsi` starts generating impulses as soon as the
disk is released and rotates back.

Needs only 3 wires: `nsi`, `nsa`, and 1x ground to the 2 switches. Internal pull-up is used
for pin 12 and 13.

If you connect it to your computer (OS does not matter, as long as it supports USB
[HID](https://en.wikipedia.org/wiki/Human_interface_device)), you might get
an error message. This is for the programming serial line, also implemented on that USB device.
You can ignore that, after a moment the rotary disk keyboard should be ready.

And if you think twice, impulse dialing is digital dialing. Since 1913 (or so).
