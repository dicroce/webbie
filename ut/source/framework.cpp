
#include "framework.h"
#include <time.h>
#include <stdlib.h>

using namespace std;

list<test_fixture*> _test_fixtures;

static char* _failure_messages[] = { (char*)"Go fix your code!", (char*)"Yay, one less bug found by a customer!", (char*)"Don't even think about commenting this test out!", (char*)"Checking in broken code is worse than a broken test.", (char*)"A little pain now means less pain in the future.", (char*)"Remember, its doing what you told it to.", (char*)"Go for a walk and think about something else for a while. The answer will come to you.", (char*)"Check your assumptions.", (char*)"Wow, you really expected THAT to work?", (char*)"You should try asking about this on StackOverflow.com.", (char*)"One less field issue.", (char*)"Try asking someone.", (char*)"Trust me, it could be a lot worse!", (char*)"Try running the tests under valgrind.", (char*)"Believe me, I've seen a lot worse.", (char*)"You're contributing to the entropy of the universe." };

static char* _success_messages[] = { (char*)"You rock!", (char*)"A beautiful sight, isn't it?", (char*)"Nice one.", (char*)"Excellent command or file name.", (char*)"... (out of breath!)...", (char*)"That was some beautiful code.", (char*)"Have you tried running this under valgrind?", (char*)"Good job.", (char*)"Fight the technical debt!", (char*)"A little bit less entropy in the universe." };


#ifdef WIN32
#include <winsock2.h>
void ut_usleep(unsigned int usec)
{
    if (usec >= 1000)
        Sleep((DWORD)(usec / 1000));
    else if (usec == 0)
        Sleep(0); // Support an explicit sleep time of 0.
    else
        Sleep(1);
}
#else
#include <sys/time.h>
#include <unistd.h>
void ut_usleep(unsigned int usec)
{
    usleep(usec);
}
#endif

// This is a globally (across test) incrementing counter so that tests can avoid having hardcoded port
// numbers but can avoid stepping on eachothers ports.
int _next_port = 5000;

int ut_next_port()
{
    int ret = _next_port;
    _next_port++;
    return ret;
}

void handle_terminate()
{
    printf( "\nuncaught exception terminate handler called!\n" );
    fflush(stdout);

    std::exception_ptr p = std::current_exception();

    if( p )
    {
        try
        {
            std::rethrow_exception( p );
        }
        catch(std::exception& ex)
        {
            printf("%s\n",ex.what());
            fflush(stdout);
        }
        catch(...)
        {
            printf("caught an unknown exception in custom terminate handler.\n");
        }
    }
}

int main( int argc, char* argv[] )
{
    set_terminate( handle_terminate );

    std::string fixture_name = "";

    if( argc > 1 )
        fixture_name = argv[1];

    srand( time(0) );

    bool something_failed = false;

    list<test_fixture*>::iterator i = _test_fixtures.begin();
    for( ; i != _test_fixtures.end(); i++ )
    {
        if( fixture_name.length() > 0 )
            if((*i)->get_name() != fixture_name )
                continue;

        (*i)->run_tests();
        if( (*i)->something_failed() )
        {
            something_failed = true;
            (*i)->print_failures();
        }
    }

    if( !something_failed )
    {
        printf("\nSuccess. ");
        printf("%s\n",_success_messages[random()%(sizeof(_success_messages)/sizeof(_success_messages[0]))]);
    }
    else
    {
        printf("\nFailure. ");
        printf("%s\n",_failure_messages[random()%(sizeof(_failure_messages)/sizeof(_failure_messages[0]))]);
    }

    i = _test_fixtures.begin();
    while( i != _test_fixtures.end() )
    {
        test_fixture* p = *i;

        delete p;

        i++;
    }

    return 0;
}
