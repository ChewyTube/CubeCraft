#include "../cubecraft/Block.h"

namespace cubecraft {
	Vertex* getVertices(Faces face, int w_x, int w_y, int w_z) {
		switch (face)
		{
		case cubecraft::UP:
		{
			Vertex vertices[4] = {
				Vertex{{0.0f + w_x, 1.0f + w_y, 0.0f + w_z}, {0.0f, 0.0f}},
				Vertex{{0.0f + w_x, 1.0f + w_y, 1.0f + w_z}, {0.0f, 1.0f}},
				Vertex{{1.0f + w_x, 1.0f + w_y, 0.0f + w_z}, {1.0f, 0.0f}},
				Vertex{{1.0f + w_x, 1.0f + w_y, 1.0f + w_z}, {1.0f, 1.0f}},
			};
			return vertices;
			break;
		}
		case cubecraft::DOWN:
		{
			Vertex vertices[4] = {
				Vertex{{0.0f + w_x, 0.0f + w_y, 0.0f + w_z}, {0.0f, 0.0f}},
				Vertex{{1.0f + w_x, 0.0f + w_y, 0.0f + w_z}, {1.0f, 0.0f}},
				Vertex{{0.0f + w_x, 0.0f + w_y, 1.0f + w_z}, {0.0f, 1.0f}},
				Vertex{{1.0f + w_x, 0.0f + w_y, 1.0f + w_z}, {1.0f, 1.0f}},
			};
			return vertices;
			break;
		}
		case cubecraft::LEFT:
			break;
		case cubecraft::RIGHT:
			break;
		case cubecraft::FRONT:
			break;
		case cubecraft::BACK:
			break;
		default:
			break;
		}
		return {};
	}
}