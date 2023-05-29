#pragma once

#include "../inc/scop.hpp"

#define WIDTH 1920
#define HEIGHT 1080

class	Screen {
public:
	Screen();
	GLFWwindow	*getWindow() const;
private:
	GLFWwindow* window;
};

