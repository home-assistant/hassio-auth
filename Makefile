CFLAGS += -Werror -Wall
all: test hassio.so

clean:
	$(RM) test hassio.so *.o

hassio.so: src/hassio.c
	$(CC) $(CFLAGS) -fPIC -shared -Xlinker -x -o $@ $< -lcurl

test: src/test.c
	$(CC) $(CFLAGS) -o $@ $< -lpam -lpam_misc
