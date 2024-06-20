# EXAMPLE FROM PREV CODE:


# all: coffee_shop.o register.o
# 	gcc coffee_shop.o register.o -o register 

# coffee_shop.o: coffee_shop.c
# 	gcc -Wall -c coffee_shop.c -o coffee_shop.o

# register.o: register.c
# 	gcc -Wall -c register.c -o register.o

# clean:
# 	rm *.o register

all: server.o
	g++ server.o -o server -lpthread

server.o: server.cpp
	g++ -Wall -c server.cpp -o server.o -lpthread

clean: 
	rm *.o server