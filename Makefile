CC=g++
CFLAGS=-lwiringPi -std=c++11
EXE=test_frammo
CPPFILES=frammo.cpp main.cpp i2c.cpp common.cpp sdes.cpp
HFILES=frammo.h i2c.h sdes.h

$(EXE): $(HFILES) $(CPPFILES)
	$(CC) -o $(EXE) $(CPPFILES) $(CFLAGS)

run:$(EXE)
	sudo ./$(EXE)

clean:
	rm -fv $(EXE) *~

