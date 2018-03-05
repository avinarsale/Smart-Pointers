# Smart Pointers

Implemented a non-intrusive, thread-safe, exception-safe, reference-counting smart pointer named cs540::SharedPtr modeled to std:shared_ptr. It allows different smart pointers in different threads to be safely assigned and unassigned to the same shared objects by using mutex locks. 
