all:

mainer:
	g++ mainer.cpp -o mainer -g

clean:
	del mainer.exe
	del mainer
	del *~
