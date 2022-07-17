# Tool Chain

## LLVM Compile Chain

See `imageRef/LLVM_Tool_Chain.png`...Typora cannot handle that image.



## LLVM IR

LLVM has only one level IR, which has structure like this:

![](https://pic4.zhimg.com/80/v2-65480aa6ea685586f12cd19068d947bf_1440w.jpg)

LLVM IR is NOT three-address-code, but has almost same format. And LLVM use `Mem2Reg` algorithm to translate LLVM IR to SSA format.

See [LLVM Manual](https://releases.llvm.org/2.0/docs/LangRef.html) and [Mem2Reg Introduction](https://llvm-clang-study-notes.readthedocs.io/en/latest/ssa/Mem2Reg.html) for tutorial.



# Web Ref

* [LLVM IR 理解 - 吴建明wujianming的文章 - 知乎](https://zhuanlan.zhihu.com/p/384115598)

