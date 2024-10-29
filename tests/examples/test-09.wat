(module
  (func $Max3 (param $var0 i32) (param $var1 i32) (param $var2 i32) (result i32)
    ;; Variables
    
    (block $fun_exit1 (result i32)
      ;; Test condition for if.
      (local.get $var0)    ;; Place var 'v1' onto stack
      (local.get $var1)    ;; Place var 'v2' onto stack
      (i32.gt_s)  ;; Stack2 > Stack1
      (if (result i32) ;; Execute code based on result of condition.
        (then ;; 'then' block
          ;; Test condition for if.
          (local.get $var0)    ;; Place var 'v1' onto stack
          (local.get $var2)    ;; Place var 'v3' onto stack
          (i32.gt_s)  ;; Stack2 > Stack1
          (if (result i32) ;; Execute code based on result of condition.
            (then ;; 'then' block
              (local.get $var0)    ;; Place var 'v1' onto stack
            ) ;; End 'then'
            (else ;; 'else' block
              (local.get $var2)    ;; Place var 'v3' onto stack
            ) ;; End 'else'
          )  ;; End 'if'
        ) ;; End 'then'
        (else ;; 'else' block
          ;; Test condition for if.
          (local.get $var1)    ;; Place var 'v2' onto stack
          (local.get $var2)    ;; Place var 'v3' onto stack
          (i32.gt_s)  ;; Stack2 > Stack1
          (if (result i32) ;; Execute code based on result of condition.
            (then ;; 'then' block
              (local.get $var1)    ;; Place var 'v2' onto stack
            ) ;; End 'then'
            (else ;; 'else' block
              (local.get $var2)    ;; Place var 'v3' onto stack
            ) ;; End 'else'
          )  ;; End 'if'
        ) ;; End 'else'
      )  ;; End 'if'
    ) ;; end of function block.
  ) ;; END Max3 function definition.
  
  (export "Max3" (func $Max3))
  
) ;; END program module
