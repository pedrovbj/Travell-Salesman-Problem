 #********************************************************
 #* INSTITUTO DE CIENCIAS MATEMATICAS E DE COMPUTACAO DA *
 #* UNIVERSIDADE DE SAO PAULO (ICMC-USP)                 *
 #*                                                      *
 #* Avaliacao Bimestral AB1 (26/05 a 29/05 de 2017)      *
 #* Programacao Concorrente (SSC0742)                    *
 #*                                                      *
 #* Professor Paulo Sergio Lopes de Souza                *
 #*                                                      *
 #* Aluno Pedro Virgilio Basilio Jeronymo (8910559)      *
 #*                                                      *
 #********************************************************
 #* Problema do Caixeiro Viajante                        *
 #********************************************************

NAME := pcv
SRCS := $(wildcard *.c)
OBJS_TADS := circularArray.o linkedList.o adjMatrix.o
OBJS_MASTER := master.o
OBJS_SLAVE := slave.o
OBJS := $(OBJS_TADS) $(OBJS_MASTER) $(OBJS_SLAVE)

all: master slave

master: $(OBJS)
	mpicc -Wall $(OBJS_MASTER) $(OBJS_TADS) -o $(NAME)

slave: $(OBJS)
	mpicc -pthread -Wall $(OBJS_SLAVE) $(OBJS_TADS) -o slave

$(OBJS):
	mpicc -pthread -c -Wall $(SRCS)

run:
	mpirun -np 1 ./$(NAME) file.in

clean:
	@- $(RM) $(NAME) slave
	@- $(RM) $(OBJS)
