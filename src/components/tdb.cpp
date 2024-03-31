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
            // check connection
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
    // write log to file
    logger->flush();
}

bool TDB::PrepareDb()
{
    try
    {
        // create table for users
        std::string query_str = "CREATE TABLE IF NOT EXISTS USERS(id SERIAL NOT NULL PRIMARY KEY, \
        username TEXT NOT NULL UNIQUE, password TEXT NOT NULL, ip_addr TEXT DEFAULT NULL, \
        is_admin BOOLEAN  DEFAULT FALSE, \
        create_at TIMESTAMP DEFAULT CURRENT_TIMESTAMP);";
        // create table for sessions with foreign key to users
        query_str += " CREATE TABLE IF NOT EXISTS SESSIONS(id SERIAL NOT NULL PRIMARY KEY, \
        token TEXT NOT NULL, user_id INTEGER, \
		create_at TIMESTAMP DEFAULT CURRENT_TIMESTAMP, \
        last_connect TIMESTAMP DEFAULT CURRENT_TIMESTAMP, \
        FOREIGN KEY (user_id) REFERENCES users (id) ON DELETE CASCADE);";

        if (GetSession())
        {
            *sql << query_str;
            logger->info("Db has been prepared");

            // check admin in DB
            soci::indicator ind;
            int id = 0;
            soci::statement st = (MainDB.sql->prepare << "SELECT id FROM USERS WHERE username = 'admin'",
                                  soci::into(id, ind));
            st.execute();
            st.fetch();
            
            if (ind == soci::indicator::i_null)
            {
                // insert admin user
                query_str = "INSERT INTO USERS (username, password, is_admin) VALUES ('admin', 'admin', true)";
            }
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
