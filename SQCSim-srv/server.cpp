#include "server.h"

Server::Server(LOG_DEST log) {
	m_log = log;
	if (log == LOG_DEST::LOGFILE) {
		m_logfile = std::ofstream("server.log", std::ofstream::out);
		if (!m_logfile.is_open()) {
			m_log = LOG_DEST::CONSOLE; // Fallback console.
			Log("Ouverture fichier log: repli vers console.", true, false);
		}
	}
}

Server::~Server() {
	if (m_logfile.is_open())
		m_logfile.close();
	if (m_sock_udp)
		closesocket(m_sock_udp);
	if (m_sock_tcp)
		closesocket(m_sock_tcp);
	for (const auto& [key, player] : m_conns)
		closesocket(player->getSock());
	m_conns.clear();
	delete m_world;
#ifdef _WIN32
	WSACleanup();
#endif
}

int Server::Init() {
	Log("Initialisation du serveur...", false, false);

#ifdef _WIN32
	if (WSAStartup(MAKEWORD(2, 2), &m_wsaData) != 0) { /* Initialisation de l'environnement reseau (Windows only) */
		Log("Initialisation WinSock.", true, true);
		return 1;
	}
#endif

	m_sock_udp = socket(AF_INET, SOCK_DGRAM, 0);
	if (m_sock_udp == INVALID_SOCKET) { /* Creation du socket UDP */
		Log("Creation Socket UDP.", true, true);
		return 2;
	}

	m_sock_tcp = socket(AF_INET, SOCK_STREAM, 0);
	if (m_sock_tcp == INVALID_SOCKET) { /* Creation du socket TCP */
		Log("Creation Socket TCP.", true, true);
		return 3;
	}

	/* Creation structure donnes descripteur du socket serveur */
	sockaddr_in addr;
	addr.sin_family = AF_INET;
	addr.sin_port = htons(SRV_PORT);
	addr.sin_addr.s_addr = htonl(INADDR_ANY);

	if (bind(m_sock_udp, (sockaddr*)&addr, sizeof(addr)) != 0) { /* Associer le socket UDP au port */
		Log("Association Socket UDP.", true, true);
		return 4;
	}

	if (bind(m_sock_tcp, (sockaddr*)&addr, sizeof(addr)) != 0) { /* Associer le socket TCP au port */
		Log("Association Socket TCP.", true, true);
		return 5;
	}

	return 0;
}

