# EXAMPLE FROM PREV CODE:


# all: coffee_shop.o register.o
# 	gcc coffee_shop.o register.o -o register 

# coffee_shop.o: coffee_shop.c
# 	gcc -Wall -c coffee_shop.c -o coffee_shop.o

# register.o: register.c
# 	gcc -Wall -c register.c -o register.o

# clean:
# 	rm *.o register

all: mailbox.o
	g++ mailbox.o -o mailbox

mailbox.o: mailbox.cpp
	g++ -Wall -c mailbox.cpp -o mailbox.o

clean: 
	rm *.o mailbox