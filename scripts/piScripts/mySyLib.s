	.arch armv7-a
	.eabi_attribute 28, 1
	.eabi_attribute 20, 1
	.eabi_attribute 21, 1
	.eabi_attribute 23, 3
	.eabi_attribute 24, 1
	.eabi_attribute 25, 1
	.eabi_attribute 26, 2
	.eabi_attribute 30, 2
	.eabi_attribute 34, 1
	.eabi_attribute 18, 4
	.file	"mySyLib.c"
	.text
	.section	.rodata.str1.4,"aMS",%progbits,1
	.align	2
.LC0:
	.ascii	"%d\000"
	.text
	.align	1
	.p2align 2,,3
	.global	_Z6getintv
	.arch armv7-a
	.syntax unified
	.thumb
	.thumb_func
	.fpu vfpv3-d16
	.type	_Z6getintv, %function
_Z6getintv:
	.fnstart
.LFB30:
	@ args = 0, pretend = 0, frame = 8
	@ frame_needed = 0, uses_anonymous_args = 0
	ldr	r2, .L6
	ldr	r3, .L6+4
.LPIC1:
	add	r2, pc
	ldr	r0, .L6+8
	push	{lr}
	.save {lr}
	.pad #12
	sub	sp, sp, #12
	ldr	r3, [r2, r3]