int Server::Ready() {
	int nbrjoueurs = 0,
		nbrconn = 0;
	bool readystart = false;
	do {
		Log("Entrez la duree de la partie: ", false, false);
		std::cin.getline(m_buf.ptr, BUFFER_LENGTH);
		try {
			m_game.countdown = std::stoi(m_buf.ptr);
		}
		catch (const std::exception& e) {
			Log(e.what(), true, false);
			m_game.countdown = 0;
		}
	} while (m_game.countdown < 1);
	// m_game.seed = 9370707;
	do {
		Log("Entrez le seed de la partie: ", false, false);
		std::cin.getline(m_buf.ptr, BUFFER_LENGTH);
		try {
			m_game.seed = std::stoi(m_buf.ptr);
		}
		catch (const std::exception& e) {
			Log(e.what(), true, false);
			m_game.seed = 0;
		}
	} while (m_game.seed < 1);
	do {
		Log("Entrez le nombre de joueurs: ", false, false);
		std::cin.getline(m_buf.ptr, BUFFER_LENGTH);
		try {
			nbrjoueurs = std::stoi(m_buf.ptr);
		}
		catch (const std::exception& e) {
			Log(e.what(), true, false);
			nbrjoueurs = 0;
		}
		if (nbrjoueurs <= 0 || nbrjoueurs > MAX_CONNECTIONS)
			Log("Nombre de joueurs invalide.", true, false);
	} while (nbrjoueurs <= 0 || nbrjoueurs > MAX_CONNECTIONS);
	do {
		Log("Entrez le nombre de boosters: ", false, false);
		std::cin.getline(m_buf.ptr, BUFFER_LENGTH);
		try {
			m_boostcount = std::stoi(m_buf.ptr);
		}
		catch (const std::exception& e) {
			Log(e.what(), true, false);
			m_boostcount = -1;
		}
	} while (m_boostcount < 0);
	m_game.gameType = 1;

	if (listen(m_sock_tcp, MAX_CONNECTIONS) < 0) {
		Log("Ecoute sur le port TCP.", true, true);
		return 1;
	}

	buildIdList(ID_LIST_SIZE);

	Log("A l'ecoute sur le port: " + std::to_string(SRV_PORT), false, false);

	while (!readystart) {
		sockaddr_in sockad;
		addrlen_t addrlen = sizeof(sockad);
		SOCKET sock = accept(m_sock_tcp, (sockaddr*)&sockad, &addrlen);

		if (sock < 0)
			Log("Erreur de connexion", true, false);
		else if (sock > 0) {
			std::string str = "Nouvelle connexion provenant de: ";
			str.append(inet_ntop(AF_INET, &sockad.sin_addr, m_buf.ptr, m_buf.len)).append(": ").append(std::to_string(sockad.sin_port));

			if (recv(sock, m_buf.ptr, m_buf.len, 0) > 0) {
				PlayerInfo* play = new PlayerInfo();

				m_buf.len = BUFFER_LENGTH;
				Packet pck = getPack(&m_buf);
				if (pck.type != PACKET_TYPE::LOGINF) {
					Log("Paquet invalide.", true, false);
					if (pck.type != PACKET_TYPE::ERR)
						netprot::emptyPack(pck);
					continue; // Passer au prochain appel si c'est pas un LoginInfo ou un LoginInfo invalide qui rentre.
				}
				LoginInfo* log = (LoginInfo*)pck.ptr;

				log->sid = getUniqueId();
				log->tid = 1145389380; // TODO: À changer si on implemente un mode en equipe.

				Log(str.append(" Nom: ").append(log->name), false, false);
				str.clear();


				sendPackTo<LoginInfo>(m_sock_udp, log, &m_buf, &sockad);

				play->id = getUniqueId();
				play->tid = log->tid;
				strcpy(play->name, 32, log->name);

				Log(str.append(play->name).append(" SID: [").append(std::to_string(log->sid)).append("]")
					.append(" ID: [").append(std::to_string(play->id)).append("]")
					.append(" TID: [").append(std::to_string(play->tid)).append("]"), false, false);
				play->tid = log->tid;

				sendPackTo<GameInfo>(m_sock_udp, &m_game, &m_buf, &sockad);
				Connection* conn = new Connection(sock, sockad, log, play);

				m_conns[log->sid] = conn;

				if (++nbrconn >= nbrjoueurs)
					readystart = true;
			}
		}
	}
	for (auto& [keyin, playin] : m_conns) // Not pretty, but it works.
		for (auto& [keyout, playout] : m_conns) {
			if (keyin == keyout)
				continue;
			sendPackTo<PlayerInfo>(m_sock_udp, playout->getInfo(), &m_buf, playin->getAddr()); // et envoyer les infos des joueurs distants au nouveau joueur.
		}

	return 0;
}

