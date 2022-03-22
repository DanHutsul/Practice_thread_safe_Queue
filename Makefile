CC = gcc

my_queue_t.so: my_queue_t.o
	$(CC) -fPIC -shared -pthread my_queue.o -o libmy_queue.so

clean:
	rm *.so *.o