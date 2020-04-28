# LBeacon

LBeacon (Location Beacon): BeDIPS uses LBeacons to deliver to users the 3D coordinates and textual descriptions of their locations. Basically, LBeacon is a low-cost, Bluetooth Smart Ready device. At deployment and maintenance times, the 3D coordinates and location description of every LBeacon are retrieved from BeDIS (Building/environment Data and Information System) and stored locally. Once initialized, each LBeacon broadcast its coordinates and location description to Bluetooth enabled mobile devices nearby within its coverage area.

Alpha version of LBeacon was demonstrated during Academia Sinica open house and will be experimented with and assessed by collaborators of the project.

## General Installation

### Installing LBeacon on Raspberry Pi

[Download](http://downloads.raspberrypi.org/raspbian_lite/images/) Raspbian Jessie lite for the Raspberry Pi operating system and follow its installation guide.

In Raspberry Pi, install packages by running the following command:
```sh
sudo apt-get update
sudo apt-get upgrade
sudo apt-get install -y git bluez libbluetooth-dev fuse libfuse-dev libexpat1-dev swig python-dev ruby ruby-dev libusb-1.0-0-dev default-jdk xsltproc libxml2-utils cmake doxygen
```

### Change Username
To avoid path issues, Change username `pi` to `bedis`

### Install related packages

Clone related packages from Github
```sh
git clone https://github.com/OpenISDM/libEncrypt.git
git clone https://github.com/OpenISDM/BeDIS_library.git
```

move files in `BeDIS_library/` to `LBeacon/import/`
```sh
 mv ./BeDIS_library/* ./LBeacon/import/
```

Install libEncrypt and move the file to the specified path
```sh
mv ./libEncrypt/key/Key_example.h ./libEncrypt/key/Key.h
cd ./libEncrypt/src/
make
cd
mkdir bot-encrypt
mv ./libEncrypt/src/libEncrypt.so ./bot-encrypt/
```

Update the links/cache the dynamic loader

In file `/etc/ld.so.conf` add a new line `/home/bedis/bot-encrypt/`
```sh
sudo ldconfig -v
```

### Installation of Zlog library
Download the lastest version (1.2.12.tar.gz) of the library:
[Download here](https://github.com/HardySimpson/zlog/releases) <br />
<pre>
tar -zxvf zlog-latest-stable.tar.gz
cd zlog-latest-stable/
make 
sudo make install
</pre>
After installation, refresh your dynamic linker to make sure your program can find zlog library.
<pre><code>sudo nano /etc/ld.so.conf </code></pre>

Add one line to the file:
/usr/local/lib <br />

<pre><code>sudo ldconfig -v</code></pre>


### Installing and Compiling LBeacon
```sh
git clone https://github.com/OpenISDM/LBeacon.git
cd LBeacon/src
sudo make clean
make 
```

## Link for referance
* Libxbee: <https://github.com/attie/libxbee3>
* Zlog: <https://github.com/HardySimpson/zlog>
* Lbeacon_Zigbee: <https://github.com/OpenISDM/Lbeacon-zigbee>
* LBeacon_Gateway: <https://github.com/OpenISDM/Lbeacon-Gateway>

