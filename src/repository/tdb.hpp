#pragma once

#include <pthread.h>
#include <soci/soci.h>
#include <soci/postgresql/soci-postgresql.h>

#include "../components/log.hpp"

class TDB
{
private:
    std::string dbName;
    std::string user;
    std::string psw;
    
    pthread_mutex_t session_mutex;

public:
    std::unique_ptr<soci::session> sql;

    TDB();
    // ~TDB();

    bool GetSession();
    void FreeSession();
    bool PrepareDb();
};

extern TDB MainDB;