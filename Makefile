CC=gcc
DEPS= monitor_fd.h
OBJ= monitor_fd.c watch_fd.c
CFLAGS = -g \
		-W \
		-Wall \
		-Wshadow \
		-Wwrite-strings \
		-Wstrict-prototypes \
		-Wmissing-prototypes \
		-Wmissing-declarations \
		-D_GNU_SOURCE \
		-D_REENTRANT \
		-D_LINUX_ \


%.o: %.c $(DEPS)
	$(CC) -c -o $@ $< $(CFLAGS)

fd-watcher: $(OBJ)
	$(CC) -o $@ $^ $(CFLAGS)
