#include "app.h"


int main() {
	App app;
	if (app.Init())
		return -1;
	return app.Run();
}


void framebuffer_size_callback(GLFWwindow* window, int width, int height) {
	App::GetInstance()->framebuffer_size_callback(window, width, height);
}

void mouse_callback(GLFWwindow* window, double xpos, double ypos) {
	App::GetInstance()->mouse_callback(window, xpos, ypos);
}

void scroll_callback(GLFWwindow* window, double xoffset, double yoffset)
{
	App::GetInstance()->scroll_callback(window, xoffset, yoffset);
}