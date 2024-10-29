(module
  (func $Get42 (result i32)
    ;; Variables
    
    (block $fun_exit1 (result i32)
      (i32.const 42)    ;; Put a 42 on the stack
    ) ;; end of function block.
  ) ;; END Get42 function definition.
  
  (export "Get42" (func $Get42))
  
) ;; END program module
