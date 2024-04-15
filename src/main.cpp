#ifdef __linux__
#include <sys/types.h>
#include <sys/stat.h>
#include <signal.h>
#include <termios.h>
#else
#include <windows.h>
#include <conio.h>
#endif // __linux__

// if need to use filesystem uncomment it
// #if __GNUC__ >= 8
// #include <filesystem>
// namespace fs = std::filesystem;
// #else
// #include <experimental/filesystem>
// namespace fs = std::experimental::filesystem;
// #endif

#include "handlers/authHandler.hpp"
#include "handlers/userHandler.hpp"
#include "handlers/htmlHandler.hpp"
#include "components/tdb.hpp"

using namespace std;

#ifdef __linux__
extern void Rest_StartDaemon(void);
#else
extern void Rest_StartService(void);
#endif // __linux__

#ifdef __linux__
// To capture keys in console mode, reading input without pressing enter
// so that the output data is not buffered, call setbuf(stdout, NULL);
struct termios stdin_orig;

// Resetting the terminal parameters (called after exiting the cycle)
void term_reset()
{
    tcsetattr(STDIN_FILENO, TCSANOW, &stdin_orig);
    tcsetattr(STDIN_FILENO, TCSAFLUSH, &stdin_orig);
}

// Called before the loop, prevents stopping in the getchar procedure
void term_nonblocking()
{
    struct termios newt;
    tcgetattr(STDIN_FILENO, &stdin_orig);
    fcntl(STDIN_FILENO, F_SETFL, O_NONBLOCK); // non-blocking
    newt = stdin_orig;
    newt.c_lflag &= ~(ICANON | ECHO);
    tcsetattr(STDIN_FILENO, TCSANOW, &newt);

    atexit(term_reset);
}
#endif // __linux__

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
            //"document_root", INFOBASE.c_str(),
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
    mg_set_request_handler(civetweb, "/logout", AuthHandler::logout, NULL);
    mg_set_request_handler(civetweb, "/ping", AuthHandler::ping, NULL);

    // user handler
    mg_set_request_handler(civetweb, "/user/create$", UserHandler::CreateNewUser, NULL);
    mg_set_request_handler(civetweb, "/user/info/*$", UserHandler::GetUserInfo, NULL);
    mg_set_request_handler(civetweb, "/user/update$", UserHandler::UpdateCurrentUser, NULL);
    mg_set_request_handler(civetweb, "/user/delete/*$", UserHandler::DeleteUserById, NULL);

    // html handler
    mg_set_request_handler(civetweb, "/html/users$", HtmlHandler::AllUsersPage, NULL);
    mg_set_request_handler(civetweb, "/html/sessions/*$", HtmlHandler::UserSessionsPage, NULL);
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

