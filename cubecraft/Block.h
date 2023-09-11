#pragma once

#include "Vertex.h"

namespace cubecraft {
	enum Faces
	{
		UP ,DOWN, LEFT, RIGHT, FRONT, BACK
	};

	class Block
	{
	public:
		//Vertices getVertices(Faces face, int w_x, int w_y, int w_z);
	private:

	};
	Vertex* getVertices(Faces face, int w_x, int w_y, int w_z);
}