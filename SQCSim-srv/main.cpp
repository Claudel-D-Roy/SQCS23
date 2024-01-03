#include "server.h"

int main() {
	std::unique_ptr<Server> server = std::make_unique<Server>();
	if (server->Init() == 0)
		while (server->Ready() == 0) {
			server->Run();
			if (!server->NewGameRequested())
				break;
			server->Cleanup();
		}
	server->DeInit();
}