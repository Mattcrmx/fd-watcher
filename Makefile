monitor:
	gcc -g \
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
 		-o monitor_fd monitor_fd.c \
 		-lm
