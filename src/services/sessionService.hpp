#pragma once

#ifdef __linux__
#include <uuid/uuid.h>
#else
#include <windows.h>
#endif

#include "../components/tdb.hpp"

struct SessionModel
{
    int id = 0;
    std::string token = "";
    int userId = 0;
    std::tm createAt;
    std::tm lastConnect;
};

class SessionsService
{
public:
    // SessionsService(SessionModel session) : _session(session) {};
    SessionsService();
    // ~SessionsService();

    // return new session token, if ip not empty update it for user
    static std::string CreateSession(SessionModel session, std::string ip = "");
    
    // check session is exist, update time of last connect and if ip not empty update it for user
    static bool CheckSession(std::string token, std::string ip = "");
    
    static void DeleteSession(std::string token);
    
    // delete all session with expire lifetime
    static void CheckAllSessionsTime();
};
