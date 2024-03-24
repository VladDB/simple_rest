#include <windows.h>
#include <conio.h>

// #if __GNUC__ >= 8
// #include <filesystem>
// namespace fs = std::filesystem;
// #else
// #include <experimental/filesystem>
// namespace fs = std::experimental::filesystem;
// #endif

#include "handlers/authHandler.hpp"
#include "repository/tdb.hpp"

using namespace std;

/* Server context handle */
static struct mg_context *civetweb = NULL;

int cvw_log_message(const struct mg_connection *conn, const char *message)
{
    // PRINT(message);
    return 1;
}

void startHHTPServer(int port, int sslPort, string sslPath)
{
    mg_init_library(0);
    logger->info("Starting HTTP server...");
    static struct mg_callbacks callbacks;
    memset(&callbacks, 0, sizeof(callbacks));
    callbacks.log_message = cvw_log_message;
    string sPort = to_string(port);

    if (sslPort != 0 && sslPath.length() != 0)
        sPort = sPort + "," + to_string(sslPort) + "s";
    else
        logger->error("For using SSL specify port and full path to certificate in server_settings.ini");

    logger->info("HTTP interface port: " + sPort);

    static const char *cvw_options[] =
        {
            //"document_root", REST_INFOBASE.c_str(),
            "listening_ports", sPort.c_str(),
            "num_threads", "10",
            "request_timeout_ms", "10000",
            "ssl_certificate", sslPath.c_str(),
            //"websocket_timeout_ms", "3600000",
            //"enable_auth_domain_check", "yes",
            0};

    civetweb = mg_start(&callbacks, 0, cvw_options);
    if (!civetweb)
    {
        logger->error("Error mg_start - civetweb is NULL, HTTP server is not running !");
        return;
    }

    // auth
    mg_set_request_handler(civetweb, "/login", AuthHandler::login, NULL);
}

void StopHTTPserver(void)
{
    if (!civetweb)
        return;
    logger->info("Stoping HTTP server...");
    /* Stop the server */
    mg_stop(civetweb);
    /* Un-initialize the library */
    mg_exit_library();
}

int main(int, char **)
{
    try
    {
        init_logger();
        if (!logger)
            printf("Logger does not init");
        logger->info("----------------------------------------------");
#if defined __x86_64__ && !defined __ILP32__
        logger->info("Build:  {}  {}, version x64", __DATE__, __TIME__);
#else
        logger->info("Build:  {}  {}, version x32", __DATE__, __TIME__);
#endif
        logger->info("GCC version:  {}", __VERSION__);
        logger->info("----------------------------------------------");
        logger->info("Start Server");

        logger->info("Prepare DB");
        MainDB.PrepareDb();

        startHHTPServer(8849, 0, "");

        // waiting pushing Esc
        logger->info("Press ESC to stop Server");
        int counter = 0;
        int key = 0;
#ifdef __linux__
        term_nonblocking();
        setbuf(stdout, NULL);
#endif // __linux__

        while (key != 27)
        {
            // Sleep(1000);
            if (counter == 30)
            {
                logger->flush();
                counter = 0;
            }
            counter++;
#ifdef __linux__
            key = getchar();
#else
            if (kbhit() != 0)
                key = getch();
#endif // __linux__
        }
#ifdef __linux__
        term_reset();
#endif // __linux__

        StopHTTPserver();
        deinit_logger();
    }
    catch (exception &e)
    {
        if (civetweb)
            StopHTTPserver();

        if (logger)
        {
            logger->error("Exception in main.cpp: {}", e.what());
            deinit_logger();
        }
        else
        {
            printf("Logger does not init. Exception in main.cpp: %s", e.what());
        }
    }
    return 0;
}
