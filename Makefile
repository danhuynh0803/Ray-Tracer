all:

mainer:
	g++ mainer.cpp -o mainer -g
	mainer.exe

clean:
	del mainer.exe
	del *~
