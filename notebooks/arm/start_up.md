# Some Basic Info

* How to Load Immediate Int:
	
	* `movw` + `movt`
	  ```arm
	  movw r0, #0x0000
	  movt r0, #0x0001
	  ```
	  This move imm `0x00000001` to `r0`.
	  
	* `movw`
	
	  For integer value less than 65536.
	  
	* `mov`
	
	  For Integer that can form `<imm8m>`.
	  
	* `mvn`
	
	  For Integer that can form `<imm8m> ^ 0xFFFFFFFF`.
	  
	* For Some Instructions, not Need to Load imm on Reg:
	
	  Like `sub a0, a0, 1`.
	
* How to Load Immediate Float:
	
	* Between Memory and ARM-Reg:
	
	  Just move bit pattern like loading immediate integer.
	  
	* Between VFP and ARM:
		
		* `vmov`
		  `vmov{C}.<P> Fd, #<fpconst>`
		
		  `#<fpconst>` : $\pm m * 2^{-n}$, where $16\le m\le 31, 0\le n\le 7$.
		  
		* Move to ARM-Reg and move to VFP-Reg then.
	
* How to Load Immediate Int on ARM-Reg from Memory:

    * `movw`+`movt`+`ldr`

        ```assembly
        movw	r1, :lower16:ar
        str	r0, [sp]	@ index saved on [sp]
        movt	r1, :upper16:ar
        ldr	r1, [r1, r0, lsl #2]
        ```

        `ar` is global address point to array.

* How to Load Immediate Float on VFP-Reg from Memory:

    * `movw`+`movt`+`add`+`vldr`:

        ```assembly
        movw	r1, :lower16:ar
        str	r0, [sp]
        movt	r1, :upper16:ar
        add	r1, r1, r0, lsl #2
        vldr	s0, [r1]
        ```

* How to Decalre Global Address to Data:

    No Difference on Int/Float, or on shape of array.

    ```assembly
    ar:
    	.long	0x3f800000                      @ float 1
    	.long	0x40000000                      @ float 2
    	.long	0x40400000                      @ float 3
    	.size	ar, 12
    ar:	
    	.long	1                               @ 0x1
    	.long	2                               @ 0x2
    	.long	3                               @ 0x3
    	.long	4                               @ 0x4
    	.long	5                               @ 0x5
    	.long	6                               @ 0x6
    	.long	7                               @ 0x7
    	.long	8                               @ 0x8
    	.long	9                               @ 0x9
    	.size	ar, 36
    ```

* 

