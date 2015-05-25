g++ -c 1_0905099.cpp -I"./"
g++ -o a.exe 1_0905099.o -L"./" -lglu32 -lglut32 -lopengl32 -Wl,--subsystem,windows
a.exe