void Server::Run() {
	bool endgame = false;
	Input in;
	sockaddr_in sockad;
	addrlen_t socklen = sizeof(sockad);

	Log("Debut de la partie...", false, false);

	int players = m_conns.size();

	m_world = new World();
	m_world->SetSeed(m_game.seed);
	m_world->GetChunks().Reset(nullptr);
	m_world->BuildWorld();

	for (auto& [key, conn] : m_conns) { // Creation des instances de joueurs et premier sync.
		if (!conn) {
			m_conns.erase(key);
			continue;
		}
		int x = (rand() % (CHUNK_SIZE_X * WORLD_SIZE_X - 1) - (CHUNK_SIZE_X * WORLD_SIZE_X / 2)) / 4,
			y = (rand() % (CHUNK_SIZE_Z * WORLD_SIZE_Y - 1) - (CHUNK_SIZE_Z * WORLD_SIZE_Y / 2)) / 4;
		conn->player = new Player(Vector3f(x + .5f, CHUNK_SIZE_Y + 1.8f, y + .5f));
		conn->player->m_username = conn->GetName();
		m_players[key] = conn->player;
		Sync sync;
		sync.position = conn->player->GetPositionAbs();
		sync.hp = conn->player->GetHP();
		sync.sid = key;
		sync.ammo = 0;
		sync.timestamp = 0;
		sync.timer = m_game.countdown;
		sendPackTo<Sync>(m_sock_udp, &sync, &m_buf, conn->getAddr());
	}
	
	int timer = m_game.countdown, timer_acc = 0, deadplayers = 0;
	std::chrono::high_resolution_clock::time_point start = std::chrono::high_resolution_clock::now();
	Timestamp last = 0;
	std::vector<Chat*> chatlog;
	std::vector<ChunkMod*> chunkdiffs;
	std::vector<Bullet*> bullets;
	std::vector<std::vector<Bullet*>::iterator> bullit;
	std::vector<BulletAdd*> netbull;
	std::vector<PickupMod*> netboosters;
	std::vector<char*> lsPck;

	int max = 0;
	for (int64_t x = 0; x < m_boostcount; ++x) {
		Vector3f pos = Vector3f(rand() % (WORLD_SIZE_X * CHUNK_SIZE_X) + .5f, rand() % CHUNK_SIZE_Y + .5f, rand() % (WORLD_SIZE_Y * CHUNK_SIZE_Z) + .5f);
		if (m_world->BlockAt(pos) == BTYPE_AIR && m_world->BlockAt(Vector3f(pos.x, pos.y - 2, pos.z)) != BTYPE_AIR) {
			Booster* boost = new Booster(pos, (BOOST_TYPE)(rand() % BTYPE_BOOST_LAST), getUniqueId());
			m_boosters[boost->GetId()] = boost;
		}
		else --x;
		if (++max > 100000)
			break;
	}

	Chat* startchat = new Chat();
	startchat->src_id = 0;
	char startmess[] = "How would -YOU- like to die today, motherf-words?";
	float endtime = 0.;
	strcpy(startchat->mess, 140, startmess);

	chatlog.emplace_back(startchat);
		
	while (!endgame && endtime < 1.) {
		using namespace std::chrono;
		Timestamp tstamp = duration_cast<milliseconds>(high_resolution_clock::now() - start).count();

		if (last == 0)
			last = tstamp;
		timer_acc += tstamp - last;
		if (timer_acc >= 1000) {
			while (timer_acc >= 1000)
				timer_acc -= 1000;
			if (!endgame)
				--timer;
			std::string str = "Timer: ";
			Log(str.append(std::to_string(timer)), false, false);
		}
		last = tstamp;


		for (auto& [key, conn] : m_conns) {				

			/* In */

			Input in; Sync sync;

			recvPacks(m_sock_udp, &m_buf, &lsPck);
			for (auto& pck : lsPck) {
				uint32_t bsize = m_buf.len - (pck - m_buf.ptr);
				switch (netprot::getType(pck, 1)) {
					using enum netprot::PACKET_TYPE;
				case INPUT:
					if (Deserialize(&in, pck, &bsize)) {
						if (m_conns.count(in.sid))
							m_conns[in.sid]->AddInput(in);
					}
					break;
				case SYNC:
					if (Deserialize(&sync, pck, &bsize)) {}
					break;
				default: break;
				}
			}
			if (!lsPck.empty())
				lsPck.clear();

			/* Process */

			if (conn->m_nsync) {
				Timestamp tstamp = conn->Run(m_world, m_boosters);

				if (conn->player->AmIDead() && !conn->player->Eulogy) {
					Chat* chat = new Chat();
					chat->dest_id = chat->dest_team_id = chat->src_id = 0;

					Player* murderer = m_conns.at(conn->player->Killer)->player;

					if (murderer != conn->player)
						murderer->addPoint();

					std::string killer = murderer->GetUsername();
					
					std::string mess = getDeathMessage(conn->player->GetUsername(), killer);
					
					strcpy(chat->mess, 140, mess.c_str());
					chatlog.emplace_back(chat);
					++deadplayers;

					conn->player->Eulogy = true;
					conn->m_nsync = false;
				}
				else {
					for (auto& chmo : conn->ChunkDiffs)
						chunkdiffs.emplace_back(std::move(chmo));
					if (!conn->ChunkDiffs.empty())
						conn->ChunkDiffs.clear();

					for (auto& bull : conn->Bullets) {
						bullets.emplace_back(bull);
						BulletAdd* nbul = new BulletAdd();
						nbul->pos = bull->getPos();
						nbul->dir = bull->getVel();
						nbul->id = key;
						nbul->tstamp = tstamp;

						netbull.emplace_back(std::move(nbul));
					}
					if (!conn->Bullets.empty())
						conn->Bullets.clear();
				}
			}
			/* Out */
			conn->sendPacks(m_sock_udp, m_conns, timer);

			if ((deadplayers == players - 1 && deadplayers != 0) || timer < 0) {
				if (!endgame) {
					Chat* gameover = new Chat();
					gameover->dest_id = gameover->dest_team_id = gameover->src_id = 0;
					std::string winner, winmess;
					int score = 0; 
					bool plural = false;
					for (auto& [key, conn] : m_conns) {
						if (conn->player->getScore() > score) {
							winner = conn->player->GetUsername();
							score = conn->player->getScore();
							plural = false;
						}
						else if (conn->player->getScore() == score) {
							winner = winner.append(" and ").append(conn->player->GetUsername());
							plural = true;
						}
					}

					winmess = "And the winner";
					if (!plural)
						winmess = winmess.append(" is ");
					else winmess = winmess.append("s are ");
				
					winmess = winmess.append(winner).append(" with ").append(std::to_string(score)).append(" point");

					if (score > 1)
						winmess = winmess.append("s.");
					else winmess = winmess.append(".");

					strcpy(gameover->mess, 140, winmess.c_str());
					
					chatlog.emplace_back(gameover);
				}
				endgame = true;
				endtime += .001;
			}
		}

		int max = 0;
		for (auto& [key, booster] : m_boosters) {
			if (booster->modified) {
				PickupMod pmod;
				pmod.available = booster->GetAvailability();
				pmod.id = booster->GetId();
				pmod.pos = booster->GetPosition();

				Boosts boost;
				switch (booster->GetType()) {
				case BTYPE_DAMAGE:
					boost.damage = true;
					break;
				case BTYPE_HEAL:
					boost.hp = true;
					break;
				case BTYPE_INVINCIBLE:
					boost.invincible = true;
					break;
				default: continue;
				}
				pmod.boost = boost;
				booster->modified = false;
				for (auto& [key, conn] : m_conns)
					sendPackTo<PickupMod>(m_sock_udp, &pmod, &m_buf, conn->getAddr());
				max++;
				if (max > 5)
					break;
			}
		}

		for (auto& bull : netbull) {
			for (auto& [key, conn] : m_conns)
				if (bull->id != conn->GetHash(false)) // Pour pas repitcher au joueur sa propre balle.
					sendPackTo<BulletAdd>(m_sock_udp, bull, &m_buf, conn->getAddr());
			delete bull;
		}
		if (!netbull.empty())
			netbull.clear();

		for (auto bull = bullets.begin(); bull != bullets.end(); ++bull) {
			ChunkMod* cmod = nullptr;
			Bullet* bullet = *bull;
			if (bullet->Update(m_world, (1. / 60.), 50, m_players, &cmod)) {
				if (cmod)
					chunkdiffs.emplace_back(cmod);
				bullit.push_back(bull);
			}
		}

		for (auto& bull : bullit) {
			delete* bull;
			bullets.erase(bull);
		}

		if (!bullit.empty())
			bullit.clear();

		for (auto& chat : chatlog) {
			Log(chat->mess, false, false);
			for (auto& [key, conn] : m_conns) 
				sendPackTo<Chat>(m_sock_udp, chat, &m_buf, conn->getAddr());
			delete chat;
		}
		if (!chatlog.empty())
			chatlog.clear();

		for (auto& chmo : chunkdiffs) {
			for (auto& [key, conn] : m_conns)
				sendPackTo<ChunkMod>(m_sock_udp, chmo, &m_buf, conn->getAddr());
			delete chmo;
		}
		if (!chunkdiffs.empty())
			chunkdiffs.clear();
	}

	Chat end;
	end.src_id = 0;
	char endmess[] = "Game over, man. Game over.";
	strcpy(end.mess, 140, endmess);

	for (auto& [key, conn] : m_conns) {
		std::string str = conn->player->GetUsername();
		Log(str.append(" ").append(std::to_string(conn->player->getScore())), false, false);
	}

	for (auto& [key, conn] : m_conns)
		sendPackTo<Chat>(m_sock_udp, &end, &m_buf, conn->getAddr());

	// TODO: Gérer les 2-3 secondes post-game avant le billboard pour pas avoir un whiplash à la fin de la game.

	char* ch = new char[2];
	std::cout << "Nouvelle partie? [o/N] ";
	std::cin.getline(ch, 2);
	std::cout << std::endl;

	m_exit = true;
	if (ch[0] == 'o' || ch[0] == 'O')
		m_exit = false;

	delete[] ch;
}

