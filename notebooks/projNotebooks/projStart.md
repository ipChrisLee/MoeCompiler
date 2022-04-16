This is about what to do before reading source code and coding.

# Cross Compiler

To compile and run arm32 architecture code in our X86-64 computer, install some applications by command line codes below: 

```sh
sudo apt update -y && sudo apt upgrade -y
sudo apt install qemu-user qemu-user-static gcc-aarch64-linux-gnu binutils-aarch64-linux-gnu binutils-aarch64-linux-gnu-dbg build-essential
sudo apt install gcc-arm-linux-gnueabihf binutils-arm-linux-gnueabihf binutils-arm-linux-gnueabihf-dbg
```



Read [ref](https://azeria-labs.com/arm-on-x86-qemu-user/) to know how to compile, assemble link C files targeting at arm32 architecture in X84 computer.



# About static lib and dynamic lib

Read [this](https://stackoverflow.com/questions/140061/when-to-use-dynamic-vs-static-libraries) article to understand difference about static lib and dynamic lib.

We should treat `sylib` as static lib according to CSC2022 document. We will talk about how to realize this later.

