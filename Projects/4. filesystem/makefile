# EXAMPLE FROM PREV CODE:


# all: coffee_shop.o register.o
# 	gcc coffee_shop.o register.o -o register 

# coffee_shop.o: coffee_shop.c
# 	gcc -Wall -c coffee_shop.c -o coffee_shop.o

# register.o: register.c
# 	gcc -Wall -c register.c -o register.o

# clean:
# 	rm *.o register

all: fs.o
	g++ fs.o -o fs

fs.o: fs.cpp
	g++ -Wall -c fs.cpp -o fs.o

clean: 
	rm *.o fs