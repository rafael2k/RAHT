CPP=g++
CXXFLAGS=-Wall -g
LDFLAGS=

all: raht_enc raht_dec

raht_enc: haar3D.o main_cli_e.o sqrtif.o file/file.o
	$(CPP) $(LDFLAGS) haar3D.o main_cli_e.o sqrtif.o file/file.o -o raht_enc

raht_dec: haar3D.o main_cli_d.o sqrtif.o file/file.o
	$(CPP) $(LDFLAGS) haar3D.o main_cli_d.o sqrtif.o file/file.o -o raht_dec

haar3D.o: haar3D.cpp haar3D.h
	$(CPP) $(CXXFLAGS) -c haar3D.cpp -o haar3D.o

sqrtif.o: sqrtif.cpp
	$(CPP) $(CXXFLAGS) -c sqrtif.cpp -o sqrtif.o

#voxel.o: voxel.cpp voxel.h
#	$(CPP) $(CXXFLAGS) -c voxel.cpp -o voxel.o

file/file.o: file/file.cpp file/file.h
	$(CPP) $(CXXFLAGS) -c file/file.cpp -o file/file.o

main_cli_e.o: main_cli.cpp
	$(CPP) $(CXXFLAGS) -DRAHT_ENC -c main_cli.cpp -o main_cli_e.o

main_cli_d.o: main_cli.cpp
	$(CPP) $(CXXFLAGS) -DRAHT_DEC -c main_cli.cpp -o main_cli_d.o

.PHONY: clean

clean:
	rm -f *.o raht_enc raht_dec
