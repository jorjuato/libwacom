CXX= gcc
CXXFLAGS =-Wall  -O2 -c 
PROGS= dual unblock signal nosignal
FILES = wacserial.c wacusb.c wactablet.c

all: dual
#$(PROGS)

dual:
	$(CXX) $(CXXFLAGS) $(FILES) wacthread.c 

	ar rcs libwacom.a wacthread.o wacserial.o wacusb.o wactablet.o

unblock:
	$(CXX) $(CXXFLAGS) $(FILES)  wacomthread_unblock.c 

	ar rcs libwacom.a *.o


signal:
	$(CXX) $(CXXFLAGS) $(FILES) wacomthread_signal.c 

	ar rcs libwacom.a *.o

nosignal:
	$(CXX) $(CXXFLAGS) $(FILES) wacomthread.c 
	
	ar rcs libwacom.a *.o

clean: 
	rm -f *.o *.a
	
	