int main(int argc, char *argv[])
{
    try
    {
        printf("locale: %s\n", std::setlocale(LC_ALL, NULL));
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

        // read cmd line
        string cmdLine = "";
        for (int i = 0; i < argc; ++i)
        {
            cmdLine += argv[i];
            cmdLine += " ";
        }
        logger->info("CMD: {}", cmdLine);

        logger->info("Prepare DB");
        MainDB.PrepareDb();

        startHHTPServer(8849, 0, "");

        if (cmdLine.compare("-service") == 0)
        {
#ifdef __linux__
            Sleep(30000);
            Rest_StartDaemon();
#else
            Rest_StartService();
#endif // __linux__
        }
        else
        {
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
                Sleep(1000);
                if (counter == 10)
                {
                    SessionsService::CheckAllSessionsTime();
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
        }

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

/// LINUX daemon
#ifdef __linux__
bool need_stop = false;

void hdl(int sig)
{
    need_stop = true;
}

void Rest_StartDaemon(void)
{
    struct sigaction act;
    memset(&act, 0, sizeof(act));
    act.sa_handler = hdl;
    sigset_t set;
    sigemptyset(&set);
    sigaddset(&set, SIGUSR1);
    sigaddset(&set, SIGUSR2);
    sigaddset(&set, SIGTERM);
    act.sa_mask = set;
    sigaction(SIGUSR1, &act, 0);
    sigaction(SIGUSR2, &act, 0);
    sigaction(SIGTERM, &act, 0);

    int counter = 0;
    logger->debug("start daemon loop");
    while (!need_stop)
    {
        Sleep(1000);
        if (counter == 10)
        {
            SessionsService::CheckAllSessionsTime();
            logger->flush();
            counter = 0;
        }
        counter++;
    }
    logger->debug("daemon loop end, exit");
    return;
}

#else
/// Windows Service main application
// Add your code where necessary to create your own windows service application.

// Some global vars
SERVICE_STATUS gStatus;
SERVICE_STATUS_HANDLE gStatusHandle;
HANDLE ghStopEvent = NULL;

void MySetServiceStatus(DWORD State, DWORD ExitCode, DWORD Wait)
{
    gStatus.dwCurrentState = State;
    gStatus.dwWin32ExitCode = ExitCode;
    gStatus.dwWaitHint = Wait;

    if (State == SERVICE_START_PENDING)
        gStatus.dwControlsAccepted = 0;
    else
        gStatus.dwControlsAccepted = SERVICE_ACCEPT_STOP;

    SetServiceStatus(gStatusHandle, &gStatus);
}

void WINAPI ServiceCtrlHandler(DWORD CtrlCmd)
{
    switch (CtrlCmd)
    {
    case SERVICE_CONTROL_STOP:
        logger->error("ServiceCtrlHandler: SERVICE_CONTROL_STOP");
        MySetServiceStatus(SERVICE_STOP_PENDING, NO_ERROR, 30000);
        // Add here the necessary code to stop the daemon
        SetEvent(ghStopEvent);
        break;

    case SERVICE_CONTROL_INTERROGATE:
        // Add here the necessary code to query the daemon
        break;
    }

    MySetServiceStatus(gStatus.dwCurrentState, NO_ERROR, 0);
}

DWORD WINAPI ServiceWorkerThread(LPVOID lpParam)
{
    //  Periodically check if the service has been requested to stop
    int counter = 0;
    while (WaitForSingleObject(ghStopEvent, 0) != WAIT_OBJECT_0)
    {
        Sleep(1000);
        if (counter == 10)
        {
            SessionsService::CheckAllSessionsTime();
            logger->flush();
            counter = 0;
        }
        counter++;
    }

    return ERROR_SUCCESS;
}

void WINAPI ServiceMain(DWORD dwArgc, LPSTR *lpszArgv)
{
    // printf in this function will not work when running from SCM

    gStatusHandle = RegisterServiceCtrlHandler(/*DASserviceName*/ *lpszArgv, ServiceCtrlHandler);
    if (!gStatusHandle)
    {
        int e;

        e = GetLastError();
        if (e == ERROR_INVALID_NAME)
            printf("RegisterServiceCtrlHandler failed (ERROR_INVALID_NAME)\n");
        else if (e == ERROR_SERVICE_DOES_NOT_EXIST)
            printf("RegisterServiceCtrlHandler failed (ERROR_SERVICE_DOES_NOT_EXIST)\n");
        else
            printf("RegisterServiceCtrlHandler failed (%d)\n", (int)e);

        return;
    }

    gStatus.dwServiceType = SERVICE_WIN32_OWN_PROCESS; // SERVICE_WIN32;
    gStatus.dwServiceSpecificExitCode = 0;
    MySetServiceStatus(SERVICE_START_PENDING, NO_ERROR, 30000);

    // HERE put your daemon initialization code
    // if it takes too long to start, call MySetServiceStatus() with
    // SERVICE_START_PENDING periodically.
    // If initialization fails, call MySetServiceStatus with SERVICE_STOPPED.

    // end of daemon initialization

    MySetServiceStatus(SERVICE_RUNNING, NO_ERROR, 0);
    ghStopEvent = CreateEvent(NULL, TRUE, FALSE, NULL);
    if (ghStopEvent == NULL)
    {
        MySetServiceStatus(SERVICE_STOPPED, NO_ERROR, 0);
        logger->error("ServiceMain: CreateEvent failed, exit");
        return;
    }

    HANDLE hThread = CreateThread(NULL, 0, ServiceWorkerThread, NULL, 0, NULL);
    logger->debug("ServiceMain: WaitForSingleObject...");
    WaitForSingleObject(hThread, INFINITE);

    MySetServiceStatus(SERVICE_STOPPED, NO_ERROR, 0);

    logger->info("ServiceMain: exit");
}

SERVICE_TABLE_ENTRY DispatchTable[] =
    {
        {const_cast<char*>("DataAccesServer"),
         (LPSERVICE_MAIN_FUNCTION)ServiceMain},
        {NULL,
         NULL}};

void Rest_StartService(void)
{
    if (!StartServiceCtrlDispatcher(DispatchTable))
        printf("StartServiceCtrlDispatcher failed\n");
}
#endif