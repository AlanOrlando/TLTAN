cd ..
cd ..
cp -r Kernel/ /home/utnso/
cd configuraciones/
cd prueba_base/
cd kernel-config/
cp kernel.config /home/utnso/Kernel/
cd ..
cd ..
cd ..
cd ..
mv Kernel kernel-base
cd kernel-base/
cd Default/
make clean
make all
