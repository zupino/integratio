# integratio
Configurable user-space network stack, aimed to simulate network error condition, delays and vulnerabilities.

Dependencies

This was checked on fresh Ubuntu 18.04 on VMWare virtual machine.

`sudo apt update`
`sudo apt install git`
`sudo apt install libboost-all-dev`

Installation of libtins

`git clone https://github.com/mfontanini/libtins.git`
`apt-get install libpcap-dev libssl-dev cmake`
`mkdir build`
`cd build`
`cmake ../ -DLIBTINS_ENABLE_CXX11=1`
`make`
`sudo make install`
`sudo ldconfig`

Clone integratio on home dir

`git clone https://github.com/zupino/integratio.git`
`cd integratio`
`make`

Also, remember to block RST packet from the server machine with the following iptable rule

NOTE    This rule will check both RST and PSH flags, and drop only the RST=1 and PSH=0
        In this way, we can block the RST from kernel, but let the RST PSH from test cases

`iptables -A OUTPUT -p tcp --tcp-flags RST,PSH RST -j DROP`

