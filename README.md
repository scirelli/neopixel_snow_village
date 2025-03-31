# neopixel_snow_village

Make sure your user is in the `dialout` group. `usermod -a -G dialout $USER`
`/etc/udev/rules.d/99-USBtiny.rules`
```
#SUBSYSTEM=="usb", ATTR{idVendor}=="1781", ATTR{idProduct}=="0c9f", GROUP="dialout", MODE="0666", SYMLINK+="ATtiny"
# usb 1-3: new low-speed USB device number 15 using xhci_hcd
#usb 1-3: New USB device found, idVendor=1781, idProduct=0c9f, bcdDevice= 1.05
#usb 1-3: New USB device strings: Mfr=1, Product=2, SerialNumber=0
#usb 1-3: Product: Trinket
#usb 1-3: Manufacturer: Adafruit

SUBSYSTEMS=="usb", ATTRS{product}=="USBtiny", ATTRS{idProduct}=="0c9f", ATTRS{idVendor}=="1781", MODE="0660", GROUP="dialout", SYMLINK+="ATtiny"
SUBSYSTEMS=="usb", ATTRS{product}=="Trinket", ATTRS{idProduct}=="0c9f", ATTRS{idVendor}=="1781", MODE="0660", GROUP="dialout", SYMLINK+="Trinket"
```

## To setup Arduino IDE
https://learn.adafruit.com/introducing-trinket/setting-up-with-arduino-ide
1. Board setup
    Add the Adafruit Board Support package!
    Paste
    `https://adafruit.github.io/arduino-board-index/package_adafruit_index.json`
    Into the "Additional Board Managers URLS" box
2. Then, select USBtinyISP from the Tools->Programmer sub-menu
3. Remember to press the reset button on the ATtiny.
