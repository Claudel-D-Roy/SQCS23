#ifndef SRV_DEFINE_H__
#define SRV_DEFINE_H__

#include "../SQCSim-common/define.h"
#include <iostream>
#include <sstream>
#include <cctype>
#include <string>

#define MAX_CONNECTIONS 16
#define ID_LIST_SIZE 127
#define SRV_MANUAL_SETUP true

// @ = Dead guy, $ = Killer.
const std::vector<std::string> DEATHMESSAGES = { "@ has gone to meet their maker.",
			   								     "@ has bit the dust, if you know what I mean.",
												 "@ has ceased to be.",
												 "@ is no more.",
												 "@ is like, super dead.",
												 "Requiescat In Pace, @.",
												 "So long, @, and thanks for all the lols!", 
												 "@ has a bad case of being dead.",
												 "@ has finally seen the light!",
												 "$ sees dead people; in this case they see a dead @.",
												 "Thought @ was hot; guess what? He's not. He is dead, dead, dead.",
												 "@ did not want to live forever.",
												 "$ made @ die for their country.",
												 "$ has become death, destroyer of @.",
												 "$ did not make @ feel lucky." };

#endif
