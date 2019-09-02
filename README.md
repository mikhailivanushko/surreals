# surreals

a C++ library that implements J.H. Conway's surreal numbers with basic functionality:

* *Surreal* - a class that represents surreal numbers with finite left and right sets.
    * Comparison and Ordering
    * Addition, Negation, Multiplication
    * Two-way conversion with *float* and *int*
    * Display

* *SurrealInf* - a class that represents surreal numbers with infinite/finite sets.
    * Conversion from *Surreal* or construction from two functions returning *SurrealInf*
    * Display