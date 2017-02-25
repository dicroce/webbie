
#include "cppkit/ck_logger.h"
#include "cppkit/ck_string.h"
#include "cppkit/ck_path.h"

using namespace cppkit;

void main_entry()
{
    ck_string logPath = ck_string::format( "log.txt" );
    ck_log::enable_print_to_file( logPath.c_str(), true );
    ck_log::set_log_level( LOGLEVEL_INFO );
    ck_log::install_terminate();
}
