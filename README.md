# Introduction
Firmware for LiPow the open Source Lipo Battery Charger with USB C Power Delivery Based on the STM32G0

LiPow uses USB Type C with Power Delivery to charge Lithium Polymer batteries. It supports charging and balancing for 2s-4s packs. The device supports charging up to 60W.

# **LiPow Features**

- Charges and balances 2s-4s packs (single cell charging possible in future)
- USB type C input
- Supports charging up to 60W from USB PD power supplies or any other USB C port with PD source capability (such as a Thinkpad X1 laptop)
- Supports non USB PD power supplies (limited to 2.5W - 5V, 0.5A)
- Automatically reduces charging power as device temperature increases
- Charging is done through an XT60 connector and has JST XH connectors for balancing 2s-4s packs
- User feedback through an RGB LED
- Open source schematic, BOM, and firmware
- SWD and UART breakouts for firmware development
- UART command line interface for debugging and run time information

# **User Guide**

- Plug into any USB C power supply/wall wart/device that supports power output
- Plug in the balance plug from your 2s-4s battery
- If the LED turns blue, the battery needs balancing and balancing is active
- If the LED turns green, the battery is balanced
- Now plug in the XT60 plug from your battery
- If the LED turns red, the battery needs charging and charging is active
- If the LED turns green, the battery is charged and balanced
- Balancing and charging can be active at the same time and both the red and blue LEDs will be on
- Charging will only start when both the balance and XT60 plugs are connected
- If a damaged pack is attached, charging will stop if any cell rises above 4.21V
- If any cell is below 2.7V it will not charge or balance

Everything runs automatically and will charge up to the max capability of the connected USB PD power supply if the max current output limit exceeds the input power supply. Lower current limits can be programmed as well.

Charging current is decided by the USB PD Source capability. First, it checks the available voltages from the source, then selects the voltage that will result in the highest efficiency for the regulator based on the number of cells. For instance, using a 30W supply with a 20V 1.5A (30W) capability and a 4s Lipo battery at 15.0V. The charging current will be 30W/15.0V=2A. As the battery voltage increases, the max charging current will decrease. 30W/16.0V=1.875A.

# **Developer Guide**
- Program through SWD or UART
- Runs FreeRTOS
- ST USB PD Middleware
- UART Command Line Interface (921600 baud rate, 8N1)
- Build using makefile or in TrueStudio


To load firmware through SWD use a JLINK or STLINK.

To load firmware through UART, use one of these tools:

https://www.st.com/en/development-tools/stm32cubeprog.html

https://sourceforge.net/projects/stm32flash/

To place the STM32G0 into bootloader mode and enable UART firmware loading, jumper BOOT0 to 3.3V before powering on. Use one of the above programs with UART to load the firmware. All necessary pins are located on the debug header shown below.

# **Hardware Specifications**

- STM32G071CBT6 microcontroller with built in USB PD Phy
- BQ25703ARSNR programmable regulator (could be used as programmable power supply in future)
- IFX25001MEV33HTSA1 3.3V regulator supporting up to 45V input
- USB Type C connector power input
- XT60 connector for charging
- JST XH connectors (3, 4, and 5 pin) for balancing and pack cell count detection

# **Debug Header Pinout**

![LiPow CLI](https://i.imgur.com/APBez16.png "LiPow Debug Header")

# **Sample of the CLI**

![LiPow CLI](https://i.imgur.com/6QrrqDk.png "LiPow CLI")

# Where to Buy

You can purchase a unit [here](https://www.tindie.com/products/arkelectronics/lipow-the-usb-c-lipo-battery-charger/) or a bare PCB [here](https://www.tindie.com/products/arkelectronics/lipow-the-usb-c-lipo-battery-charger-bare-pcb/).

# Questions, Comments, and Contributions

Pull requests are welcome. If you have questions or comments, you can email me directly at alex@arkelectron.com. 
