CFLAGS += -Werror -Wall
all: test pam_hassio.so

clean:
	$(RM) test pam_hassio.so *.o

pam_hassio.so: src/pam_hassio.c
	$(CC) $(CFLAGS) -fPIC -shared -Xlinker -x -o $@ $< -lcurl

test: src/test.c
	$(CC) $(CFLAGS) -o $@ $< -lpam -lpam_misc
