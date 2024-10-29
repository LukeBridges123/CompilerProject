(module
  (func $IsPrime (param $var0 i32) (result i32)
    ;; Variables
    (local $var1 i32)  ;; Variable: is_prime
    (local $var2 i32)  ;; Variable: test_cap
    (local $var3 i32)  ;; Variable: test_val
    
    (block $fun_exit1 (result i32)
      ;; ASSIGNMENT
        (i32.const 1)    ;; Put a 1 on the stack
        (local.set $var1)    ;; Set var 'is_prime' from stack
        (local.get $var1)    ;; Place var 'is_prime' onto stack
      ;; END ASSIGNMENT (with result on stack)
      (drop)  ;; remove unused value on stack.
      ;; ASSIGNMENT
        (local.get $var0)    ;; Place var 'value' onto stack
        (f64.convert_i32_s) ;; Convert to double.
        (f64.sqrt)     ;; Square Root
        (i32.trunc_f64_s) ;; Convert to int.
        (local.set $var2)    ;; Set var 'test_cap' from stack
        (local.get $var2)    ;; Place var 'test_cap' onto stack
      ;; END ASSIGNMENT (with result on stack)
      (drop)  ;; remove unused value on stack.
      ;; ASSIGNMENT
        (i32.const 2)    ;; Put a 2 on the stack
        (local.set $var3)    ;; Set var 'test_val' from stack
        (local.get $var3)    ;; Place var 'test_val' onto stack
      ;; END ASSIGNMENT (with result on stack)
      (drop)  ;; remove unused value on stack.
      (block $exit1   ;; Outer block for breaking while loop.
        (loop $loop1  ;; Inner loop for continuing while.
          ;; WHILE Test condition...
          ;; Setup the && operation
          (local.get $var1)    ;; Place var 'is_prime' onto stack
          (if (result i32) ;; Setup for && operator
            (then
              (local.get $var3)    ;; Place var 'test_val' onto stack
              (local.get $var2)    ;; Place var 'test_cap' onto stack
              (i32.le_s)  ;; Stack2 <= Stack1
              (i32.const 0) ;; Put a zero on the stack for comparison)
              (i32.ne)      ;; Set any non-zero value to one.)
            )
            (else 
              (i32.const 0) ;; First clause of && was false.
            )
          )
          ;; End of && operation
          (i32.eqz)  ;; Invert the result of the test condition.
          (br_if $exit1)  ;; If condition is false (0), exit the loop
          ;; WHILE Loop body...
          ;; Test condition for if.
          (local.get $var0)    ;; Place var 'value' onto stack
          (local.get $var3)    ;; Place var 'test_val' onto stack
          (i32.rem_s) ;; Stack2 % Stack1
          (i32.const 0)    ;; Put a 0 on the stack
          (i32.eq)    ;; Stack2 == Stack1
          (if  ;; Execute code based on result of condition.
            (then ;; 'then' block
              ;; ASSIGNMENT
                (i32.const 0)    ;; Put a 0 on the stack
                (local.set $var1)    ;; Set var 'is_prime' from stack
                (local.get $var1)    ;; Place var 'is_prime' onto stack
              ;; END ASSIGNMENT (with result on stack)
              (drop)  ;; remove unused value on stack.
            ) ;; End 'then'
          )  ;; End 'if'
          ;; ASSIGNMENT
            (local.get $var3)    ;; Place var 'test_val' onto stack
            (i32.const 1)    ;; Put a 1 on the stack
            (i32.add)   ;; Stack2 + Stack1
            (local.set $var3)    ;; Set var 'test_val' from stack
            (local.get $var3)    ;; Place var 'test_val' onto stack
          ;; END ASSIGNMENT (with result on stack)
          (drop)  ;; remove unused value on stack.
          ;; WHILE start next loop.
          (br $loop1)   ;; Jump back to the start of the loop
        ) ;; End loop
      )   ;; End block
      (local.get $var1)    ;; Place var 'is_prime' onto stack
    ) ;; end of function block.
  ) ;; END IsPrime function definition.
  
  (export "IsPrime" (func $IsPrime))
  
) ;; END program module
