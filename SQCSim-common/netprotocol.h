#ifndef NETPROTOCOL_H__
#define NETPROTOCOL_H__

#include <string>
#include "define.h"
#include "vector3.h"

/* Protocole Particulier de Partie a Plusieurs Personnes (PPPPP) */

namespace netprot {
	enum class PACKET_TYPE: uint8_t {
				ERR, INPUT, OUTPUT, SYNC,
				TEAMINF, SELFINF, PLAYINF, LOGINF,
				CHUNKMOD, PLAYERMOD, PICKUPMOD,
				GAMEINFO, ENDINFO , BULLET,
				CHAT, ERRLOG, LAST_PACK
			};

	/* Structures */
	
	struct Buffer { // Pour pouvoir rendre l'utilisation des buffers plus clean.
		char *ptr = new char[BUFFER_LENGTH] { 1 }, *tmp = nullptr;
		uint32_t len = BUFFER_LENGTH;

		~Buffer() { delete[] ptr; }
		void rstLen() { len = BUFFER_LENGTH; }
	};

	struct Packet { // Pour pouvoir recevoir les paquets du recv() sans avoir à les aiguiller dans la même thread.
		void *ptr = nullptr; // Notez que le pointeur doit être supprimé séparément lorsqu'il n'est plus utile.
		PACKET_TYPE type = PACKET_TYPE::ERR;
	};

	/* Constantes */

	inline const char Footer[sizeof(uint32_t)] = { '\0', '\r', '\0', '\n' }; // constante de footer qui est ajoutée à chaque paquet envoyé.

	/* Sous-structures */

	struct Keys {
		bool forward = false,
			backward = false,
			left = false,
			right = false,
			jump = false,
			shoot = false,
			block = false;
	};

	struct States {
		bool jumping = false,
			shooting = false,
			hit = false,
			powerup = false,
			dead = false,
			still = false,
			jumpshot = false,
			running = false;
	};

	struct Boosts {
		bool invincible = false,
			damage = false,
			hp = false;
	};

	/* Structures de paquets */

	struct Input {										// cli -> srv			UDP			~frame
		Timestamp timestamp;
		uint64_t sid = 0;
		Keys keys; // 0bFBLRJS__ bit-packing de bool.
		Vector3f direction;
	};

	struct Output {										// srv -> cli			UDP			~frame
		Timestamp timestamp;
		uint64_t id = 0;
		Vector3f position,
			     direction;
		States states; // 0bJSH_____ bit-packing de bool.
	};

	struct Sync {										// srv -> cli			TCP			~second				- un premier sync démarre la partie.
		Timestamp timestamp;
		uint64_t sid = 0;
		uint32_t timer = 0;
		uint16_t ammo = 0;
		Boosts boost;
		float hp = 0;
		Vector3f position;
		Sync() {}
		Sync(Sync* sync) : timestamp(sync->timestamp), sid(sync->sid), timer(sync->timer), ammo(sync->ammo), hp(sync->hp), position(sync->position), boost(sync->boost) {}
	};

	struct TeamInfo {									// cli <-> srv			TCP			once
		char *name = new char[32];
		uint64_t id = 0;
		TeamInfo() {}
		TeamInfo(TeamInfo* tem) : id(tem->id) { strcpy(name, 32, tem->name); }
		~TeamInfo() { delete[] name; }
	};

	struct LoginInfo {									// cli <-> srv			TCP			once
		char *name = new char[32];
		uint64_t sid = 0, 
			     tid = 0;
		LoginInfo() {}
		LoginInfo(LoginInfo* log): sid(log->sid), tid(log->tid) { strcpy(name, 32, log->name); }
		~LoginInfo() { delete[] name; }
	};

	struct PlayerInfo {									// cli <-> srv			TCP			once
		char *name = new char[32];
		uint64_t id = 0,
				 tid = 0;
		PlayerInfo() {}
		PlayerInfo(PlayerInfo* ply) : id(ply->id), tid(ply->tid) { strcpy(name, 32, ply->name); };
		PlayerInfo(int id, int tid, std::string strname) : id(id), tid(tid) { strcpy(name, 32, strname.c_str()); }
		~PlayerInfo() { delete[] name; }
	};

	struct GameInfo {									// cli <-> srv			TCP			event (before game start)/ once
		uint64_t seed;
		uint32_t countdown;
		uint8_t gameType; // TODD: enum.
		GameInfo() {}
		GameInfo(GameInfo* gam) : seed(gam->seed), countdown(gam->countdown), gameType(gam->gameType) {}
	};

	struct PickupMod {
		uint64_t id;
		Vector3f pos;
		Boosts boost;
		bool available = true;
		PickupMod() {}
		PickupMod(PickupMod* pmod) : id(pmod->id), pos(pmod->pos), boost(pmod->boost), available(pmod->available) {}
	};

	struct Chat {										// cli <-> srv			TCP			event
		uint64_t src_id = 0,
			dest_id = 0,
			dest_team_id = 0;
		char *mess = new char[140]; // Good 'nough for twitr, good 'nough for me.
		Chat() {}
		Chat(Chat* cha) : src_id(cha->src_id), dest_id(cha->dest_id), dest_team_id(cha->dest_team_id) { strcpy(mess, 140, cha->mess); }
		~Chat() { delete[] mess; }
	};

	struct ChunkMod {
		Vector3f pos;
		BlockType b_type, old_b_type;
	};

	struct BulletAdd {
		Timestamp tstamp;
		Vector3f pos, dir;
		uint64_t id;
	};

