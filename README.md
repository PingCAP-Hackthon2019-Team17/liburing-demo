# liburing-demo
A simple demo to show performance grap between write+fsync and async io-uring write+fsync

# Requirement

Please make sure your linux kernel vesion >=5.1.x which include the *liburing* dependency, we use the version here: 

```
Linux rocks-uring 5.3.0-1004-gcp #4-Ubuntu SMP Wed Oct 9 17:54:26 UTC 2019 x86_64 x86_64 x86_64 GNU/Linux
```

Also we need to make sure the g++>=9.2.1, here we use:
```
g++ --version
g++ (Ubuntu 9.2.1-9ubuntu2) 9.2.1 20191008
```

# How to run the benchmark

```
git clone https://github.com/PingCAP-Hackthon2019-Team17/liburing-demo.git
cd liburing-demo
make
./sync-io.out  <your-target-path>/sync-io-output-file.txt
./uring-io.out <your-target-path>/uring-io-output-file.txt
```

We have the tests under our host: 

```
➜  io-bench git:(master) ./sync-io.out a.txt 
start the bench write
End the native write & fdatasync bench write: 0.065001(sec)
➜  io-bench git:(master) ./sync-io.out a.txt
start the bench write
End the native write & fdatasync bench write: 0.069517(sec)
➜  io-bench git:(master) ./sync-io.out a.txt
start the bench write
End the native write & fdatasync bench write: 0.069618(sec)
➜  io-bench git:(master) ./uring-io.out b.txt
start the bench write
End the uring-async write & sync bench write: 0.054406(sec)
➜  io-bench git:(master) ./uring-io.out b.txt
start the bench write
End the uring-async write & sync bench write: 0.056496(sec)
➜  io-bench git:(master) ./uring-io.out b.txt
start the bench write
End the uring-async write & sync bench write: 0.058327(sec)
```

Seems the uring-async will have the +17.7% performance improvement.
