make clean

./autogen.sh

# icon
windres res/icon.rc icon.o

CFLAGS="-O3 -mavx2 -maes -Wall" CXXFLAGS="$CFLAGS -std=gnu++11 -flto -fpermissive" ./configure --with-curl

make -j8

strip -p --strip-debug --strip-unneeded hodlminer.exe

mv hodlminer.exe hodlminer_AVX2_AES.exe
