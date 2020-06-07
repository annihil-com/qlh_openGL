CFLAGS=-m32 -Wall -fPIC
LDFLAGS=-m32 -shared -ldl -lopcodes

libo = qlh_openGL.o ghf/asm.o ghf/elf.o ghf/utils.o ghf/vfunction.o
libc = qlh_openGL.c ghf/asm.c ghf/elf.c ghf/utils.c ghf/vfunction.c ghf/ghf.h

libname = qlh_openGL

.PHONY: all
all: $(libname).so

$(libname).so: $(libo)
	$(CC) $^ -o $@ $(LDFLAGS)

$(libname).o: $(libc)
	$(CC) $(CFLAGS) $< -c -o $@

.PHONY: clean
clean:
	-rm -rf $(libname).so $(libo)
