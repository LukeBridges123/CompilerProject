(module
  (func $Collatz (param $var0 i32) (result i32)
    ;; Variables
    (local $var1 i32)  ;; Variable: count
    
    (block $fun_exit1 (result i32)
      ;; ASSIGNMENT
        (i32.const 0)    ;; Put a 0 on the stack
        (local.set $var1)    ;; Set var 'count' from stack
        (local.get $var1)    ;; Place var 'count' onto stack
      ;; END ASSIGNMENT (with result on stack)
      (drop)  ;; remove unused value on stack.
      (block $exit1   ;; Outer block for breaking while loop.
        (loop $loop1  ;; Inner loop for continuing while.
          ;; WHILE Test condition...
          (local.get $var0)    ;; Place var 'n' onto stack
          (i32.const 1)    ;; Put a 1 on the stack
          (i32.ne)    ;; Stack2 != Stack1
          (i32.eqz)  ;; Invert the result of the test condition.
          (br_if $exit1)  ;; If condition is false (0), exit the loop
          ;; WHILE Loop body...
          ;; Test condition for if.
          (local.get $var0)    ;; Place var 'n' onto stack
          (i32.const 2)    ;; Put a 2 on the stack
          (i32.rem_s) ;; Stack2 % Stack1
          (i32.const 0)    ;; Put a 0 on the stack
          (i32.eq)    ;; Stack2 == Stack1
          (if  ;; Execute code based on result of condition.
            (then ;; 'then' block
              ;; ASSIGNMENT
                (local.get $var0)    ;; Place var 'n' onto stack
                (i32.const 2)    ;; Put a 2 on the stack
                (i32.div_s) ;; Stack2 / Stack1
                (local.set $var0)    ;; Set var 'n' from stack
                (local.get $var0)    ;; Place var 'n' onto stack
              ;; END ASSIGNMENT (with result on stack)
              (drop)  ;; remove unused value on stack.
            ) ;; End 'then'
            (else ;; 'else' block
              ;; ASSIGNMENT
                (i32.const 3)    ;; Put a 3 on the stack
                (local.get $var0)    ;; Place var 'n' onto stack
                (i32.mul)   ;; Stack2 * Stack1
                (i32.const 1)    ;; Put a 1 on the stack
                (i32.add)   ;; Stack2 + Stack1
                (local.set $var0)    ;; Set var 'n' from stack
                (local.get $var0)    ;; Place var 'n' onto stack
              ;; END ASSIGNMENT (with result on stack)
              (drop)  ;; remove unused value on stack.
            ) ;; End 'else'
          )  ;; End 'if'
          ;; ASSIGNMENT
            (local.get $var1)    ;; Place var 'count' onto stack
            (i32.const 1)    ;; Put a 1 on the stack
            (i32.add)   ;; Stack2 + Stack1
            (local.set $var1)    ;; Set var 'count' from stack
            (local.get $var1)    ;; Place var 'count' onto stack
          ;; END ASSIGNMENT (with result on stack)
          (drop)  ;; remove unused value on stack.
          ;; WHILE start next loop.
          (br $loop1)   ;; Jump back to the start of the loop
        ) ;; End loop
      )   ;; End block
      (local.get $var1)    ;; Place var 'count' onto stack
    ) ;; end of function block.
  ) ;; END Collatz function definition.
  
  (export "Collatz" (func $Collatz))
  
) ;; END program module
