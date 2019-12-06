# build-and-upload-to-badge.sh
set -ex

source /home/j0sh/Documents/projects/supercon-badge/ecp5-toolchain-linux_x86_64-v1.6.2/env.sh

cd '/home/j0sh/Documents/projects/supercon-badge/hadbadge2019_workshops/basic/app-basic-workshop' 
make 
cp -t /media/j0sh/4EED-0000 *.elf 
sync
