NAME		= 	scop
FRMWLIB		= 	lib/libglfw.3.3.dylib
GLADLIB		=	glad/libglad.a

SRCS		= 	$(wildcard src/*.cpp)
SRCS		+=  $(wildcard imgui/*.cpp)

OBJS		= 	$(patsubst srcs/%.cpp,objs/%.o,$(SRCS))

CC			= 	c++ -g
CFLAGS		= 	-framework OpenGl -std=c++11
FRMPATH		=	-I./GLFW/
GLADPATH	=	-I./glad/
FRMWK		=	-I/usr/include/GL
LINKER		= 	-L./lib
INCLUDE_DIR = 	-I./inc/
GLM_DIR		= 	-I./glm/
IMGUIDIR	=	-I./imgui/

CG = \033[92m
CY = \033[93m
CE = \033[0m

all:		$(NAME)

$(NAME):	$(OBJS)
			 $(CC) $(CFLAGS) $(GLADPATH) $(FRMPATH) $(LINKER) $(FRMWLIB) $(INCLUDE_DIR) $(GLM_DIR) $(IMGUIDIR) -o $@ $^ glad/glad.c -lglfw -Wno-deprecated
			 echo "\n\033[92m---> scop program created ✓\033[0m";

objs/%.o:	srcs/%.cpp
			@ mkdir -p objs
		 	@ $(CC) -c $< -o $@
		 	@ echo "\033[K$(CY)[RT] :$(CE) $(CG)Compiling $<$(CE) \033[1A";

clean:		
			@ /bin/rm -rf objs/
			@ echo "\033[1;33m---> All .o files cleared\033[0m \033[92m✓\033[0m";

fclean:		clean
			@ /bin/rm -f $(NAME)
			@ echo "\n\033[1;33m---> Everything cleared\033[2;00m \033[92m✓\033[0m";

re : fclean all

a: all 
	./scop resources/cow.obj

.PHONY: all, clean, fclean, re, a


# g++ -I./glad/ -I./GLFW/ -L./lib ./lib/libglfw.so.3 main.cpp Screen.cpp ShaderProgram.cpp glad/glad.c -lglfw -o scop