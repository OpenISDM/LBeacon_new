# LBeacon

LBeacon (Location Beacon): BeDIPS uses LBeacons to deliver to users the 3D coordinates and textual descriptions of their locations. Basically, LBeacon is a low-cost, Bluetooth Smart Ready device. At deployment and maintenance times, the 3D coordinates and location description of every LBeacon are retrieved from BeDIS (Building/environment Data and Information System) and stored locally. Once initialized, each LBeacon broadcast its coordinates and location description to Bluetooth enabled mobile devices nearby within its coverage area.

Alpha version of LBeacon was demonstrated during Academia Sinica open house and will be experimented with and assessed by collaborators of the project.

### Installing LBeacon on Raspberry Pi

[Download](https://www.raspberrypi.org/downloads/raspbian/) Raspbian stretch lite for the Raspberry Pi operating system and follow its installation guide.

In Raspberry Pi, install packages by running the following command:
```sh
sudo apt-get update
sudo apt-get upgrade
sudo apt-get install -y git bluez libbluetooth-dev fuse libfuse-dev libexpat1-dev swig python-dev ruby ruby-dev libusb-1.0-0-dev default-jdk xsltproc libxml2-utils cmake doxygen
```
Download open source code for obexftp and openobex libraries:
```sh
git clone https://gitlab.com/openobex/mainline openobex
git clone https://gitlab.com/obexftp/mainline obexftp
```
To compile and install openobex and openftp libraries:
```sh
cd openobex
mkdir build
cd build
cmake ..
sudo make
sudo make install
cd ../../
cd obexftp
mkdir build
cd build
cmake ..
sudo make
sudo make install
```
Update the links/cache the dynamic loader uses:
```sh
sudo ldconfig -v
```

### Compiling and Running LBeacon
```sh
cd LBeacon/src
gcc LBeacon.c Utilities.c LinkedList.c Queue.c -g -o LBeacon -L/usr/local/lib -lrt -lpthread -lmulticobex -lbfb -lbluetooth -lobexftp -lopenobex
sudo ./LBeacon
```