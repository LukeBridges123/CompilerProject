(module
  (func $FindNextMult5Not10 (param $var0 i32) (result i32)
    ;; Variables
    
    (block $fun_exit1 (result i32)
      (block $exit1   ;; Outer block for breaking while loop.
        (loop $loop1  ;; Inner loop for continuing while.
          ;; WHILE Test condition...
          (i32.const 1)    ;; Put a 1 on the stack
          (i32.eqz)  ;; Invert the result of the test condition.
          (br_if $exit1)  ;; If condition is false (0), exit the loop
          ;; WHILE Loop body...
          ;; ASSIGNMENT
            (local.get $var0)    ;; Place var 'start' onto stack
            (i32.const 1)    ;; Put a 1 on the stack
            (i32.add)   ;; Stack2 + Stack1
            (local.set $var0)    ;; Set var 'start' from stack
            (local.get $var0)    ;; Place var 'start' onto stack
          ;; END ASSIGNMENT (with result on stack)
          (drop)  ;; remove unused value on stack.
          ;; Test condition for if.
          (local.get $var0)    ;; Place var 'start' onto stack
          (i32.const 10)    ;; Put a 10 on the stack
          (i32.rem_s) ;; Stack2 % Stack1
          (i32.const 0)    ;; Put a 0 on the stack
          (i32.eq)    ;; Stack2 == Stack1
          (if  ;; Execute code based on result of condition.
            (then ;; 'then' block
              (br $loop1) ;; 'continue' command.
            ) ;; End 'then'
          )  ;; End 'if'
          ;; Test condition for if.
          (local.get $var0)    ;; Place var 'start' onto stack
          (i32.const 5)    ;; Put a 5 on the stack
          (i32.rem_s) ;; Stack2 % Stack1
          (i32.const 0)    ;; Put a 0 on the stack
          (i32.eq)    ;; Stack2 == Stack1
          (if  ;; Execute code based on result of condition.
            (then ;; 'then' block
              (br $exit1) ;; 'break' command.
            ) ;; End 'then'
          )  ;; End 'if'
          ;; WHILE start next loop.
          (br $loop1)   ;; Jump back to the start of the loop
        ) ;; End loop
      )   ;; End block
      (local.get $var0)    ;; Place var 'start' onto stack
    ) ;; end of function block.
  ) ;; END FindNextMult5Not10 function definition.
  
  (export "FindNextMult5Not10" (func $FindNextMult5Not10))
  
) ;; END program module
