Para ejecutar la prueba del proyecto se requiere la instalación de Migen, Litex, ISE, el toolchain del LM32 y las herramientas de la tarjeta en que se vaya a montar el SoC.

1. Abrir una terminal en la carpeta del proyecto.
2. Ejecutar make -C firmware/ clean all
3. Sintetizar con python3 base.py
4. Cargar el SoC a la FPGA con python3 load.py
5. Cargar el Firmware al procesador con sudo python3 litex_term.py /dev/ttyUSBX --kernel firmware/firmware.bin
