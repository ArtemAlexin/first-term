 section         .text
 global          _start

NUMBER_SIZE equ 1024
LENGTH equ 128
_start:

                sub             rsp, 4 * NUMBER_SIZE
                mov             rcx, LENGTH
                lea             rdi, [rsp]
                call            read_long
                lea             rdi, [rsp + NUMBER_SIZE]
                call            read_long
                lea             rsi, [rsp]
                lea             r10, [rsp + 2 * NUMBER_SIZE]
                call            mul_long_long
               
		mov             rdi, r10
                mov             rcx, 2 * LENGTH
                call            write_long

                mov             al, 0x0a
                call            write_char
                jmp             exit

; multiplies two long number
;    rdi -- address of multiplier #1
;    rsi -- address of multiplier #2
;    rcx -- length of long numbers(qwords)
; result:
;    r10 -- address of product
mul_long_long:
		push            rcx
                push            rsi
		push            rdi	
;copy value of rcx to r11	
		mov		r11, rcx		

		add             rcx, rcx               
                mov             r9, rcx
;calculate size of two numbers
		lea		r12, [rcx * 8]
		add		r12, r12
;it is neccesary to fill current result with zeroes
                mov             rdi, r10
                call            set_zero
                pop             rdi
                
;move to the last "digit"
		lea             rdi, [rdi + 8 * r9 - 8]		
                sub             rsp, r12
                lea             r8, [rsp]
 		clc
;multiplying loop
.loop:
;we need to shift left the result one category, namely multiply it by qword size
;but as the qword size doesn't fit the register, we will multiply r10 by dword size twice 
		push            rdi               
		mov             rbx, 4294967296
                mov             rdi, r10
                call            mul_long_short
                call            mul_long_short
                pop             rdi

;rbx - current "digit", rdi - address of the rest of the number
                mov             rbx, [rdi]
                sub             rdi, 8
               
		push            rdi
		push            rsi                
;zeroize buffer
		mov             rdi, r8
                call            set_zero
               
;we need to save the number in r8
                mov             rcx, r11
                call            add_long_long
                add             rcx, rcx

                call            mul_long_short
;it is neccesary to add new number to the current result
                mov             r8, rdi
                mov             rsi, r8
                mov             rdi, r10
                call            add_long_long
;repeat and action if rcx is not zero(rest of the multiplier presents)                
		pop             rsi
                pop             rdi
                dec             r9
                jnz             .loop

;roll back to the original rsp
                add             rsp, r12
                pop             rsi
       		pop             rcx
	        ret


 ; adds two long number
;    rdi -- address of summand #1 (long number)
;    rsi -- address of summand #2 (long number)
;    rcx -- length of long numbers in qwords
; result:
;    sum is written to rdi
add_long_long:
                push            rdi
                push            rsi
                push            rcx
 
                clc
.loop:
                mov             rax, [rsi]
                lea             rsi, [rsi + 8]
                adc             [rdi], rax
                lea             rdi, [rdi + 8]
                dec             rcx
                jnz             .loop
 
                pop             rcx
                pop             rsi
                pop             rdi
                ret
 
; adds 64-bit number to long number
;    rdi -- address of summand #1 (long number)
;    rax -- summand #2 (64-bit unsigned)
;    rcx -- length of long number in qwords
; result:
;    sum is written to rdi
add_long_short:
                push            rdi
                push            rcx
                push            rdx
 
                xor             rdx,rdx
.loop:
                add             [rdi], rax
                adc             rdx, 0
                mov             rax, rdx
                xor             rdx, rdx
                add             rdi, 8
                dec             rcx
                jnz             .loop
 
                pop             rdx
                pop             rcx
                pop             rdi
                ret
 
; multiplies long number by a short
;    rdi -- address of multiplier #1 (long number)
;    rbx -- multiplier #2 (64-bit unsigned)
;    rcx -- length of long number in qwords
;    rsi -- didn't change
; result:
;    product is written to rdi
mul_long_short:
                push            rax
                push            rdi
                push            rcx
                push            rsi
 
                xor             rsi, rsi
.loop:
                mov             rax, [rdi]
                mul             rbx
                add             rax, rsi
                adc             rdx, 0
                mov             [rdi], rax
                add             rdi, 8
                mov             rsi, rdx
                dec             rcx
                jnz             .loop
 
                pop             rsi
                pop             rcx
                pop             rdi
                pop             rax
                ret
 
