FLAGS = -lglfw -lGLU -lGL -lX11 -lXrandr -lm -lcorona -llua -ldl
BUILD = build/game

game: main.o window.o canvas.o image.o font.o tiles.o geometry.o
	g++ -o $@ $+ $(FLAGS)
	mkdir -p build	
	cp $@ $(BUILD)


# deps
main.o: main.cpp common.h
	g++ -c $< 

window.o: window.cpp common.h
	g++ -c $< 

canvas.o: canvas.cpp common.h
	g++ -c $< 

image.o: image.cpp common.h
	g++ -c $< 

font.o: font.cpp common.h
	g++ -c $< 

tiles.o: tiles.cpp common.h
	g++ -c $< 

geometry.o: geometry.cpp common.h
	g++ -c $< 

