all:
	g++ sync-io.cpp -o sync-io.out
	g++ uring-io.cpp -o uring-io.out -luring -Wall -O2

clean:
	rm -rf *.out
