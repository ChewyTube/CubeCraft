#pragma once 
#include "Context.h"
namespace cubecraft {
	static void Init(GLFWwindow* window) {
		Context::Init(window);
	}
	static void Quit() {
		Context::Quit();
	}
}