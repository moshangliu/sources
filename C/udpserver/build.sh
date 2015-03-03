export LD_LIBRARY_PATH=$LD_LIBRARY_PATH:"/opt/soft/tcpsvr/gmp-4.3.2/lib"
export LD_LIBRARY_PATH=$LD_LIBRARY_PATH:"/opt/soft/tcpsvr/mpc-0.8.1/lib"
export LD_LIBRARY_PATH=$LD_LIBRARY_PATH:"/opt/soft/tcpsvr/mpfr-2.4.2/lib"
export LD_LIBRARY_PATH=$LD_LIBRARY_PATH:"/opt/soft/tcpsvr/gcc-4.8.1/lib"
export LD_LIBRARY_PATH=$LD_LIBRARY_PATH:"/opt/soft/tcpsvr/gcc-4.8.1/lib64"

make clean && make
