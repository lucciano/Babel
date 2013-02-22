-- The Construct
--
-- This is the "loading program" created by the Resistance to run 
-- simulations or upload virtual objects when they hack into the 
-- Matrix...

(   (root       (tlist '/babel/tag/bvm'
                    bvm_code
                    bvm_stack
                    jump_table
                    sym_table))

    (bvm_code   (tlist '/babel/tag/bvm_code'
                    code
                    nil)) -- rstack

        -- (rstack (ptr nil))

    (bvm_stack  (tlist '/babel/tag/bvm_stack' 
                    (list nil   -- dstack
                    nil))) -- ustack

        -- (dstack (ptr nil))
        -- (ustack (ptr nil))

    (jump_table (tlist '/babel/tag/jump_table' nil ))

    (sym_table (tlist '/babel/tag/sym_table' sym_table_hash_table ))

    (sym_table_hash_table (tlist '/babel/tag/hash_table' symbol )))

