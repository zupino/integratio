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

stuck here:
/json.hpp:6321:62: error: wrong number of template arguments (1, should be 2)
590return *lhs.m_value.array < *rhs.m_value.array;
