;; Function to allocate space for a string; add one to size and place null terminator there.
(func $_str_alloc (param $size i32) (result i32)
  (local $null_pos i32)        ;; Local variable for where to place null terminator.
  (global.get $free_mem)       ;; Staring free_mem is start of new string; put on stack to return.
  (local.get $size)            ;; Load size of new string.
  (i32.add                     ;; Add free_mem and size to get position for null terminator.
    (global.get $free_mem)     ;; Load a second copy to adjust new free_mem.
    (local.get $size))
  (local.set $null_pos)        ;; Save position for null terminator to use it again.
  (i32.store8                  ;; Place null terminator.
    (local.get $null_pos)
    (i32.const 0))
  (i32.add
    (i32.const 1)
    (local.get $null_pos))     ;; Increment past null for new free_mem pos.
  (global.set $free_mem))      ;; Update free memory start.
