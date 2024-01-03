#include "skybox.h"

Skybox::Skybox() {}

Skybox::~Skybox() {}

void Skybox::Init(float size){
	int count = 0;
	VertexBuffer::VertexData* vd = new VertexBuffer::VertexData[24];

	vd[count++] = VertexBuffer::VertexData(-size, -size, size, 1.f, 1.f, 1.f, .25f, .5f); 
	vd[count++] = VertexBuffer::VertexData(-size, size, size, 1.f, 1.f, 1.f, .25f, .75f); 
	vd[count++] = VertexBuffer::VertexData(size, size, size, 1.f, 1.f, 1.f, 0.f, .75f);   
	vd[count++] = VertexBuffer::VertexData(size, -size, size, 1.f, 1.f, 1.f, 0.f, .5f);   

	vd[count++] = VertexBuffer::VertexData(-size, -size, -size, 1.f, 1.f, 1.f, .5f, .5f); 
	vd[count++] = VertexBuffer::VertexData(-size, size, -size, 1.f, 1.f, 1.f, .5f, .75f); 
	vd[count++] = VertexBuffer::VertexData(-size, size, size, 1.f, 1.f, 1.f, .25f, .75f); 
	vd[count++] = VertexBuffer::VertexData(-size, -size, size, 1.f, 1.f, 1.f, .25f, .5f); 

	vd[count++] = VertexBuffer::VertexData(size, -size, -size, 1.f, 1.f, 1.f, .75f, .5f); 
	vd[count++] = VertexBuffer::VertexData(size, size, -size, 1.f, 1.f, 1.f, .75f, .75f); 
	vd[count++] = VertexBuffer::VertexData(-size, size, -size, 1.f, 1.f, 1.f, .5f, .75f); 
	vd[count++] = VertexBuffer::VertexData(-size, -size, -size, 1.f, 1.f, 1.f, .5f, .5f); 

	vd[count++] = VertexBuffer::VertexData(size, -size, size, 1.f, 1.f, 1.f, 1.f, .5f);   
	vd[count++] = VertexBuffer::VertexData(size, size, size, 1.f, 1.f, 1.f, 1.f, .75f);   
	vd[count++] = VertexBuffer::VertexData(size, size, -size, 1.f, 1.f, 1.f, .75f, .75f); 
	vd[count++] = VertexBuffer::VertexData(size, -size, -size, 1.f, 1.f, 1.f, .75f, .5f); 

	vd[count++] = VertexBuffer::VertexData(size, size, size, 1.f, 1.f, 1.f, .25f, 1.f);  
	vd[count++] = VertexBuffer::VertexData(-size, size, size, 1.f, 1.f, 1.f, .25f, .75f);
	vd[count++] = VertexBuffer::VertexData(-size, size, -size, 1.f, 1.f, 1.f, .5f, .75f);
	vd[count++] = VertexBuffer::VertexData(size, size, -size, 1.f, 1.f, 1.f, .5f, 1.f);  

	vd[count++] = VertexBuffer::VertexData(-size, -size, size, 1.f, 1.f, 1.f, .5f, .25f);
	vd[count++] = VertexBuffer::VertexData(size, -size, size, 1.f, 1.f, 1.f, .75f, .25f);
	vd[count++] = VertexBuffer::VertexData(size, -size, -size, 1.f, 1.f, 1.f, .75f, .5f);
	vd[count++] = VertexBuffer::VertexData(-size, -size, -size, 1.f, 1.f, 1.f, .5f, .5f);

	m_vertexBuffer.SetMeshData(vd, count);
	delete[] vd;
}

void Skybox::Render(Transformation tran) const {
	glDisable(GL_DEPTH_TEST);
	glDisable(GL_BLEND);
	glLoadMatrixf(tran.GetMatrix().GetInternalValues());
	m_texture.Bind();
	m_shader.Use();
	m_vertexBuffer.Render(); 
	m_shader.Disable();
	glEnable(GL_DEPTH_TEST);
	glEnable(GL_BLEND);
}

Texture& Skybox::GetTexture() { return m_texture; }

Shader& Skybox::GetShader() { return m_shader; }