; divides long number by a short
;    rdi -- address of dividend (long number)
;    rbx -- divisor (64-bit unsigned)
;    rcx -- length of long number in qwords
; result:
;    quotient is written to rdi
;    rdx -- remainder
div_long_short:
                push            rdi
                push            rax
                push            rcx
 
                lea             rdi, [rdi + 8 * rcx - 8]
                xor             rdx, rdx
 
.loop:
                mov             rax, [rdi]
                div             rbx
                mov             [rdi], rax
                sub             rdi, 8
                dec             rcx
                jnz             .loop
 
                pop             rcx
                pop             rax
                pop             rdi
                ret
 
; assigns a zero to long number
;    rdi -- argument (long number)
;    rcx -- length of long number in qwords
set_zero:
                push            rax
                push            rdi
                push            rcx
 
                xor             rax, rax
                rep stosq
 
                pop             rcx
                pop             rdi
                pop             rax
                ret
 
; checks if a long number is a zero
;    rdi -- argument (long number)
;    rcx -- length of long number in qwords
; result:
;    ZF=1 if zero
is_zero:
                push            rax
                push            rdi
                push            rcx
 
                xor             rax, rax
                rep scasq
 
                pop             rcx
                pop             rdi
                pop             rax
                ret
 
; read long number from stdin
;    rdi -- location for output (long number)
;    rcx -- length of long number in qwords
read_long:
                push            rcx
                push            rdi
 
                call            set_zero
.loop:
                call            read_char
                or              rax, rax
                js              exit
                cmp             rax, 0x0a
                je              .done
                cmp             rax, '0'
                jb              .invalid_char
                cmp             rax, '9'
                ja              .invalid_char
 
                sub             rax, '0'
                mov             rbx, 10
                call            mul_long_short
                call            add_long_short
                jmp             .loop
 
.done:
                pop             rdi
                pop             rcx
                ret
 
.invalid_char:
                mov             rsi, invalid_char_msg
                mov             rdx, invalid_char_msg_size
                call            print_string
                call            write_char
                mov             al, 0x0a
                call            write_char
 
.skip_loop:
                call            read_char
                or              rax, rax
                js              exit
                cmp             rax, 0x0a
                je              exit
                jmp             .skip_loop
 
; write long number to stdout
;    rdi -- argument (long number)
;    rcx -- length of long number in qwords
write_long:
                push            rax
                push            rcx
 
                mov             rax, 20
                mul             rcx
                mov             rbp, rsp
                sub             rsp, rax
 
                mov             rsi, rbp
 
.loop:
                mov             rbx, 10
                call            div_long_short
                add             rdx, '0'
                dec             rsi
                mov             [rsi], dl
                call            is_zero
                jnz             .loop
 
                mov             rdx, rbp
                sub             rdx, rsi
                call            print_string
 
                mov             rsp, rbp
                pop             rcx
                pop             rax
                ret
 
; read one char from stdin
; result:
;    rax == -1 if error occurs
;    rax \in [0; 255] if OK
read_char:
                push            rcx
                push            rdi
 
                sub             rsp, 1
                xor             rax, rax
                xor             rdi, rdi
                mov             rsi, rsp
                mov             rdx, 1
                syscall
 
                cmp             rax, 1
                jne             .error
                xor             rax, rax
                mov             al, [rsp]
                add             rsp, 1
 
                pop             rdi
                pop             rcx
                ret
.error:
                mov             rax, -1
                add             rsp, 1
                pop             rdi
                pop             rcx
                ret
 
; write one char to stdout, errors are ignored
;    al -- char
write_char:
                sub             rsp, 1
                mov             [rsp], al
 
                mov             rax, 1
                mov             rdi, 1
                mov             rsi, rsp
                mov             rdx, 1
                syscall
                add             rsp, 1
                ret
 
exit:
                mov             rax, 60
                xor             rdi, rdi
                syscall
 
; print string to stdout
;    rsi -- string
;    rdx -- size
print_string:
                push            rax
 
                mov             rax, 1
                mov             rdi, 1
                syscall
 
                pop             rax
                ret
 
 
                section         .rodata
invalid_char_msg:
                db              "Invalid character: "
invalid_char_msg_size: equ             $ - invalid_char_msg
 
