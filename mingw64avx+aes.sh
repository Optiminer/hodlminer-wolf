make clean

rm -f config.status

# icon
windres res/icon.rc icon.o

CFLAGS="-O3 -mavx -maes -Wall" CXXFLAGS="$CFLAGS -std=gnu++11 -flto -fpermissive" ./configure --with-curl

make 

strip -p --strip-debug --strip-unneeded hodlminer.exe

mv hodlminer.exe hodlminer_opt_AVX_AES.exe

