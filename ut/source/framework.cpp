
#include "framework.h"
#include <time.h>
#include <stdarg.h>
#include <stdlib.h>
#include <sys/time.h>
#include <unistd.h>

using namespace std;

vector<shared_ptr<test_fixture>> _test_fixtures;

void rtf_usleep(unsigned int usec)
{
    usleep(usec);
}

string rtf_format(const char* fmt, ...)
{
    va_list args;
    va_start(args, fmt);
    const string result = rtf_format(fmt, args);
    va_end(args);
    return result;
}

string rtf_format(const char* fmt, va_list& args)
{
    va_list newargs;
    va_copy(newargs, args);
    const int chars_written = vsnprintf(nullptr, 0, fmt, newargs);
    const int len = chars_written + 1;

    vector<char> str(len);

    va_copy(newargs, args);
    vsnprintf(&str[0], len, fmt, newargs);

    string formatted(&str[0]);

    return formatted;
}

// This is a globally (across test) incrementing counter so that tests can avoid having hardcoded port
// numbers but can avoid stepping on eachothers ports.
int _next_port = 5000;

int rtf_next_port()
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
    bool dontWaitOnFail = false;

    if( argc > 1 )
    {
        std::string arg1 = argv[1];

        if(arg1 == "--dont-wait-on-fail")
            dontWaitOnFail = true;
        else fixture_name = argv[1];

        if( argc > 2 )
        {
            std::string arg2 = argv[2];
            if(arg2 == "--dont-wait-on-fail")
                dontWaitOnFail = true;
            else fixture_name = argv[2];
        }
    }
    std::string arg;
    if(argc > 1)
        arg = argv[1];

    srand( time(0) );

    bool something_failed = false;

    for(auto& tf : _test_fixtures)
    {
        if( !fixture_name.empty() )
            if(tf->get_name() != fixture_name )
                continue;

        tf->run_tests();

        if( tf->something_failed() )
        {
            something_failed = true;
            tf->print_failures();
        }
    }

    if( !something_failed )
        printf("\nSuccess.\n");
    else printf("\nFailure.\n");

    if(something_failed && !dontWaitOnFail)
        system("/bin/bash -c 'read -p \"Press Any Key\"'");

    return 0;
}
