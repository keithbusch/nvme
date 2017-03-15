CFLAGS ?= -O2 -g -Wall -Werror
CFLAGS += -std=gnu99
CPPFLAGS += -D_GNU_SOURCE -D__CHECK_ENDIAN__
NVME = nvme

default: $(NVME)

OBJS := program.o json.o nvme-models.o nvme-print.o os/linux-nvme.o

nvme: nvme.c nvme.h cmd_handler.h $(OBJS)
	$(CC) $(CPPFLAGS) $(CFLAGS) nvme.c -o $(NVME) $(OBJS)

%.o: %.c %.h nvme.h nvme-cli.h nvme-tstruct.h
	$(CC) $(CPPFLAGS) $(CFLAGS) -c $< -o $@

%.i: %.c %.h nvme.h nvme-cli.h nvme-tstruct.h
	$(CC) -E $< -o $@
clean:
	$(RM) $(NVME) *.o os/*.o *.i os/*.i

.PHONY: default FORCE OBJS
