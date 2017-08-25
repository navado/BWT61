# Background
Working example how to read data from BWT61 bluetooth accelerometer on Linux.
Device: https://www.amazon.com/Bluetooth-MPU6050-module-Accelerometer-Gyroscope/dp/B018NL1R0Y#feature-bullets-btf
This code based on example provided in: https://drive.google.com/file/d/0B8PmY6nhQadKWGNJbmNoRTBtNjA/view

# Build the code
The code in example has makefile so you can compile it using gnu make
```
cd c
make
```
This will compile bt2data tool for raw data conversion

# Connecting to the device (Ubuntu linux)
Pairing the device is a bit tricky. On Linux you need to provide pre-defined pin code - 1234 and then connect. After connection the device will not appear connected (because there is no binded deamon for it's profile).
To read data from the device firt discover MAC address
```
hcitool scan
Scanning ...
        20:17:05:08:27:73       HC-06
```

Then bind this device to char device:
```
sudo rfcomm bind rfcom0 20:17:05:08:27:73
```

Then you can read data from device. Make sure your user in dialout group (or use sudo)
```
cat /dev/rfcomm0 |./bwt61_2tsv >data.csv
```
The device is disconnected when idle, so when you start reading Linux connects to the device and only after that data stream can be received.

Don't forget to release device once you finnish working
```
sudo rfcomm release rfcomm0
```


# Data
According to documentation we have 3 vectors and one scalar here
- 3-D Acceleration in G-force, marked A after conversion
- 3-D Angle in degrees, marked D after conversion
- 3-D Angular velocity in degrees/S, marked W after conversion
- Temperature in degrees Celsius, Market as T

This translated to TSV format (TAB separated) with 5 columns
- Type (A, D, W)
- X Y Z T (or Pitch Roll Yaw for D and W)
- Latest column in all records is 0 or 1 depending if checksum was wrong for this record