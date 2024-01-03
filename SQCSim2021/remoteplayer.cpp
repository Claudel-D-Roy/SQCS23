#include "remoteplayer.h"
#include <iostream>
#include <cstring>

#include <thread>
#include <queue>




RemotePlayer::RemotePlayer(netprot::PlayerInfo* pinfo) : m_pinfo(*pinfo), m_aminacc(0.0f), m_animstate(Anim::STILL), m_team_id(0), current(), previous(), m_texture_front(), Player(Vector3f(0, 0, 0)) {

}

RemotePlayer::RemotePlayer(netprot::PlayerInfo* pinfo, const Vector3f& pos) : m_pinfo(*pinfo), m_aminacc(0.0f), m_animstate(Anim::STILL), m_team_id(0), current(), previous(), m_texture_front(), Player(pos) {

}


RemotePlayer::~RemotePlayer()
{

}

void RemotePlayer::Init()
{

}

void RemotePlayer::Feed(const netprot::Output out) {

	m_position = Vector3f(out.position);
	m_direction = Vector3f(out.direction);
	current.states = out.states;
	

}




void RemotePlayer::Render(TextureAtlas& atlas, Shader& shader, Transformation tran, float elapsedTime, Player& camera)
{

	float width = 1.f;
	float height = 1.7f;

	Vector3f DiffCam = GetPosition() - camera.GetPosition();
	Vector3f UpCam = Vector3f(0.f, 1.f, 0.f);

	Vector3f CrossA = DiffCam.Cross(UpCam);
	Vector3f CrossB = DiffCam.Cross(CrossA);

	CrossA.Normalize();
	CrossB.Normalize();

	Vector3f playerPosition = GetPosition() + Vector3f(0.f, -.75f, 0.f);

	Vector3f v2 = (playerPosition + CrossA * 0.5 * width + CrossB * 0.5 * height);
	Vector3f v1 = (playerPosition - CrossA * 0.5 * width + CrossB * 0.5 * height);
	Vector3f v3 = (playerPosition + CrossA * 0.5 * width - CrossB * 0.5 * height);
	Vector3f v4 = (playerPosition - CrossA * 0.5 * width - CrossB * 0.5 * height);

	Vector3f angleRemote = GetDirection();
	Vector3f angleCam = (v1 - v2).Cross(v3 - v2);

	angleCam.y = 0;
	angleRemote.y = 0;
	angleCam.Normalize();
	angleRemote.Normalize();

	float angle = angleRemote.Dot(angleCam);
	int index = 0;
	angle = -angle;
	Vector3f side = angleRemote.Cross(angleCam);
	side = -side;

	static float time = 0.f;
	static bool Shooting = false;
	bool isLeft = side.y > 0;

	time += elapsedTime;
	if (time >= 0.05)
	{
		time -= 0.05;

		Shooting = !Shooting;
	}
	
	//std::cout << "shooting : " << current.states.shooting << " jumping : " << current.states.jumping <<  " jumpshot : " << current.states.jumpshot << " running : " << current.states.running << " still : " << current.states.still << " dead : " << current.states.dead << " hit : " << current.states.hit << std::endl;

	if (angle >= 0.75) //Face - side positif
	{
		if (current.states.shooting) {
			if (Shooting)
				index = 17;
			else
				index = 9;
		}
		else if (current.states.jumpshot) {
			if (Shooting)
				index = 41;
			else
				index = 33;
		}
		else if (current.states.jumping)
			index = 25;
		else if (!current.states.jumping && !current.states.shooting && !current.states.jumpshot)
			index = 0;

	}
	else if (angle >= 0.25 && isLeft) //Frontleft
	{
		if (current.states.shooting) {
			if (Shooting)
				index = 18;
			else
				index = 10;

		}
		else if (current.states.jumpshot) {
			if (Shooting)
				index = 42;
			else
				index = 34;
		}
		else if (current.states.jumping)
			index = 26;
		else if (!current.states.jumping && !current.states.shooting && !current.states.jumpshot)
			index = 1;


	}
	else if (angle >= -0.25 && isLeft) //ProfileLeft
	{
		if (current.states.shooting) {
			if (Shooting)
				index = 20;
			else
				index = 12;

		}
		else if (current.states.jumpshot) {
			if (Shooting)
				index = 44;
			else
				index = 36;
		}
		else if (current.states.jumping)
			index = 28;
		else if (!current.states.jumping && !current.states.shooting && !current.states.jumpshot)
			index = 3;


	}
	else if (angle >= -0.75 && isLeft) //BackLeft
	{
		if (current.states.shooting) {
			if (Shooting)
				index = 22;
			else
				index = 14;

		}
		else if (current.states.jumpshot) {
			if (Shooting)
				index = 46;
			else
				index = 38;
		}
		else if (current.states.jumping)
			index = 30;
		else if (!current.states.jumping && !current.states.shooting && !current.states.jumpshot)
			index = 5;


	}
	else if (angle < -0.75) //Dos - side négatif
	{
		if (current.states.shooting) {
			if (Shooting)
				index = 24;
			else
				index = 16;

		}
		else if (current.states.jumpshot) {
			if (Shooting)
				index = 48;
			else
				index = 40;
		}
		else if (current.states.jumping)
			index = 32;
		else if (!current.states.jumping && !current.states.shooting && !current.states.jumpshot)
			index = 7;


	}
	else if (angle >= 0.25 && !isLeft) //FrontRight //REVOIR L'ANIME DE SHOOTING EST PAS DRETTE
	{
		if (current.states.shooting) {
			if (Shooting)
				index = 19;
			else
				index = 11;
		}
		else if (current.states.jumpshot) {
			if (Shooting)
				index = 43;
			else
				index = 35;
		}
		else if (current.states.jumping)
			index = 27;
		else if (!current.states.jumping && !current.states.shooting && !current.states.jumpshot)
			index = 2;


	}
	else if (angle >= -0.25 && !isLeft) //ProfileRight
	{
		if (current.states.shooting) {
			if (Shooting)
				index = 21;
			else
				index = 13;
		}
		else if (current.states.jumpshot) {
			if (Shooting)
				index = 45;
			else
				index = 37;
		}
		else if (current.states.jumping)
			index = 29;
		else if (!current.states.jumping && !current.states.shooting && !current.states.jumpshot)
			index = 4;


	}
	else if (angle >= -0.75 && !isLeft) //BackRight
	{
		if (current.states.shooting) {
			if (Shooting)
				index = 23;
			else
				index = 15;

		}
		else if (current.states.jumpshot) {
			if (Shooting)
				index = 47;
			else
				index = 39;
		}
		else if (current.states.jumping)
			index = 31;
		else if (!current.states.jumping && !current.states.shooting && !current.states.jumpshot)
			index = 6;


	}



	float u, v, w, h;

	shader.Use();
	atlas.Bind();
	atlas.TextureIndexToCoord(index, u, v, w, h);

	glEnable(GL_BLEND);


	if (current.states.hit)
	{
		glBlendFunc(GL_CONSTANT_COLOR, GL_ONE_MINUS_CONSTANT_COLOR);
		glBlendColor(1.f, 0.f, 0.f, 1.f);

	}
	else {
		glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	}

	glBlendEquation(GL_FUNC_ADD);




	glLoadMatrixf(tran.GetMatrix().GetInternalValues());
	glDepthFunc(GL_LEQUAL);
	glBegin(GL_QUADS);
	glTexCoord2f(u, v); glVertex3f(v1.x, v1.y, v1.z);
	glTexCoord2f(u + w, v); glVertex3f(v2.x, v2.y, v2.z);
	glTexCoord2f(u + w, v + h); glVertex3f(v3.x, v3.y, v3.z);
	glTexCoord2f(u, v + h); glVertex3f(v4.x, v4.y, v4.z);

	glEnd();

	glBlendFunc(GL_CONSTANT_COLOR, GL_ONE_MINUS_CONSTANT_COLOR);
	glBlendEquation(GL_FUNC_SUBTRACT);
	glDisable(GL_BLEND);

	shader.Disable();

}

bool RemotePlayer::LoadTexture(Texture& texture, const std::string& filename, bool useMipmaps, bool stopOnError)
{
	texture.Load(filename, useMipmaps);
	if (!texture.IsValid()) {
		std::cerr << "Unable to load texture (" << filename << ")" << std::endl;
		if (stopOnError)
			return false;

		return false;
	}

	return true;
}
