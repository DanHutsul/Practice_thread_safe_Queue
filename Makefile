CC = gcc

libmy_queue.so: my_queue.o
	$(CC) -fPIC -shared -pthread my_queue.o -o libmy_queue.so

clean:
	rm *.so *.o