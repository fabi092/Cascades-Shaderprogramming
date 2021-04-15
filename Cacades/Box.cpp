#include "Box.h"
#include "Vertex.h"
#include "Triangle.h"

Triangle* Box::GetTris(glm::vec3 size, glm::vec3 textureRepeat)
{
	Triangle* tris = new Triangle[12]
	{
		//Front
		Triangle(Vertex(glm::vec3(+0.5f * size.x, -0.5f * size.y, -0.5f * size.z), glm::vec3( 0.0f,  0.0f, -1.0f), glm::vec2( 0.0f * textureRepeat.x, 0.0f * textureRepeat.y)), 
				 Vertex(glm::vec3(-0.5f * size.x, -0.5f * size.y, -0.5f * size.z), glm::vec3( 0.0f,  0.0f, -1.0f), glm::vec2( 1.0f * textureRepeat.x, 0.0f * textureRepeat.y)), 
				 Vertex(glm::vec3(-0.5f * size.x, +0.5f * size.y, -0.5f * size.z), glm::vec3( 0.0f,  0.0f, -1.0f), glm::vec2( 1.0f * textureRepeat.x, 1.0f * textureRepeat.y))),
		Triangle(Vertex(glm::vec3(-0.5f * size.x, +0.5f * size.y, -0.5f * size.z), glm::vec3( 0.0f,  0.0f, -1.0f), glm::vec2( 1.0f * textureRepeat.x, 1.0f * textureRepeat.y)), 
				 Vertex(glm::vec3(+0.5f * size.x, +0.5f * size.y, -0.5f * size.z), glm::vec3( 0.0f,  0.0f, -1.0f), glm::vec2( 0.0f * textureRepeat.x, 1.0f * textureRepeat.y)), 
				 Vertex(glm::vec3(+0.5f * size.x, -0.5f * size.y, -0.5f * size.z), glm::vec3( 0.0f,  0.0f, -1.0f), glm::vec2( 0.0f * textureRepeat.x, 0.0f * textureRepeat.y))),
		//Back												   								   
		Triangle(Vertex(glm::vec3(-0.5f * size.x, -0.5f * size.y, +0.5f * size.z), glm::vec3( 0.0f,  0.0f,  1.0f), glm::vec2( 0.0f * textureRepeat.x, 0.0f * textureRepeat.y)),
				 Vertex(glm::vec3(+0.5f * size.x, -0.5f * size.y, +0.5f * size.z), glm::vec3( 0.0f,  0.0f,  1.0f), glm::vec2( 1.0f * textureRepeat.x, 0.0f * textureRepeat.y)),
				 Vertex(glm::vec3(+0.5f * size.x, +0.5f * size.y, +0.5f * size.z), glm::vec3( 0.0f,  0.0f,  1.0f), glm::vec2( 1.0f * textureRepeat.x, 1.0f * textureRepeat.y))),
		Triangle(Vertex(glm::vec3(+0.5f * size.x, +0.5f * size.y, +0.5f * size.z), glm::vec3( 0.0f,  0.0f,  1.0f), glm::vec2( 1.0f * textureRepeat.x, 1.0f * textureRepeat.y)),
				 Vertex(glm::vec3(-0.5f * size.x, +0.5f * size.y, +0.5f * size.z), glm::vec3( 0.0f,  0.0f,  1.0f), glm::vec2( 0.0f * textureRepeat.x, 1.0f * textureRepeat.y)),
				 Vertex(glm::vec3(-0.5f * size.x, -0.5f * size.y, +0.5f * size.z), glm::vec3( 0.0f,  0.0f,  1.0f), glm::vec2( 0.0f * textureRepeat.x, 0.0f * textureRepeat.y))),
		//Left												   								   
		Triangle(Vertex(glm::vec3(-0.5f * size.x, +0.5f * size.y, +0.5f * size.z), glm::vec3(-1.0f,  0.0f,  0.0f), glm::vec2( 0.0f * textureRepeat.z, 0.0f * textureRepeat.y)),
				 Vertex(glm::vec3(-0.5f * size.x, +0.5f * size.y, -0.5f * size.z), glm::vec3(-1.0f,  0.0f,  0.0f), glm::vec2( 1.0f * textureRepeat.z, 0.0f * textureRepeat.y)),
				 Vertex(glm::vec3(-0.5f * size.x, -0.5f * size.y, -0.5f * size.z), glm::vec3(-1.0f,  0.0f,  0.0f), glm::vec2( 1.0f * textureRepeat.z, 1.0f * textureRepeat.y))),
		Triangle(Vertex(glm::vec3(-0.5f * size.x, -0.5f * size.y, -0.5f * size.z), glm::vec3(-1.0f,  0.0f,  0.0f), glm::vec2( 1.0f * textureRepeat.z, 1.0f * textureRepeat.y)),
				 Vertex(glm::vec3(-0.5f * size.x, -0.5f * size.y, +0.5f * size.z), glm::vec3(-1.0f,  0.0f,  0.0f), glm::vec2( 0.0f * textureRepeat.z, 1.0f * textureRepeat.y)),
				 Vertex(glm::vec3(-0.5f * size.x, +0.5f * size.y, +0.5f * size.z), glm::vec3(-1.0f,  0.0f,  0.0f), glm::vec2( 0.0f * textureRepeat.z, 0.0f * textureRepeat.y))),
		//Right																		   		 						  
		Triangle(Vertex(glm::vec3(+0.5f * size.x, +0.5f * size.y, -0.5f * size.z), glm::vec3( 1.0f,  0.0f,  0.0f), glm::vec2( 0.0f * textureRepeat.z, 0.0f * textureRepeat.y)),
				 Vertex(glm::vec3(+0.5f * size.x, +0.5f * size.y, +0.5f * size.z), glm::vec3( 1.0f,  0.0f,  0.0f), glm::vec2( 1.0f * textureRepeat.z, 0.0f * textureRepeat.y)),
				 Vertex(glm::vec3(+0.5f * size.x, -0.5f * size.y, +0.5f * size.z), glm::vec3( 1.0f,  0.0f,  0.0f), glm::vec2( 1.0f * textureRepeat.z, 1.0f * textureRepeat.y))),
		Triangle(Vertex(glm::vec3(+0.5f * size.x, -0.5f * size.y, +0.5f * size.z), glm::vec3( 1.0f,  0.0f,  0.0f), glm::vec2( 1.0f * textureRepeat.z, 1.0f * textureRepeat.y)),
				 Vertex(glm::vec3(+0.5f * size.x, -0.5f * size.y, -0.5f * size.z), glm::vec3( 1.0f,  0.0f,  0.0f), glm::vec2( 0.0f * textureRepeat.z, 1.0f * textureRepeat.y)),
				 Vertex(glm::vec3(+0.5f * size.x, +0.5f * size.y, -0.5f * size.z), glm::vec3( 1.0f,  0.0f,  0.0f), glm::vec2( 0.0f * textureRepeat.z, 0.0f * textureRepeat.y))),
		//Bottom																		   	 							  
		Triangle(Vertex(glm::vec3(-0.5f * size.x, -0.5f * size.y, -0.5f * size.z), glm::vec3( 0.0f, -1.0f,  0.0f), glm::vec2( 0.0f * textureRepeat.x, 0.0f * textureRepeat.z)),
				 Vertex(glm::vec3(+0.5f * size.x, -0.5f * size.y, -0.5f * size.z), glm::vec3( 0.0f, -1.0f,  0.0f), glm::vec2( 1.0f * textureRepeat.x, 0.0f * textureRepeat.z)),
				 Vertex(glm::vec3(+0.5f * size.x, -0.5f * size.y, +0.5f * size.z), glm::vec3( 0.0f, -1.0f,  0.0f), glm::vec2( 1.0f * textureRepeat.x, 1.0f * textureRepeat.z))),
		Triangle(Vertex(glm::vec3(+0.5f * size.x, -0.5f * size.y, +0.5f * size.z), glm::vec3( 0.0f, -1.0f,  0.0f), glm::vec2( 1.0f * textureRepeat.x, 1.0f * textureRepeat.z)),
				 Vertex(glm::vec3(-0.5f * size.x, -0.5f * size.y, +0.5f * size.z), glm::vec3( 0.0f, -1.0f,  0.0f), glm::vec2( 0.0f * textureRepeat.x, 1.0f * textureRepeat.z)),
				 Vertex(glm::vec3(-0.5f * size.x, -0.5f * size.y, -0.5f * size.z), glm::vec3( 0.0f, -1.0f,  0.0f), glm::vec2( 0.0f * textureRepeat.x, 0.0f * textureRepeat.z))),
		//Top																		   		 						  
		Triangle(Vertex(glm::vec3(+0.5f * size.x, +0.5f * size.y, -0.5f * size.z), glm::vec3( 0.0f,  1.0f,  0.0f), glm::vec2( 0.0f * textureRepeat.x, 0.0f * textureRepeat.z)),
				 Vertex(glm::vec3(-0.5f * size.x, +0.5f * size.y, -0.5f * size.z), glm::vec3( 0.0f,  1.0f,  0.0f), glm::vec2( 1.0f * textureRepeat.x, 0.0f * textureRepeat.z)),
				 Vertex(glm::vec3(-0.5f * size.x, +0.5f * size.y, +0.5f * size.z), glm::vec3( 0.0f,  1.0f,  0.0f), glm::vec2( 1.0f * textureRepeat.x, 1.0f * textureRepeat.z))),
		Triangle(Vertex(glm::vec3(-0.5f * size.x, +0.5f * size.y, +0.5f * size.z), glm::vec3( 0.0f,  1.0f,  0.0f), glm::vec2( 1.0f * textureRepeat.x, 1.0f * textureRepeat.z)),
				 Vertex(glm::vec3(+0.5f * size.x, +0.5f * size.y, +0.5f * size.z), glm::vec3( 0.0f,  1.0f,  0.0f), glm::vec2( 0.0f * textureRepeat.x, 1.0f * textureRepeat.z)),
				 Vertex(glm::vec3(+0.5f * size.x, +0.5f * size.y, -0.5f * size.z), glm::vec3( 0.0f,  1.0f,  0.0f), glm::vec2( 0.0f * textureRepeat.x, 0.0f * textureRepeat.z)))
	};

	return tris;
}
