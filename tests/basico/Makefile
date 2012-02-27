

CXX = gcc-4.1
#CXXFLAGS = -g -Wall -O3
CXXFLAGS = -Wall -O1 `sdl-config --cflags --libs` -lSDL_gfx

LIBS = -L/usr/lib -L. -lSDL -lSDL_gfx -lwacom
LIBS1 = -L. -lGL -lGLU -lglut -lpthread -lwacom
LIBS2 = -L. -lpthread -lwacom -lSDL_gfx
INC = -I/usr/include/SDL -D_GNU_SOURCE=1 -D_REENTRANT
MODULES = pili_draw.c sdlgl_control.c parser.c auxfuncs.c ./font/nfontc.c
PROGS = pili
all: $(PROGS)

pili:
	@ echo
	@ echo "	#  COMPILANDO EXPERIMENTO            #"
	@ echo
	$(CXX) $(CXXFLAGS) $(INC) main.c  $(MODULES) $(LIBS)   -o pili



clean:
	rm -f *.o pili pili.log





