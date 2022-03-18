CC = gcc

my_queue_t.so: my_queue_t.o
	$(CC) -fPIC -shared -pthread my_queue_t.o -o my_queue_t.so

clean:
	rm *.so *.o