
This was a final project for Advanced Computer Architecture at NTU in 2013.

Link: [Final Report (pdf)](https://drive.google.com/file/d/0Byy1mIQw8wwYRDk0TGx1bjVqWU0/view?usp=sharing)

TODO LIST:

- Add include guards to header files
- Add include files

- Modify ARMulator
    - Merge MMU modifications
    - Merge CPU modifications

- Fill in all TODOs and obvious compiler errors
    - How to design the Address class? Can we do bitwise packing?
    - foreach
    - Some more


Important tasks:
    - Create & run benchmarks
        -> Several matmuls with matrices of different sizes
        -> Run benchmarks with all kinds of different settings
    - Take stats from sim* variables and average them over tasks (especially: Cache, MMU, Router)
    - Print screenshots of simulation stats to file
        -> Visualize results using Matlab
    - Do not send a request to memory on L2 miss, if it is already being requested?



Less important:
    Extras:
        - Reduce congestion by batching multiple requests to the same line into a single packet


    Far-away extras:
        - Add associativity to caches & good cache replacement algorithm
        - Broadcasts
        - Cache coherence
        - Allow for dynamic Core Block sizes
            - Scheduler must core block sizes against capacity miss rates
        - Produce compiler that understands the new paradigm annotations and can automatically instrument the code with place holders

        
Comparison to proposal
    - No broadcast
    - No cache coherence
    - No __syncthreads() instruction
    - Only simulate load, not store
