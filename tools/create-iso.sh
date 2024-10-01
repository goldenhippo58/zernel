#!/bin/bash
mkdir -p iso/boot/grub
cp build/kernel.bin iso/boot/kernel.bin
echo 'set timeout=0' > iso/boot/grub/grub.cfg
echo 'set default=0' >> iso/boot/grub/grub.cfg
echo 'menuentry "ML Kernel" {' >> iso/boot/grub/grub.cfg
echo '    multiboot /boot/kernel.bin' >> iso/boot/grub/grub.cfg
echo '}' >> iso/boot/grub/grub.cfg

grub-mkrescue -o build/kernel.iso iso