CC = g++ 
DEBUG = -g 
PROFILE = -g -pg
CFLAGS = -Wall -O2  
INCS=
LIBS=


COMPILE= $(CC) $(INCS) $(CFLAGS) 

SRC =$(wildcard *.cc) 
OBJS=$(SRC:.cc=.o)


test: $(OBJS) 
	@echo Linking files to make test
	@$(CC) $(LFLAGS) -o $@ $(OBJS) $(LIBS) 

%.o : %.cc
	@echo Compiling $<
	@$(COMPILE) -MD -c -o $@ $<

-include $(SRC:.cc=.d)

clean:
	rm -rf test *.o *.d *~ 