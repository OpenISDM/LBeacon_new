# LBeacon

LBeacon (Location Beacon): BeDIPS uses LBeacons to deliver to users the 3D coordinates and textual descriptions of their locations. Basically, LBeacon is a low-cost, Bluetooth Smart Ready device. At deployment and maintenance times, the 3D coordinates and location description of every LBeacon are retrieved from BeDIS (Building/environment Data and Information System) and stored locally. Once initialized, each LBeacon broadcast its coordinates and location description to Bluetooth enabled mobile devices nearby within its coverage area.

Alpha version of LBeacon was demonstrated during Academia Sinica open house and will be experimented with and assessed by collaborators of the project.

### Installing LBeacon on Raspberry Pi

[Download](https://www.raspberrypi.org/downloads/raspbian/) Raspbian Jessie lite for the Raspberry Pi operating system and follow its installation guide.

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
### Building libxbee3 library 
If you are building libxbee, then there are a number of options avaliable to you.<br />
Initially you should run the following command:
<pre><code>$ make configure</code></pre>
	
This will retrieve a default `config.mk` that is suitable for your Respberry Pi.<br />
In our project you need to<br />
un-comment `OPTIONS+=       XBEE_LOG_LEVEL=100`<br />
comment `OPTIONS+=       XBEE_LOG_RX_DEFAULT_OFF`<br />
comment `OPTIONS+=       XBEE_LOG_TX_DEFAULT_OFF`<br />
un-comment `OPTIONS+=       XBEE_NO_RTSCTS`<br />

You should review this file and then run the following command:
<pre><code>$ make all</code></pre>

After the build process has completed, you should find suitable files in **./lib**.<br />
E.g: for a Unix-like OS you can expect to find **.so** and **.a** files<br />
        for Windows you can expect to find a **.dll** file<br />

It is highly recommended that you don't modify any of the build system.


### Installation of libxbee library 
To install libxbee simply type (you will require **root permissions**):
<pre><code>$ sudo make install</code></pre>


### Libxbee library usage 
Compile your applications, including **xbee.h** in the relevant source files.<br />
Ensure you link with libxbee (e.g: using `gcc -lxbee`)

If you are compiling the object file directly into your executable instead
of making use of the library,<br />you must include the following link flags:
`-lpthread -lrt`<br />

### Compiling and Running LBeacon
```sh
cd LBeacon/src
sudo make clean
make 
sudo ./LBeacon
```
