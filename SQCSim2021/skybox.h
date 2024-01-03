#ifndef SKYBOX_H__
#define SKYBOX_H__

#include "../SQCSim-common/transformation.h"
#include "define.h"
#include "vertexbuffer.h"
#include "texture.h"
#include "shader.h"

class Skybox {
private:
	VertexBuffer m_vertexBuffer;
	Texture m_texture;
	Shader m_shader;

public:
	Skybox();
	~Skybox();
	void Init(float size);
	void Render(Transformation trans) const;
	Texture& GetTexture();
	Shader& GetShader();
};

#endif // SKYBOX_H__
