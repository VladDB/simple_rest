#pragma once

#include <pthread.h>
#include <soci/soci.h>
#include <soci/postgresql/soci-postgresql.h>

#include "log.hpp"

// class for connection to DataBase
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
    ~TDB();

    // creaete session and lock mutex
    bool GetSession();
    // unlock mutex
    void FreeSession();
    // prepare DataBase, create tables users and sessions
    bool PrepareDb();
};

// main global instance of class
extern TDB MainDB;