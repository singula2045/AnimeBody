OPT = -O2 -w
INC = -L/usr/X11R6/lib
CC = cc
LIB = -lX11 -lm
ABODY = Body.o BodyModel.o BodyView.o

abody.out : $(ABODY)
	$(CC) $(INC) -o abody.out $(ABODY) $(LIB)

Body.o : Body.cpp Body.h 
	$(CC) $(OPT) -c Body.cpp
BodyModel.o : BodyModel.cpp BodyModel.h 
	$(CC) $(OPT) -c BodyModel.cpp
BodyView.o : BodyView.cpp BodyView.h 
	$(CC) $(OPT) -c BodyView.cpp

clean :
	\rm *.o