void Server::Cleanup() {
	for (auto& [key, conn] : m_conns)
		delete conn;
	
	m_conns.clear();
	m_players.clear();
	delete m_world;
	m_world = nullptr;
}

void Server::DeInit() {
	if (m_logfile.is_open())
		m_logfile.close();
	if (m_sock_udp)
		closesocket(m_sock_udp);
	if (m_sock_tcp)
		closesocket(m_sock_tcp);

#ifdef _WIN32
	WSACleanup();
#endif
}

bool Server::NewGameRequested() const { return !m_exit; }

inline std::string Server::LogTimestamp() {
	time_t rawtime;
	tm timeinfo;
	char buffer[50];

	time(&rawtime);

#ifdef _WIN32
	localtime_s(&timeinfo, &rawtime);
#else
	localtime_r(&rawtime, &timeinfo);
#endif

	strftime(buffer, sizeof(buffer), "%d-%m-%Y %H:%M:%S", &timeinfo);
	std::string str(buffer);

	return "[" + str + "] ";
}

void Server::Log(std::string str, bool is_error = false, bool is_fatal = false) {
	switch (m_log) {
		using enum LOG_DEST; // C++20!
	case LOGFILE:
		m_logfile << LogTimestamp() << (is_fatal ? "FATAL " : "") << (is_error ? "ERROR " : "") << str << std::endl;
		break;
	case CONSOLE: [[fallthrough]]; // Pour dire que c'est voulu que ça traverse vers le case en dessous (C++17!)
	default:
		std::cout << LogTimestamp() << (is_fatal ? "FATAL " : "") << (is_error ? "ERROR " : "") << str << std::endl;
		break;
	}

	if (is_fatal) {
		if (m_logfile.is_open())
			m_logfile.close();
		if (m_sock_udp)
			closesocket(m_sock_udp);
		if (m_sock_tcp)
			closesocket(m_sock_tcp);
		for (const auto& [key, player] : m_conns)
			closesocket(player->getSock());
		
		delete m_world;
		m_conns.clear();
#ifdef _WIN32
		WSACleanup();
#endif
		exit(-1);
	}
}

