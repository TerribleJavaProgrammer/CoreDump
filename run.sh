cmake -S . -B build -DCMAKE_BUILD_TYPE=Release
cd build
make -j$(nproc)
cd ..
#./build/core_dump
python3 ./gui.py
