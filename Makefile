Include_Dir := Include

all: 
	g++ -c Source/*.cpp -Os -I $(Include_Dir)
	ar src RARCExt.a *.o
	rm *.o
	g++ Main.cpp RARCExt.a -o RARCExt