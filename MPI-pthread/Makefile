NAME := pcv
SRCS := $(wildcard *.c)
OBJS_TADS := circularArray.o linkedList.o
OBJS_MASTER := master.o
OBJS_SLAVE := slave.o
OBJS := $(OBJS_TADS) $(OBJS_MASTER) $(OBJS_SLAVE)

all: master slave

master: $(OBJS)
	mpicc -pthread -Wall -g $(OBJS_MASTER) $(OBJS_TADS) -o $(NAME)

slave: $(OBJS)
	mpicc -pthread -Wall -g $(OBJS_SLAVE) $(OBJS_TADS) -o slave

$(OBJS):
	mpicc -pthread -c -Wall -g $(SRCS)

run:
	mpirun -np 1 ./$(NAME) file.in

clean:
	@- $(RM) $(NAME) slave
	@- $(RM) $(OBJS)

x:
	make clean
	make all
	make run
