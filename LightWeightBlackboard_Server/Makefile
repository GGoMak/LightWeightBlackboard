blackboard : main.o blackboard.o filemodule.o
	gcc -o blackboard main.o blackboard.o filemodule.o -lpthread -lmysqlclient -g

main.o : main.c blackboard.h
	gcc -c -o main.o main.c -lpthread -lmysqlclient

blackboard.o : blackboard.c blackboard.h
	gcc -c -o blackboard.o blackboard.c -lpthread -lmysqlclient

filemodule.o : filemodule.c blackboard.h
	gcc -c -o filemodule.o filemodule.c -lpthread -lmysqlclient

clean :
	rm *.o 
