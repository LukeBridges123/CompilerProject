(module
  (func $FindPrime (param $var0 i32) (result i32)
    ;; Variables
    (local $var1 i32)  ;; Variable: is_prime
    (local $var2 i32)  ;; Variable: test_factor
    (local $var3 i32)  ;; Variable: test_cap
    
    (block $fun_exit1 (result i32)
      (block $exit1   ;; Outer block for breaking while loop.
        (loop $loop1  ;; Inner loop for continuing while.
          ;; WHILE Test condition...
          (i32.const 1)    ;; Put a 1 on the stack
          (i32.eqz)  ;; Invert the result of the test condition.
          (br_if $exit1)  ;; If condition is false (0), exit the loop
          ;; WHILE Loop body...
          ;; ASSIGNMENT
            (i32.const 1)    ;; Put a 1 on the stack
            (local.set $var1)    ;; Set var 'is_prime' from stack
            (local.get $var1)    ;; Place var 'is_prime' onto stack
          ;; END ASSIGNMENT (with result on stack)
          (drop)  ;; remove unused value on stack.
          ;; ASSIGNMENT
            (i32.const 2)    ;; Put a 2 on the stack
            (local.set $var2)    ;; Set var 'test_factor' from stack
            (local.get $var2)    ;; Place var 'test_factor' onto stack
          ;; END ASSIGNMENT (with result on stack)
          (drop)  ;; remove unused value on stack.
          ;; ASSIGNMENT
            (local.get $var0)    ;; Place var 'value' onto stack
            (f64.convert_i32_s) ;; Convert to double.
            (f64.sqrt)     ;; Square Root
            (i32.trunc_f64_s) ;; Convert to int.
            (local.set $var3)    ;; Set var 'test_cap' from stack
            (local.get $var3)    ;; Place var 'test_cap' onto stack
          ;; END ASSIGNMENT (with result on stack)
          (drop)  ;; remove unused value on stack.
          (block $exit2   ;; Outer block for breaking while loop.
            (loop $loop2  ;; Inner loop for continuing while.
              ;; WHILE Test condition...
              (local.get $var2)    ;; Place var 'test_factor' onto stack
              (local.get $var3)    ;; Place var 'test_cap' onto stack
              (i32.le_s)  ;; Stack2 <= Stack1
              (i32.eqz)  ;; Invert the result of the test condition.
              (br_if $exit2)  ;; If condition is false (0), exit the loop
              ;; WHILE Loop body...
              ;; Test condition for if.
              (local.get $var0)    ;; Place var 'value' onto stack
              (local.get $var2)    ;; Place var 'test_factor' onto stack
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
                  (br $exit2) ;; 'break' command.
                ) ;; End 'then'
              )  ;; End 'if'
              ;; ASSIGNMENT
                (local.get $var2)    ;; Place var 'test_factor' onto stack
                (i32.const 1)    ;; Put a 1 on the stack
                (i32.add)   ;; Stack2 + Stack1
                (local.set $var2)    ;; Set var 'test_factor' from stack
                (local.get $var2)    ;; Place var 'test_factor' onto stack
              ;; END ASSIGNMENT (with result on stack)
              (drop)  ;; remove unused value on stack.
              ;; WHILE start next loop.
              (br $loop2)   ;; Jump back to the start of the loop
            ) ;; End loop
          )   ;; End block
          ;; Test condition for if.
          (local.get $var1)    ;; Place var 'is_prime' onto stack
          (if  ;; Execute code based on result of condition.
            (then ;; 'then' block
              (br $exit1) ;; 'break' command.
            ) ;; End 'then'
          )  ;; End 'if'
          ;; ASSIGNMENT
            (local.get $var0)    ;; Place var 'value' onto stack
            (i32.const 1)    ;; Put a 1 on the stack
            (i32.add)   ;; Stack2 + Stack1
            (local.set $var0)    ;; Set var 'value' from stack
            (local.get $var0)    ;; Place var 'value' onto stack
          ;; END ASSIGNMENT (with result on stack)
          (drop)  ;; remove unused value on stack.
          ;; WHILE start next loop.
          (br $loop1)   ;; Jump back to the start of the loop
        ) ;; End loop
      )   ;; End block
      (local.get $var0)    ;; Place var 'value' onto stack
    ) ;; end of function block.
  ) ;; END FindPrime function definition.
  
  (export "FindPrime" (func $FindPrime))
  
) ;; END program module
