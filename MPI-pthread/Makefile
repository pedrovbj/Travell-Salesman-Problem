NAME := pcv
SRCS := $(wildcard *.c)
OBJS_TADS := circularArray.o linkedList.o
OBJS_MASTER := master.o
OBJS_SLAVE := slave.o
OBJS := $(OBJS_TADS) $(OBJS_MASTER) $(OBJS_SLAVE)

x: clean all run

all: master slave

master: $(OBJS)
	mpicc -Wall -g $(OBJS_MASTER) $(OBJS_TADS) -o $(NAME)

slave: $(OBJS)
	mpicc -Wall -g $(OBJS_SLAVE) $(OBJS_TADS) -o slave

$(OBJS):
	mpicc -c -Wall -g $(SRCS)

run:
	mpirun -np 1 ./$(NAME) file.in

clean:
	@- $(RM) $(NAME) slave
	@- $(RM) $(OBJS)
