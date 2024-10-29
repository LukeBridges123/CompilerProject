(module
  (func $Fibonacci (param $var0 i32) (result i32)
    ;; Variables
    (local $var1 i32)  ;; Variable: val1
    (local $var2 i32)  ;; Variable: val2
    (local $var3 i32)  ;; Variable: step
    (local $var4 i32)  ;; Variable: sum
    
    (block $fun_exit1 (result i32)
      ;; Test condition for if.
      (local.get $var0)    ;; Place var 'n' onto stack
      (i32.const 2)    ;; Put a 2 on the stack
      (i32.lt_s)  ;; Stack2 < Stack1
      (if  ;; Execute code based on result of condition.
        (then ;; 'then' block
          (local.get $var0)    ;; Place var 'n' onto stack
          (br $fun_exit1) ;; return value.
        ) ;; End 'then'
      )  ;; End 'if'
      ;; ASSIGNMENT
        (i32.const 0)    ;; Put a 0 on the stack
        (local.set $var1)    ;; Set var 'val1' from stack
        (local.get $var1)    ;; Place var 'val1' onto stack
      ;; END ASSIGNMENT (with result on stack)
      (drop)  ;; remove unused value on stack.
      ;; ASSIGNMENT
        (i32.const 1)    ;; Put a 1 on the stack
        (local.set $var2)    ;; Set var 'val2' from stack
        (local.get $var2)    ;; Place var 'val2' onto stack
      ;; END ASSIGNMENT (with result on stack)
      (drop)  ;; remove unused value on stack.
      ;; ASSIGNMENT
        (i32.const 2)    ;; Put a 2 on the stack
        (local.set $var3)    ;; Set var 'step' from stack
        (local.get $var3)    ;; Place var 'step' onto stack
      ;; END ASSIGNMENT (with result on stack)
      (drop)  ;; remove unused value on stack.
      (block $exit1   ;; Outer block for breaking while loop.
        (loop $loop1  ;; Inner loop for continuing while.
          ;; WHILE Test condition...
          (local.get $var3)    ;; Place var 'step' onto stack
          (local.get $var0)    ;; Place var 'n' onto stack
          (i32.le_s)  ;; Stack2 <= Stack1
          (i32.eqz)  ;; Invert the result of the test condition.
          (br_if $exit1)  ;; If condition is false (0), exit the loop
          ;; WHILE Loop body...
          ;; ASSIGNMENT
            (local.get $var1)    ;; Place var 'val1' onto stack
            (local.get $var2)    ;; Place var 'val2' onto stack
            (i32.add)   ;; Stack2 + Stack1
            (local.set $var4)    ;; Set var 'sum' from stack
            (local.get $var4)    ;; Place var 'sum' onto stack
          ;; END ASSIGNMENT (with result on stack)
          (drop)  ;; remove unused value on stack.
          ;; ASSIGNMENT
            (local.get $var2)    ;; Place var 'val2' onto stack
            (local.set $var1)    ;; Set var 'val1' from stack
            (local.get $var1)    ;; Place var 'val1' onto stack
          ;; END ASSIGNMENT (with result on stack)
          (drop)  ;; remove unused value on stack.
          ;; ASSIGNMENT
            (local.get $var4)    ;; Place var 'sum' onto stack
            (local.set $var2)    ;; Set var 'val2' from stack
            (local.get $var2)    ;; Place var 'val2' onto stack
          ;; END ASSIGNMENT (with result on stack)
          (drop)  ;; remove unused value on stack.
          ;; ASSIGNMENT
            (local.get $var3)    ;; Place var 'step' onto stack
            (i32.const 1)    ;; Put a 1 on the stack
            (i32.add)   ;; Stack2 + Stack1
            (local.set $var3)    ;; Set var 'step' from stack
            (local.get $var3)    ;; Place var 'step' onto stack
          ;; END ASSIGNMENT (with result on stack)
          (drop)  ;; remove unused value on stack.
          ;; WHILE start next loop.
          (br $loop1)   ;; Jump back to the start of the loop
        ) ;; End loop
      )   ;; End block
      (local.get $var2)    ;; Place var 'val2' onto stack
    ) ;; end of function block.
  ) ;; END Fibonacci function definition.
  
  (export "Fibonacci" (func $Fibonacci))
  
) ;; END program module
