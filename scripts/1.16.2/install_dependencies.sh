#!/bin/bash

sudo -E apt install libeigen3-dev libdlib-dev libudev-dev libpciaccess-dev opencl-headers

git clone https://github.com/json-c/json-c.git
cd json-c
git checkout f8c632f579c71012f9aca81543b880a579f634fc
./autogen.sh
./configure && make && sudo make install
cd ..
rm -rf json-c

wget https://github.com/intel/libva/archive/2.9.0.tar.gz -O libva.tar.gz
tar zxf libva.tar.gz; mv libva-2.9.0 libva
cd libva
./autogen.sh --prefix=/usr --libdir=/usr/lib/x86_64-linux-gnu; make -j4; sudo make install
cd ..

wget https://github.com/intel/gmmlib/archive/intel-gmmlib-20.3.2.tar.gz
tar zxf intel-gmmlib-20.3.2.tar.gz; mv gmmlib-intel-gmmlib-20.3.2 gmmlib
cd gmmlib
mkdir build
cd build
cmake ..
make -j8
sudo make install
cd ../..

wget https://github.com/intel/libva-utils/archive/2.9.1.tar.gz -O libva-utils.tar.gz
tar zxf libva-utils.tar.gz; mv libva-utils-2.9.1 libva-utils
cd libva-utils
./autogen.sh --prefix=/usr --libdir=/usr/lib/x86_64-linux-gnu; make -j4; sudo make install
cd ..

wget https://github.com/intel/media-driver/archive/intel-media-20.3.0.tar.gz
tar zxf intel-media-20.3.0.tar.gz; mv media-driver-intel-media-20.3.0 media-driver
cd media-driver
mkdir build
cd build
cmake ..
make -j8
sudo make install
cd ../..

sudo ln -sf /opt/intel/system_studio_2019/opencl-sdk /opt/intel/opencl
wget https://github.com/Intel-Media-SDK/MediaSDK/archive/intel-mediasdk-20.3.0.tar.gz
tar zxf intel-mediasdk-20.3.0.tar.gz; mv MediaSDK-intel-mediasdk-20.3.0 MediaSDK
cd MediaSDK
mkdir build
cd build
cmake ..
make -j8
sudo make install
cd ../..
cd /opt/intel/mediasdk
sudo ln -sf lib lib64
cd -

export CPLUS_INCLUDE_PATH=/opt/intel/mediasdk/include:$CPLUS_INCLUDE_PATH
sudo cp -r /opt/intel/openvino_2020.4.287/opencv /opt/intel/openvino_2020.4.287/opencv.openvino
git clone https://github.com/opencv/opencv.git
cd opencv
git checkout 62eece0d7e67df1e79d9646f0870a90e56feca10
rm -rf build
mkdir build
cd build
cmake -DCMAKE_INSTALL_PREFIX=/opt/intel/openvino_2020.4.287/opencv -DWITH_VA_INTEL=ON -DWITH_IPP=OFF -DWITH_CUDA=OFF -DOPENCV_GENERATE_PKGCONFIG=ON ..
make -j16
sudo make install
cd ../..
rm -rf opencv
sudo cp /opt/intel/openvino_2020.4.287/opencv/lib/* /opt/intel/openvino_2020.4.287/opencv.openvino/lib
sudo cp -r /opt/intel/openvino_2020.4.287/opencv/lib/pkgconfig /opt/intel/openvino_2020.4.287/opencv.openvino/lib
sudo cp -r /opt/intel/openvino_2020.4.287/opencv/include/* /opt/intel/openvino_2020.4.287/opencv.openvino/include
sudo rm -rf /opt/intel/openvino_2020.4.287/opencv
sudo cp -r /opt/intel/openvino_2020.4.287/opencv.openvino /opt/intel/openvino_2020.4.287/opencv


sudo dpkg -r intel-ocloc
sudo dpkg -r intel-opencl
sudo dpkg -r intel-igc-opencl
sudo dpkg -r intel-igc-core
sudo dpkg -r intel-gmmlib
mkdir neo
cd neo
wget https://github.com/intel/compute-runtime/releases/download/20.39.17972/intel-gmmlib_20.2.5_amd64.deb
wget https://github.com/intel/intel-graphics-compiler/releases/download/igc-1.0.5064/intel-igc-core_1.0.5064_amd64.deb
wget https://github.com/intel/intel-graphics-compiler/releases/download/igc-1.0.5064/intel-igc-opencl_1.0.5064_amd64.deb
wget https://github.com/intel/compute-runtime/releases/download/20.39.17972/intel-opencl_20.39.17972_amd64.deb
wget https://github.com/intel/compute-runtime/releases/download/20.39.17972/intel-ocloc_20.39.17972_amd64.deb
sudo dpkg -i *.deb

