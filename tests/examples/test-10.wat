(module
  (func $Factorial (param $var0 i32) (result i32)
    ;; Variables
    (local $var1 i32)  ;; Variable: result
    (local $var2 i32)  ;; Variable: i
    
    (block $fun_exit1 (result i32)
      ;; ASSIGNMENT
        (i32.const 1)    ;; Put a 1 on the stack
        (local.set $var1)    ;; Set var 'result' from stack
        (local.get $var1)    ;; Place var 'result' onto stack
      ;; END ASSIGNMENT (with result on stack)
      (drop)  ;; remove unused value on stack.
      ;; ASSIGNMENT
        (i32.const 2)    ;; Put a 2 on the stack
        (local.set $var2)    ;; Set var 'i' from stack
        (local.get $var2)    ;; Place var 'i' onto stack
      ;; END ASSIGNMENT (with result on stack)
      (drop)  ;; remove unused value on stack.
      (block $exit1   ;; Outer block for breaking while loop.
        (loop $loop1  ;; Inner loop for continuing while.
          ;; WHILE Test condition...
          (local.get $var2)    ;; Place var 'i' onto stack
          (local.get $var0)    ;; Place var 'n' onto stack
          (i32.le_s)  ;; Stack2 <= Stack1
          (i32.eqz)  ;; Invert the result of the test condition.
          (br_if $exit1)  ;; If condition is false (0), exit the loop
          ;; WHILE Loop body...
          ;; ASSIGNMENT
            (local.get $var1)    ;; Place var 'result' onto stack
            (local.get $var2)    ;; Place var 'i' onto stack
            (i32.mul)   ;; Stack2 * Stack1
            (local.set $var1)    ;; Set var 'result' from stack
            (local.get $var1)    ;; Place var 'result' onto stack
          ;; END ASSIGNMENT (with result on stack)
          (drop)  ;; remove unused value on stack.
          ;; ASSIGNMENT
            (local.get $var2)    ;; Place var 'i' onto stack
            (i32.const 1)    ;; Put a 1 on the stack
            (i32.add)   ;; Stack2 + Stack1
            (local.set $var2)    ;; Set var 'i' from stack
            (local.get $var2)    ;; Place var 'i' onto stack
          ;; END ASSIGNMENT (with result on stack)
          (drop)  ;; remove unused value on stack.
          ;; WHILE start next loop.
          (br $loop1)   ;; Jump back to the start of the loop
        ) ;; End loop
      )   ;; End block
      (local.get $var1)    ;; Place var 'result' onto stack
    ) ;; end of function block.
  ) ;; END Factorial function definition.
  
  (export "Factorial" (func $Factorial))
  
) ;; END program module
