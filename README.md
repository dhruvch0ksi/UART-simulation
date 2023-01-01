
# UART simulation and testing

This project simulates the UART's Rx functionality for x86 Linux system using multithreading.



## Description

UART's Tx puts data into the fifo buffer. When the buffer gets full, an interrupt gets generated and the buffer is read by the UART's Rx. 

There are three threads in the application.

1. Transmitter thread: It acts just like a test case. It puts test data into the buffer
2. Receiver thread: When buffer gets full, this thread reads data from buffer and store it in storage space.
3. Background data processor thread: Once, data is read by receiver thread, processor thread validates the data in the background and output number of valid sequence.


## Explanation

Whenever the buffer is empty, transmitter thread loads the buffer with test data. Once the buffer is full, the receiver thread will read that data and copy it to storage space. Once, the data is in storage, background process is signalled and it takes care of validating the data. 

NUM_TEST_CASE is the number of test cases.

<h2>Screenshot 


<img width="996" alt="Screenshot 2023-01-01 at 12 54 35 PM" src="https://user-images.githubusercontent.com/94764272/210163720-8bac1605-0e37-4f9a-b1ce-6fd17cf4a8a5.png">

<img width="384" alt="Screenshot 2023-01-01 at 12 55 22 PM" src="https://user-images.githubusercontent.com/94764272/210163754-94bbbdfc-48c9-4fe3-9296-d8bc1d3f6477.png">
