cd buddy-2.4
./configure
make
make install

export LD_LIBRARY_PATH=/usr/local/lib # (sudo find / -name libbdd.so.0)

cd proj
g++ simple.cpp -o simple -lbdd
./simple

g++ simple.cpp -o simple -lbdd && ./simple > res.txt && head res.txt
