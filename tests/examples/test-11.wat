(module
  (func $GCD (param $var0 i32) (param $var1 i32) (result i32)
    ;; Variables
    (local $var2 i32)  ;; Variable: temp
    
    (block $fun_exit1 (result i32)
      (block $exit1   ;; Outer block for breaking while loop.
        (loop $loop1  ;; Inner loop for continuing while.
          ;; WHILE Test condition...
          (local.get $var1)    ;; Place var 'b' onto stack
          (i32.const 0)    ;; Put a 0 on the stack
          (i32.ne)    ;; Stack2 != Stack1
          (i32.eqz)  ;; Invert the result of the test condition.
          (br_if $exit1)  ;; If condition is false (0), exit the loop
          ;; WHILE Loop body...
          ;; ASSIGNMENT
            (local.get $var1)    ;; Place var 'b' onto stack
            (local.set $var2)    ;; Set var 'temp' from stack
            (local.get $var2)    ;; Place var 'temp' onto stack
          ;; END ASSIGNMENT (with result on stack)
          (drop)  ;; remove unused value on stack.
          ;; ASSIGNMENT
            (local.get $var0)    ;; Place var 'a' onto stack
            (local.get $var1)    ;; Place var 'b' onto stack
            (i32.rem_s) ;; Stack2 % Stack1
            (local.set $var1)    ;; Set var 'b' from stack
            (local.get $var1)    ;; Place var 'b' onto stack
          ;; END ASSIGNMENT (with result on stack)
          (drop)  ;; remove unused value on stack.
          ;; ASSIGNMENT
            (local.get $var2)    ;; Place var 'temp' onto stack
            (local.set $var0)    ;; Set var 'a' from stack
            (local.get $var0)    ;; Place var 'a' onto stack
          ;; END ASSIGNMENT (with result on stack)
          (drop)  ;; remove unused value on stack.
          ;; WHILE start next loop.
          (br $loop1)   ;; Jump back to the start of the loop
        ) ;; End loop
      )   ;; End block
      (local.get $var0)    ;; Place var 'a' onto stack
    ) ;; end of function block.
  ) ;; END GCD function definition.
  
  (export "GCD" (func $GCD))
  
) ;; END program module