.LPIC0:
	add	r0, pc
	mov	r1, sp
	ldr	r3, [r3]
	str	r3, [sp, #4]
	mov	r3,#0
	bl	__isoc99_scanf(PLT)
	ldr	r2, .L6+12
	ldr	r3, .L6+4
.LPIC2:
	add	r2, pc
	ldr	r0, [sp]
	ldr	r3, [r2, r3]
	ldr	r2, [r3]
	ldr	r3, [sp, #4]
	eors	r2, r3, r2
	mov	r3, #0
	bne	.L5
	add	sp, sp, #12
	@ sp needed
	ldr	pc, [sp], #4
.L5:
	bl	__stack_chk_fail(PLT)
.L7:
	.align	2
.L6:
	.word	_GLOBAL_OFFSET_TABLE_-(.LPIC1+4)
	.word	__stack_chk_guard(GOT)
	.word	.LC0-(.LPIC0+4)
	.word	_GLOBAL_OFFSET_TABLE_-(.LPIC2+4)
	.fnend
	.size	_Z6getintv, .-_Z6getintv
	.section	.rodata.str1.4
	.align	2
.LC1:
	.ascii	"%c\000"
	.text
	.align	1
	.p2align 2,,3
	.global	_Z5getchv
	.syntax unified
	.thumb
	.thumb_func
	.fpu vfpv3-d16
	.type	_Z5getchv, %function
_Z5getchv:
	.fnstart
.LFB31:
	@ args = 0, pretend = 0, frame = 8
	@ frame_needed = 0, uses_anonymous_args = 0
	ldr	r2, .L12
	ldr	r3, .L12+4
.LPIC4:
	add	r2, pc
	ldr	r0, .L12+8
	push	{lr}
	.save {lr}
	.pad #12
	sub	sp, sp, #12
	ldr	r3, [r2, r3]
	add	r1, sp, #3
.LPIC3:
	add	r0, pc
	ldr	r3, [r3]
	str	r3, [sp, #4]
	mov	r3,#0
	bl	__isoc99_scanf(PLT)
	ldr	r2, .L12+12
	ldr	r3, .L12+4
.LPIC5:
	add	r2, pc
	ldrb	r0, [sp, #3]	@ zero_extendqisi2
	ldr	r3, [r2, r3]
	ldr	r2, [r3]
	ldr	r3, [sp, #4]
	eors	r2, r3, r2
	mov	r3, #0
	bne	.L11
	add	sp, sp, #12
	@ sp needed
	ldr	pc, [sp], #4
.L11:
	bl	__stack_chk_fail(PLT)
.L13:
	.align	2
.L12:
	.word	_GLOBAL_OFFSET_TABLE_-(.LPIC4+4)
	.word	__stack_chk_guard(GOT)
	.word	.LC1-(.LPIC3+4)
	.word	_GLOBAL_OFFSET_TABLE_-(.LPIC5+4)
	.fnend
	.size	_Z5getchv, .-_Z5getchv
	.section	.rodata.str1.4
	.align	2
.LC2:
	.ascii	"%a\000"
	.text
	.align	1
	.p2align 2,,3
	.global	_Z8getfloatv
	.syntax unified
	.thumb
	.thumb_func
	.fpu vfpv3-d16
	.type	_Z8getfloatv, %function
_Z8getfloatv:
	.fnstart
.LFB32:
	@ args = 0, pretend = 0, frame = 8
	@ frame_needed = 0, uses_anonymous_args = 0
	ldr	r2, .L18
	ldr	r3, .L18+4
.LPIC7:
	add	r2, pc
	ldr	r0, .L18+8
	push	{lr}
	.save {lr}
	.pad #12
	sub	sp, sp, #12
	ldr	r3, [r2, r3]
.LPIC6:
	add	r0, pc
	mov	r1, sp
	ldr	r3, [r3]
	str	r3, [sp, #4]
	mov	r3,#0
	bl	__isoc99_scanf(PLT)
	ldr	r2, .L18+12
	ldr	r3, .L18+4
.LPIC8:
	add	r2, pc
	vldr.32	s0, [sp]
	ldr	r3, [r2, r3]
	ldr	r2, [r3]
	ldr	r3, [sp, #4]
	eors	r2, r3, r2
	mov	r3, #0
	bne	.L17
	add	sp, sp, #12
	@ sp needed
	ldr	pc, [sp], #4
.L17:
	bl	__stack_chk_fail(PLT)
.L19:
	.align	2
.L18:
	.word	_GLOBAL_OFFSET_TABLE_-(.LPIC7+4)
	.word	__stack_chk_guard(GOT)
	.word	.LC2-(.LPIC6+4)
	.word	_GLOBAL_OFFSET_TABLE_-(.LPIC8+4)
	.fnend
	.size	_Z8getfloatv, .-_Z8getfloatv
	.align	1
	.p2align 2,,3
	.global	_Z8getarrayPi
	.syntax unified
	.thumb
	.thumb_func
	.fpu vfpv3-d16
	.type	_Z8getarrayPi, %function
_Z8getarrayPi:
	.fnstart
.LFB33:
	@ args = 0, pretend = 0, frame = 8
	@ frame_needed = 0, uses_anonymous_args = 0
	ldr	r2, .L27
	push	{r4, r5, r6, lr}
	.save {r4, r5, r6, lr}
.LPIC12:
	add	r2, pc
	ldr	r6, .L27+4
	.pad #8
	sub	sp, sp, #8
	ldr	r3, .L27+8
	mov	r4, r0
.LPIC9:
	add	r6, pc
	mov	r1, sp
	ldr	r3, [r2, r3]
	mov	r0, r6
	ldr	r3, [r3]
	str	r3, [sp, #4]
	mov	r3,#0
	bl	__isoc99_scanf(PLT)
	ldr	r0, [sp]
	cmp	r0, #0
	ble	.L20
	movs	r5, #0
.L22:
	mov	r1, r4
	mov	r0, r6
	bl	__isoc99_scanf(PLT)
	ldr	r0, [sp]
	adds	r5, r5, #1
	adds	r4, r4, #4
	cmp	r0, r5
	bgt	.L22
.L20:
	ldr	r2, .L27+12
	ldr	r3, .L27+8
.LPIC11:
	add	r2, pc
	ldr	r3, [r2, r3]
	ldr	r2, [r3]
	ldr	r3, [sp, #4]
	eors	r2, r3, r2
	mov	r3, #0
	bne	.L26
	add	sp, sp, #8
	@ sp needed
	pop	{r4, r5, r6, pc}
.L26:
	bl	__stack_chk_fail(PLT)
.L28:
	.align	2
.L27:
	.word	_GLOBAL_OFFSET_TABLE_-(.LPIC12+4)
	.word	.LC0-(.LPIC9+4)
	.word	__stack_chk_guard(GOT)
	.word	_GLOBAL_OFFSET_TABLE_-(.LPIC11+4)
	.fnend
	.size	_Z8getarrayPi, .-_Z8getarrayPi
	.align	1
	.p2align 2,,3
	.global	_Z9getfarrayPf
	.syntax unified
	.thumb
	.thumb_func
	.fpu vfpv3-d16
	.type	_Z9getfarrayPf, %function
_Z9getfarrayPf:
	.fnstart
.LFB34:
	@ args = 0, pretend = 0, frame = 8
	@ frame_needed = 0, uses_anonymous_args = 0
	ldr	r2, .L36
	ldr	r3, .L36+4
.LPIC16:
	add	r2, pc
	push	{r4, r5, r6, lr}
	.save {r4, r5, r6, lr}
	mov	r4, r0
	.pad #8
	sub	sp, sp, #8
	ldr	r0, .L36+8
	ldr	r3, [r2, r3]
.LPIC13:
	add	r0, pc
	mov	r1, sp
	ldr	r3, [r3]
	str	r3, [sp, #4]
	mov	r3,#0
	bl	__isoc99_scanf(PLT)
	ldr	r0, [sp]
	cmp	r0, #0
	ble	.L29
	ldr	r6, .L36+12
	movs	r5, #0
.LPIC14:
	add	r6, pc
.L31:
	mov	r1, r4
	mov	r0, r6
	bl	__isoc99_scanf(PLT)
	ldr	r0, [sp]
	adds	r5, r5, #1
	adds	r4, r4, #4
	cmp	r0, r5
	bgt	.L31
.L29:
	ldr	r2, .L36+16
	ldr	r3, .L36+4
.LPIC15:
	add	r2, pc
	ldr	r3, [r2, r3]
	ldr	r2, [r3]
	ldr	r3, [sp, #4]
	eors	r2, r3, r2
	mov	r3, #0
	bne	.L35
	add	sp, sp, #8
	@ sp needed
	pop	{r4, r5, r6, pc}
.L35:
	bl	__stack_chk_fail(PLT)
.L37:
	.align	2
.L36:
	.word	_GLOBAL_OFFSET_TABLE_-(.LPIC16+4)
	.word	__stack_chk_guard(GOT)
	.word	.LC0-(.LPIC13+4)
	.word	.LC2-(.LPIC14+4)
	.word	_GLOBAL_OFFSET_TABLE_-(.LPIC15+4)
	.fnend
	.size	_Z9getfarrayPf, .-_Z9getfarrayPf
	.align	1
	.p2align 2,,3
	.global	_Z6putinti
	.syntax unified
	.thumb
	.thumb_func
	.fpu vfpv3-d16
	.type	_Z6putinti, %function
_Z6putinti:
	.fnstart
.LFB35:
	@ args = 0, pretend = 0, frame = 0
	@ frame_needed = 0, uses_anonymous_args = 0
	@ link register save eliminated.
	ldr	r1, .L39
	mov	r2, r0
	movs	r0, #1
.LPIC17:
	add	r1, pc
	b	__printf_chk(PLT)
.L40:
	.align	2
.L39:
	.word	.LC0-(.LPIC17+4)
	.cantunwind
	.fnend
	.size	_Z6putinti, .-_Z6putinti
	.align	1
	.p2align 2,,3
	.global	_Z5putchi
	.syntax unified
	.thumb
	.thumb_func
	.fpu vfpv3-d16
	.type	_Z5putchi, %function
_Z5putchi:
	.fnstart
.LFB36:
	@ args = 0, pretend = 0, frame = 0
	@ frame_needed = 0, uses_anonymous_args = 0
	@ link register save eliminated.
	b	putchar(PLT)
	.cantunwind
	.fnend
	.size	_Z5putchi, .-_Z5putchi
	.section	.rodata.str1.4
	.align	2
.LC3:
	.ascii	"%d:\000"
	.align	2
.LC4:
	.ascii	" %d\000"
	.text
	.align	1
	.p2align 2,,3
	.global	_Z8putarrayiPKi
	.syntax unified
	.thumb
	.thumb_func
	.fpu vfpv3-d16
	.type	_Z8putarrayiPKi, %function
_Z8putarrayiPKi:
	.fnstart
.LFB37:
	@ args = 0, pretend = 0, frame = 0
	@ frame_needed = 0, uses_anonymous_args = 0
	push	{r4, r5, r6, lr}
	.save {r4, r5, r6, lr}
	mov	r4, r1
	ldr	r1, .L47
	mov	r5, r0
	mov	r2, r0
	movs	r0, #1
.LPIC18:
	add	r1, pc
	bl	__printf_chk(PLT)
	cmp	r5, #0
	ble	.L43
	ldr	r6, .L47+4
	add	r5, r4, r5, lsl #2
.LPIC19:
	add	r6, pc
.L44:
	ldr	r2, [r4], #4
	mov	r1, r6
	movs	r0, #1
	bl	__printf_chk(PLT)
	cmp	r4, r5
	bne	.L44
.L43:
	pop	{r4, r5, r6, lr}
	movs	r0, #10
	b	putchar(PLT)
.L48:
	.align	2
.L47:
	.word	.LC3-(.LPIC18+4)
	.word	.LC4-(.LPIC19+4)
	.fnend
	.size	_Z8putarrayiPKi, .-_Z8putarrayiPKi
	.align	1
	.p2align 2,,3
	.global	_Z8putfloatf
	.syntax unified
	.thumb
	.thumb_func
	.fpu vfpv3-d16
	.type	_Z8putfloatf, %function
_Z8putfloatf:
	.fnstart
.LFB38:
	@ args = 0, pretend = 0, frame = 0
	@ frame_needed = 0, uses_anonymous_args = 0
	@ link register save eliminated.
	vcvt.f64.f32	d7, s0
	ldr	r1, .L50
	movs	r0, #1
.LPIC20:
	add	r1, pc
	vmov	r2, r3, d7
	b	__printf_chk(PLT)
.L51:
	.align	2
.L50:
	.word	.LC2-(.LPIC20+4)
	.cantunwind
	.fnend
	.size	_Z8putfloatf, .-_Z8putfloatf
	.section	.rodata.str1.4
	.align	2
.LC5:
	.ascii	" %a\000"
	.text
	.align	1
	.p2align 2,,3
	.global	_Z9putfarrayiPKf
	.syntax unified
	.thumb
	.thumb_func
	.fpu vfpv3-d16
	.type	_Z9putfarrayiPKf, %function
_Z9putfarrayiPKf:
	.fnstart
.LFB39:
	@ args = 0, pretend = 0, frame = 0
	@ frame_needed = 0, uses_anonymous_args = 0
	push	{r4, r5, r6, lr}
	.save {r4, r5, r6, lr}
	mov	r4, r1
	ldr	r1, .L57
	mov	r5, r0
	mov	r2, r0
	movs	r0, #1
.LPIC21:
	add	r1, pc
	bl	__printf_chk(PLT)
	cmp	r5, #0
	ble	.L53
	ldr	r6, .L57+4
	add	r5, r4, r5, lsl #2
.LPIC22:
	add	r6, pc
.L54:
	vldmia.32	r4!, {s15}
	mov	r1, r6
	movs	r0, #1
	vcvt.f64.f32	d7, s15
	vmov	r2, r3, d7
	bl	__printf_chk(PLT)
	cmp	r4, r5
	bne	.L54
.L53:
	pop	{r4, r5, r6, lr}
	movs	r0, #10
	b	putchar(PLT)
.L58:
	.align	2
.L57:
	.word	.LC3-(.LPIC21+4)
	.word	.LC5-(.LPIC22+4)
	.fnend
	.size	_Z9putfarrayiPKf, .-_Z9putfarrayiPKf
	.align	1
	.p2align 2,,3
	.global	_Z4putfPcz
	.syntax unified
	.thumb
	.thumb_func
	.fpu vfpv3-d16
	.type	_Z4putfPcz, %function
_Z4putfPcz:
	.fnstart
.LFB40:
	@ args = 4, pretend = 16, frame = 8
	@ frame_needed = 0, uses_anonymous_args = 1
	push	{r0, r1, r2, r3}
	.save {r0, r1, r2, r3}
	push	{r4, lr}
	.save {r4, lr}
	.pad #8
	sub	sp, sp, #8
	ldr	r4, .L63
	add	r3, sp, #16
	ldr	r1, .L63+4
.LPIC24:
	add	r4, pc
	ldr	r0, .L63+8
	ldr	r2, [r3], #4
	ldr	r1, [r4, r1]
.LPIC23:
	add	r0, pc
	ldr	r4, .L63+12
	ldr	r1, [r1]
	str	r1, [sp, #4]
	mov	r1,#0
	str	r3, [sp]
	movs	r1, #1
	ldr	r0, [r0, r4]
	ldr	r0, [r0]
	bl	__vfprintf_chk(PLT)
	ldr	r2, .L63+16
	ldr	r3, .L63+4
.LPIC25:
	add	r2, pc
	ldr	r3, [r2, r3]
	ldr	r2, [r3]
	ldr	r3, [sp, #4]
	eors	r2, r3, r2
	mov	r3, #0
	bne	.L62
	add	sp, sp, #8
	@ sp needed
	pop	{r4, lr}
	add	sp, sp, #16
	bx	lr
.L62:
	bl	__stack_chk_fail(PLT)
.L64:
	.align	2
.L63:
	.word	_GLOBAL_OFFSET_TABLE_-(.LPIC24+4)
	.word	__stack_chk_guard(GOT)
	.word	_GLOBAL_OFFSET_TABLE_-(.LPIC23+4)
	.word	stdout(GOT)
	.word	_GLOBAL_OFFSET_TABLE_-(.LPIC25+4)
	.fnend
	.size	_Z4putfPcz, .-_Z4putfPcz
	.section	.text.startup,"ax",%progbits
	.align	1
	.p2align 2,,3
	.global	_Z11before_mainv
	.syntax unified
	.thumb
	.thumb_func
	.fpu vfpv3-d16
	.type	_Z11before_mainv, %function
_Z11before_mainv:
	.fnstart
.LFB41:
	@ args = 0, pretend = 0, frame = 0
	@ frame_needed = 0, uses_anonymous_args = 0
	@ link register save eliminated.
	push	{r4, r5}
	movs	r3, #0
	ldr	r5, .L69
	ldr	r4, .L69+4
	ldr	r0, .L69+8
.LPIC26:
	add	r5, pc
	ldr	r1, .L69+12
.LPIC27:
	add	r4, pc
.LPIC28:
	add	r0, pc
	subs	r2, r5, #4
.LPIC29:
	add	r1, pc
	addw	r5, r5, #4092
	subs	r4, r4, #4
	subs	r0, r0, #4
	subs	r1, r1, #4
.L66:
	str	r3, [r2, #4]!
	cmp	r2, r5
	str	r3, [r4, #4]!
	str	r3, [r0, #4]!
	str	r3, [r1, #4]!
	bne	.L66
	ldr	r3, .L69+16
	movs	r2, #1
	pop	{r4, r5}
.LPIC31:
	add	r3, pc
	str	r2, [r3]
	bx	lr
.L70:
	.align	2
.L69:
	.word	_sysy_us-(.LPIC26+4)
	.word	_sysy_s-(.LPIC27+4)
	.word	_sysy_m-(.LPIC28+4)
	.word	_sysy_h-(.LPIC29+4)
	.word	.LANCHOR0-(.LPIC31+4)
	.cantunwind
	.fnend
	.size	_Z11before_mainv, .-_Z11before_mainv
	.section	.init_array,"aw",%init_array
	.align	2
	.word	_Z11before_mainv(target1)
	.section	.rodata.str1.4
	.align	2
.LC6:
	.ascii	"Timer@%04d-%04d: %dH-%dM-%dS-%dus\012\000"
	.align	2
.LC7:
	.ascii	"TOTAL: %dH-%dM-%dS-%dus\012\000"
	.section	.text.exit,"ax",%progbits
	.align	1
	.p2align 2,,3
	.global	_Z10after_mainv
	.syntax unified
	.thumb
	.thumb_func
	.fpu vfpv3-d16
	.type	_Z10after_mainv, %function
_Z10after_mainv:
	.fnstart
.LFB42:
	@ args = 0, pretend = 0, frame = 32
	@ frame_needed = 0, uses_anonymous_args = 0
	ldr	r3, .L76
	push	{r4, r5, r6, r7, r8, r9, r10, fp, lr}
	.save {r4, r5, r6, r7, r8, r9, r10, fp, lr}
.LPIC32:
	add	r3, pc
	vpush.64	{d8}
	.vsave {d8}
	ldr	r3, [r3]
	ldr	r2, .L76+4
	cmp	r3, #1
.LPIC40:
	add	r2, pc
	.pad #60
	sub	sp, sp, #60
	ble	.L72
	movw	r1, #56963
	ldr	r3, .L76+8
	mov	r0, r1
	ldr	r1, .L76+12
	movt	r0, 17179
	str	r0, [sp, #52]
	ldr	r0, .L76+16
.LPIC33:
	add	r1, pc
	adds	r1, r1, #4
	str	r1, [sp, #36]
.LPIC34:
	add	r0, pc
	ldr	r6, .L76+20
	adds	r0, r0, #4
	str	r0, [sp, #40]
	ldr	r3, [r2, r3]
.LPIC35:
	add	r6, pc
	ldr	r1, .L76+24
	adds	r6, r6, #4
	ldr	r8, .L76+28
	movs	r7, #1
	str	r3, [sp, #48]
.LPIC36:
	add	r1, pc
	ldr	r3, .L76+32
.LPIC37:
	add	r8, pc
	ldr	r5, .L76+36
	add	r10, r1, #4
.LPIC39:
	add	r3, pc
	add	fp, r8, #4
.LPIC38:
	add	r5, pc
	str	r10, [sp, #44]
	vmov	s16, r3	@ int
	adds	r5, r5, #4
	strd	fp, r5, [sp, #28]
.L74:
	ldr	r2, [sp, #36]
	adds	r7, r7, #1
	ldr	r0, [sp, #28]
	ldr	r5, [sp, #44]
	ldr	r3, [r2], #4
	ldr	lr, [r0]
	ldr	r0, [sp, #32]
	str	r2, [sp, #36]
	ldr	r2, [sp, #40]
	ldr	ip, [r5]
	ldr	r4, [r6]
	ldr	r1, [r2], #4
	ldr	r9, [r0]
	ldr	r0, [sp, #48]
	str	r2, [sp, #40]
	vmov	r2, s16	@ int
	ldr	r0, [r0]
	strd	lr, r9, [sp, #12]
	strd	r4, ip, [sp, #4]
	str	r1, [sp]
	movs	r1, #1
	bl	__fprintf_chk(PLT)
	ldr	r2, [sp, #32]
	ldr	r0, [r5], #4
	movw	r3, #34953
	ldr	lr, .L76+40
	movt	r3, 34952
	ldr	r1, [r2], #4
.LPIC42:
	add	lr, pc
	ldr	r9, .L76+44
	str	r5, [sp, #44]
	ldr	r5, [sp, #28]
.LPIC44:
	add	r9, pc
	str	r2, [sp, #32]
	ldr	r2, [lr]
	ldr	r4, [r9]
	add	ip, r2, r1
	ldr	r1, [r5], #4
	ldr	r2, [r8]
	add	r0, r0, r4
	add	r2, r2, r1
	str	r5, [sp, #28]
	smull	r1, r10, r3, r0
	asr	fp, r2, #31
	smull	r1, r3, r3, r2
	ldr	r1, [sp, #52]
	add	r10, r10, r0
	asrs	r4, r0, #31
	add	r3, r3, r2
	smull	r5, r1, r1, ip
	rsb	r3, fp, r3, asr #5
	rsb	r4, r4, r10, asr #5
	asr	fp, ip, #31
	mov	r10, #60
	mls	r3, r10, r3, r2
	movw	r2, #16960
	rsb	r1, fp, r1, asr #18
	movt	r2, 15
	mls	r4, r10, r4, r0
	ldr	r0, .L76+48
	mls	r1, r2, r1, ip
	ldr	ip, .L76+52
.LPIC46:
	add	r0, pc
	ldr	r2, [r6], #4
.LPIC49:
	add	ip, pc
	str	r3, [r8]
	ldr	r5, [r0]
	ldr	ip, [ip]
	add	r2, r2, r5
	str	r4, [r9]
	cmp	ip, r7
	str	r1, [lr]
	str	r2, [r0]
	bgt	.L74
	mov	ip, r3
	mov	r3, r2
.L73:
	ldr	r2, [sp, #48]
	ldr	r0, [r2]
	ldr	r2, .L76+56
	str	r1, [sp, #8]
	movs	r1, #1
.LPIC54:
	add	r2, pc
	strd	r4, ip, [sp]
	bl	__fprintf_chk(PLT)
	add	sp, sp, #60
	@ sp needed
	vldm	sp!, {d8}
	pop	{r4, r5, r6, r7, r8, r9, r10, fp, pc}
.L72:
	ldr	r0, .L76+8
	ldr	r3, .L76+60
	ldr	r4, .L76+64
	ldr	r5, .L76+68
.LPIC50:
	add	r3, pc
	ldr	r1, .L76+72
.LPIC51:
	add	r4, pc
	ldr	r2, [r2, r0]
.LPIC52:
	add	r5, pc
.LPIC53:
	add	r1, pc
	ldr	r3, [r3]
	ldr	r4, [r4]
	ldr	ip, [r5]
	ldr	r1, [r1]
	str	r2, [sp, #48]
	b	.L73
.L77:
	.align	2
.L76:
	.word	.LANCHOR0-(.LPIC32+4)
	.word	_GLOBAL_OFFSET_TABLE_-(.LPIC40+4)
	.word	stderr(GOT)
	.word	_sysy_l1-(.LPIC33+4)
	.word	_sysy_l2-(.LPIC34+4)
	.word	_sysy_h-(.LPIC35+4)
	.word	_sysy_m-(.LPIC36+4)
	.word	_sysy_s-(.LPIC37+4)
	.word	.LC6-(.LPIC39+4)
	.word	_sysy_us-(.LPIC38+4)
	.word	_sysy_us-(.LPIC42+4)
	.word	_sysy_m-(.LPIC44+4)
	.word	_sysy_h-(.LPIC46+4)
	.word	.LANCHOR0-(.LPIC49+4)
	.word	.LC7-(.LPIC54+4)
	.word	_sysy_h-(.LPIC50+4)
	.word	_sysy_m-(.LPIC51+4)
	.word	_sysy_s-(.LPIC52+4)
	.word	_sysy_us-(.LPIC53+4)
	.fnend
	.size	_Z10after_mainv, .-_Z10after_mainv
	.section	.fini_array,"aw",%fini_array
	.align	2
	.word	_Z10after_mainv(target1)
	.text
	.align	1
	.p2align 2,,3
	.global	_Z15_sysy_starttimei
	.syntax unified
	.thumb
	.thumb_func
	.fpu vfpv3-d16
	.type	_Z15_sysy_starttimei, %function
_Z15_sysy_starttimei:
	.fnstart
.LFB43:
	@ args = 0, pretend = 0, frame = 0
	@ frame_needed = 0, uses_anonymous_args = 0
	@ link register save eliminated.
	ldr	r2, .L80
	movs	r1, #0
	ldr	r3, .L80+4
.LPIC56:
	add	r2, pc
	push	{r4, r5}
.LPIC55:
	add	r3, pc
	ldr	r4, [r2], #4
	mov	r5, r0
	mov	r0, r2
	str	r5, [r3, r4, lsl #2]
	pop	{r4, r5}
	b	gettimeofday(PLT)
.L81:
	.align	2
.L80:
	.word	.LANCHOR0-(.LPIC56+4)
	.word	_sysy_l1-(.LPIC55+4)
	.cantunwind
	.fnend
	.size	_Z15_sysy_starttimei, .-_Z15_sysy_starttimei
	.align	1
	.p2align 2,,3
	.global	_Z14_sysy_stoptimei
	.syntax unified
	.thumb
	.thumb_func
	.fpu vfpv3-d16
	.type	_Z14_sysy_stoptimei, %function
_Z14_sysy_stoptimei:
	.fnstart
.LFB44:
	@ args = 0, pretend = 0, frame = 0
	@ frame_needed = 0, uses_anonymous_args = 0
	push	{r4, r5, r6, r7, r8, lr}
	movs	r1, #0
	ldr	r4, .L84
	mov	r6, r0
.LPIC58:
	add	r4, pc
	add	r0, r4, #12
	bl	gettimeofday(PLT)
	ldr	r2, [r4, #12]
	ldr	r3, [r4, #4]
	movw	lr, #16960
	ldr	r0, [r4, #16]
	movt	lr, 15
	subs	r3, r2, r3
	ldr	ip, .L84+4
	ldr	r1, [r4]
	mla	r3, lr, r3, r0
.LPIC65:
	add	ip, pc
	ldr	r2, [r4, #8]
	ldr	r5, [ip, r1, lsl #2]
	subs	r3, r3, r2
	ldr	r0, .L84+8
	add	r3, r3, r5
	movw	r2, #56963
	movt	r2, 17179
.LPIC60:
	add	r0, pc
	asrs	r5, r3, #31
	smull	r2, r7, r2, r3
	str	r6, [r0, r1, lsl #2]
	ldr	r6, .L84+12
	movw	r2, #34953
	movt	r2, 34952
.LPIC66:
	add	r6, pc
	ldr	r0, [r6, r1, lsl #2]
	rsb	r5, r5, r7, asr #18
	ldr	r7, .L84+16
	add	r8, r5, r0
	mls	r3, lr, r5, r3
.LPIC68:
	add	r7, pc
	smull	lr, r0, r2, r8
	asr	r5, r8, #31
	add	r0, r0, r8
	str	r3, [ip, r1, lsl #2]
	ldr	r3, [r7, r1, lsl #2]
	rsb	r0, r5, r0, asr #5
	movs	r5, #60
	add	ip, r0, r3
	mls	r0, r5, r0, r8
	smull	r2, r3, r2, ip
	asr	r2, ip, #31
	add	r3, r3, ip
	str	r0, [r6, r1, lsl #2]
	ldr	r0, .L84+20
	rsb	r2, r2, r3, asr #5
	adds	r3, r1, #1
.LPIC70:
	add	r0, pc
	str	r3, [r4]
	mls	r5, r5, r2, ip
	ldr	r3, [r0, r1, lsl #2]
	add	r3, r3, r2
	str	r3, [r0, r1, lsl #2]
	str	r5, [r7, r1, lsl #2]
	pop	{r4, r5, r6, r7, r8, pc}
.L85:
	.align	2
.L84:
	.word	.LANCHOR0-(.LPIC58+4)
	.word	_sysy_us-(.LPIC65+4)
	.word	_sysy_l2-(.LPIC60+4)
	.word	_sysy_s-(.LPIC66+4)
	.word	_sysy_m-(.LPIC68+4)
	.word	_sysy_h-(.LPIC70+4)
	.cantunwind
	.fnend
	.size	_Z14_sysy_stoptimei, .-_Z14_sysy_stoptimei
	.global	_sysy_idx
	.global	_sysy_us
	.global	_sysy_s
	.global	_sysy_m
	.global	_sysy_h
	.global	_sysy_l2
	.global	_sysy_l1
	.global	_sysy_end
	.global	_sysy_start
	.bss
	.align	2
	.set	.LANCHOR0,. + 0
	.type	_sysy_idx, %object
	.size	_sysy_idx, 4
_sysy_idx:
	.space	4
	.type	_sysy_start, %object
	.size	_sysy_start, 8
_sysy_start:
	.space	8
	.type	_sysy_end, %object
	.size	_sysy_end, 8
_sysy_end:
	.space	8
	.type	_sysy_us, %object
	.size	_sysy_us, 4096
_sysy_us:
	.space	4096
	.type	_sysy_s, %object
	.size	_sysy_s, 4096
_sysy_s:
	.space	4096
	.type	_sysy_m, %object
	.size	_sysy_m, 4096
_sysy_m:
	.space	4096
	.type	_sysy_h, %object
	.size	_sysy_h, 4096
_sysy_h:
	.space	4096
	.type	_sysy_l2, %object
	.size	_sysy_l2, 4096
_sysy_l2:
	.space	4096
	.type	_sysy_l1, %object
	.size	_sysy_l1, 4096
_sysy_l1:
	.space	4096
	.ident	"GCC: (Ubuntu 9.4.0-1ubuntu1~20.04.1) 9.4.0"
	.section	.note.GNU-stack,"",%progbits