	struct ErrorLog {									// srv -> cli			TCP			event
		char *mess = new char[140];
		bool is_fatal = false;
		ErrorLog() {};
		ErrorLog(ErrorLog* err) : is_fatal(err->is_fatal) { strcpy(mess, 140, err->mess); }
		~ErrorLog() { delete[] mess; }
	};

	/* Fonctions */

	void Serialize(Input* in, char* buf[], uint32_t* buflen); // cli
	void Serialize(Output* out, char* buf[], uint32_t* buflen); // srv
	void Serialize(Sync* sync, char* buf[], uint32_t* buflen); // srv
	void Serialize(TeamInfo* tinfo, char* buf[], uint32_t* buflen); // cli/srv
	void Serialize(LoginInfo* linfo, char* buf[], uint32_t* buflen); // cli/srv
	void Serialize(PlayerInfo* pinfo, char* buf[], uint32_t* buflen); // srv
	void Serialize(GameInfo* ginfo, char* buf[], uint32_t* buflen); // cli/srv
	void Serialize(Chat* chat, char* buf[], uint32_t* buflen); // cli/srv
	void Serialize(ChunkMod* chmod, char* buf[], uint32_t* buflen); // srv
	void Serialize(PickupMod* chmod, char* buf[], uint32_t* buflen); // srv
	void Serialize(BulletAdd* bull, char* buf[], uint32_t* buflen); // srv
	void Serialize(ErrorLog* errlog, char* buf[], uint32_t* buflen); // srv

	bool Deserialize(Input* in, char* buf, uint32_t* buflen); // srv
	bool Deserialize(Output* out, char* buf, uint32_t* buflen); // cli
	bool Deserialize(Sync* sync, char* buf, uint32_t* buflen); // cli
	bool Deserialize(TeamInfo* tinfo, char* buf, uint32_t* buflen); // cli/srv
	bool Deserialize(LoginInfo* linfo, char* buf, uint32_t* buflen); // cli/srv
	bool Deserialize(PlayerInfo* pinfo, char* buf, uint32_t* buflen); // cli
	bool Deserialize(GameInfo* ginfo, char* buf, uint32_t* buflen); // cli
	bool Deserialize(Chat* chat, char* buf, uint32_t* buflen); // srv/cli
	bool Deserialize(ChunkMod* chmod, char* buf, uint32_t* buflen); // cli
	bool Deserialize(PickupMod* chmod, char* buf, uint32_t* buflen); // cli
	bool Deserialize(BulletAdd* bull, char* buf, uint32_t* buflen); // cli
	bool Deserialize(ErrorLog* errlog, char* buf, uint32_t* buflen); // srv

	PACKET_TYPE getType(char* buf, uint32_t buflen);

	Packet getPack(char* buf, uint32_t *buflen);
	Packet getPack(Buffer* buf);

	bool emptyPack(Packet pck);

	Packet makePack(void* ptr, PACKET_TYPE type); // Pour pouvoir faire une liste de stock a supprimer sans avoir a en faire une pour chaque type.
	
	template <class T> T copyPack(Packet* pck);

	template <class T> void sendPack(SOCKET sock, T* pack, char** buf, uint32_t* buflen);
	template <class T> void sendPackTo(SOCKET sock, T* pack, char** buf, uint32_t* buflen, sockaddr_in* sockad);

	template <class T> void sendPack(SOCKET sock, T* pack, Buffer* buf);
	template <class T> void sendPackTo(SOCKET sock, T* pack, Buffer* buf, sockaddr_in* sockad);
	
	void recvPacks(SOCKET sock, Buffer* buf, std::vector<char*>* lsPck);
	void recvPacksFrom(SOCKET sock, Buffer* buf, sockaddr_in from, std::vector<char*>* lsPck);

	/* Templates */

	template <class T>
	T copyPack(Packet* pck) { return T((T*)pck->ptr); }

	template <class T>
	void sendPack(SOCKET sock, T* pack, char** buf, uint32_t* buflen) {
		netprot::Serialize(pack, buf, buflen);
		memcpy(*buf + *buflen, Footer, sizeof(uint32_t));
		*buflen += sizeof(Footer);
		send(sock, *buf, *buflen, 0);
		*buflen = BUFFER_LENGTH;
	}

	template <class T>
	void sendPackTo(SOCKET sock, T* pack, char** buf, uint32_t* buflen, sockaddr_in* sockad) {
		const sockaddr_in addr = *sockad;
		netprot::Serialize(pack, buf, buflen);
		memcpy(*buf + *buflen, Footer, sizeof(uint32_t));
		*buflen += sizeof(Footer);
		sendto(sock, *buf, *buflen, 0, (sockaddr*)&addr, sizeof(addr));
		*buflen = BUFFER_LENGTH;
	}

	template <class T>
	void sendPack(SOCKET sock, T* pack, Buffer* buf) {
		netprot::Serialize(pack, &buf->ptr, &buf->len);
		memcpy(&buf->ptr[buf->len], Footer, sizeof(uint32_t));
		buf->len += sizeof(Footer);
		send(sock, buf->ptr, buf->len, 0);
		buf->rstLen();
	}

	template <class T>
	void sendPackTo(SOCKET sock, T* pack, Buffer* buf, sockaddr_in* sockad) {
		const sockaddr_in addr = *sockad;
		netprot::Serialize(pack, &buf->ptr, &buf->len);
		memcpy(&buf->ptr[buf->len], Footer, sizeof(uint32_t));
		buf->len += sizeof(Footer);
		sendto(sock, buf->ptr, buf->len, 0, (sockaddr*)&addr, sizeof(addr));
		buf->rstLen();
	}

};
#endif
