cd ..
cd ..
cp -r LFS/ /home/utnso/
cd configuraciones/
cd prueba_lfs/
cd lfs-config/
cp fileSystem.config /home/utnso/LFS/
cd ..
cd metadata-config/
cp Metadata.bin /home/utnso/LFS/Metadata/
cd ..
cd ..
cd ..
cd ..
mv LFS lfs-prueba-memoria
cd lfs-prueba-memoria/
cd Default/
make clean
make all