void Server::buildIdList(size_t size) {
	std::set<uint64_t> lst;

	srand(time(NULL));
	do lst.insert(((uint64_t)rand() << 32 | rand()));
	while (lst.size() < size);

	m_ids = std::vector<uint64_t>(lst.begin(), lst.end());
}

uint64_t Server::getUniqueId() {
	uint64_t id = m_ids.back();
	m_ids.pop_back();
	return id;
}

std::string Server::getDeathMessage(std::string username, std::string killer) const {
	std::string mess;
	std::string temp = DEATHMESSAGES.at(rand() % DEATHMESSAGES.size());
	size_t ind = temp.find('@');
	size_t indk = temp.find('$');
	bool bypass = false;

	if (indk == std::string::npos)
		bypass = true;

	if (ind < indk || bypass) {
		mess.append(temp.substr(0, ind));
		mess.append(username);
		if (!bypass) {
			mess.append(temp.substr(ind + 1, indk - 1));
			mess.append(killer);
			mess.append(temp.substr(indk + 1));
		}
		else mess.append(temp.substr(ind + 1));
	}
	else {
		mess.append(temp.substr(0, indk));
		mess.append(killer);
		mess.append(temp.substr(indk + 1, ind - 1));
		mess.append(username);
		mess.append(temp.substr(ind + 1));
	}

	return mess;
}
