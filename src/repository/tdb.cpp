#include "tdb.hpp"

TDB MainDB;

TDB::TDB()
{
    dbName = "test";
    user = "postgres";
    psw = "masterkey";
    sql = nullptr;
    pthread_mutex_init(&session_mutex, NULL);
}

bool TDB::GetSession()
{
    try
    {
        if (!sql)
            sql = std::make_unique<soci::session>();

        if (!sql->is_connected())
        {
            pthread_mutex_lock(&session_mutex);
            std::string connectString = "postgresql://host='localhost' dbname='" + dbName + "' user='" + user + "' password='" + psw + "'";
            sql->open(connectString);
            if (!sql->is_connected())
                throw std::runtime_error("No connection to DB");
            soci::indicator ind;
            int is_connected = 0;
            *sql << "SELECT 1;", soci::into(is_connected, ind);
            if (is_connected)
                logger->info("Connected to DB \"{}\"", dbName);
        }
    }
    catch (const std::exception &e)
    {
        logger->error("Error in TDB::Connect: {}", e.what());
        pthread_mutex_unlock(&session_mutex);
        return false;
    }
    return true;
}

void TDB::FreeSession()
{
    pthread_mutex_unlock(&(session_mutex));
    logger->flush();
}

bool TDB::PrepareDb()
{
    try
    {
        std::string query_str = "CREATE TABLE IF NOT EXISTS users(id SERIAL NOT NULL PRIMARY KEY, \
        username TEXT NOT NULL UNIQUE, password TEXT NOT NULL, \
        sesstion_create TIMESTAMP DEFAULT CURRENT_TIMESTAMP)";

        if (GetSession())
        {
            *sql << query_str;
            logger->info("Db has been prepared");
        }
        FreeSession();
    }
    catch (const std::exception &e)
    {
        logger->error("Error in TDB::PrepareDb: {}", e.what());
        FreeSession();
        return false;
    }
    return true;
}
