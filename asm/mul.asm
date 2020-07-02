section         .text
 
                global          _start
_start:
;free space for 2 long numbers
                sub             rsp, 2 * 128 * 8
;adress of the second number in rdi
                lea             rdi, [rsp + 128 * 8]
;length of long number
                mov             rcx, 128
;new long number is in rdi
                call            read_long
;read another long to the rsp adress
                mov             rdi, rsp
                call            read_long
;rsi - begining of the second number, rdi - beggining of the first number
                lea             rsi, [rsp + 128 * 8]
;rdi - adress of two long numbers product
                call            mul_long_long
;write product to stdout
                call            write_long
;write /n
                mov             al, 0x0a
                call            write_char
;exit the programm
                jmp             exit
 
; multiplies two long numbers
;    rdi -- address of first long number
;    rsi -- address of second long number
;    rcx -- length of long numbers in qwords
; result:
;    product is written to rdi
mul_long_long:
                push            rcx
                push            rsi
;cleats carry flag
                clc
;free space for result of product(register r8)
                lea             r8, [buffer + 4000]
;copy length to r9(rcx will be changed during other operations)
                mov             r9, rcx
;multiplies new qword of the second number by first number and adds to the result(with neccesary shifts), like usual long multiplication
.sum_loop:
;rbx - first qword of long number
                mov             rbx, [rsi]
;rsi - adress of next qword
                add             rsi, 8
;next qword = 0 -> it is not neccesary to multiply anything -> just shift the number(this situation is standed out, because further there will be devision by rbx)
                or              rbx, rbx
                jz              .shift_qword_left

;rdi - adress long number and read qword product
                call            mul_long_short
				push            rdi                
				push            rsi
;map rdi -> r8(result), rsi -> rdi(new summand)
                mov             rsi, rdi
                mov             rdi, r8
;number at rdi(r8)+=number(rsi), adds new summand to the current result
                call            add_long_long
                pop             rsi
                pop             rdi
;number at the rdi adress may be changed, we want to write there initial value(it is done to minimize value of spent memory)
                call            div_long_short
;we need to shift number 1 qword left <=> 2 dwords left
;shifts number at rdi one qword left
.shift_qword_left:
;rbx stores size of dword, because qword does not fit into the 64-bit register
                mov             rbx, 4294967296
;shifts number one dword left
                call            mul_long_short
;shifst number one dword left
                call			mul_long_short
                dec             r9
;r9 == 0 -> all qword have been added to the result
                jnz             .sum_loop
;all qwords are passed, rdi -> product
                mov             rdi, r8
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
 
                section         .bss
; declaring buffer as automatically zero-filled variable at runtime
buffer:
                resb 128
; declaring space for storing data in bytes
