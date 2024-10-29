(module
  (func $TestEven (param $var0 i32) (result i32)
    ;; Variables
    
    (block $fun_exit1 (result i32)
      (local.get $var0)    ;; Place var 'test' onto stack
      (i32.const 2)    ;; Put a 2 on the stack
      (i32.rem_s) ;; Stack2 % Stack1
      i32.eqz ;; Boolean NOT.
    ) ;; end of function block.
  ) ;; END TestEven function definition.
  
  (export "TestEven" (func $TestEven))
  
) ;; END program module
