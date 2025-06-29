To install:

1) open terminal and "cd" to your directory where the repository is cloned
2) then type sudo insmod stopwatch.ko

These are userspace commands to be used in sequence:


1) echo start | sudo tee /dev/stopwatch

2) echo stop | sudo tee /dev/stopwatch

3) echo elapsed | sudo tee /dev/stopwatch

4) sudo dmesg 
