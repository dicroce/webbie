//=============================================================================
//Copyright (c) 2012 Pelco. All rights reserved.
//
// This file contains trade secrets of Pelco.  No part may be reproduced or
// transmitted in any form by any means or for any purpose without the express
// written permission of Pelco.
//
// $File: WebbyExceptionTest.cpp $
// $Revision:$
// $Date:$
// $Author: Jonathan M Davis $
//=============================================================================

#include "WebbyExceptionTest.h"
#include "Webby/WebbyException.h"
#include "XSDK/XString.h"

using namespace WEBBY;
using namespace XSDK;

REGISTER_TEST_FIXTURE(WebbyExceptionTest);


void WebbyExceptionTest::TestConstructors()
{
    const XString hello = "hello world";

    UT_ASSERT(XString(WebbyException().GetMsg()).empty());
    UT_ASSERT(WebbyException("hello %s", "world").GetMsg() == hello);
    UT_ASSERT(WebbyException(XString("hello world")).GetMsg() == hello);

    UT_ASSERT(HTTPException(42, "hello %s", "world").GetMsg() == hello);
    UT_ASSERT(HTTPException(42, XString("hello world")).GetMsg() == hello);

    UT_ASSERT(XString(HTTP400Exception().GetMsg()).empty());
    UT_ASSERT(HTTP400Exception("hello %s", "world").GetMsg() == hello);
    UT_ASSERT(HTTP400Exception(XString("hello world")).GetMsg() == hello);

    UT_ASSERT(XString(HTTP401Exception().GetMsg()).empty());
    UT_ASSERT(HTTP401Exception("hello %s", "world").GetMsg() == hello);
    UT_ASSERT(HTTP401Exception(XString("hello world")).GetMsg() == hello);

    UT_ASSERT(XString(HTTP403Exception().GetMsg()).empty());
    UT_ASSERT(HTTP403Exception("hello %s", "world").GetMsg() == hello);
    UT_ASSERT(HTTP403Exception(XString("hello world")).GetMsg() == hello);

    UT_ASSERT(XString(HTTP404Exception().GetMsg()).empty());
    UT_ASSERT(HTTP404Exception("hello %s", "world").GetMsg() == hello);
    UT_ASSERT(HTTP404Exception(XString("hello world")).GetMsg() == hello);

    UT_ASSERT(XString(HTTP415Exception().GetMsg()).empty());
    UT_ASSERT(HTTP415Exception("hello %s", "world").GetMsg() == hello);
    UT_ASSERT(HTTP415Exception(XString("hello world")).GetMsg() == hello);

    UT_ASSERT(XString(HTTP453Exception().GetMsg()).empty());
    UT_ASSERT(HTTP453Exception("hello %s", "world").GetMsg() == hello);
    UT_ASSERT(HTTP453Exception(XString("hello world")).GetMsg() == hello);

    UT_ASSERT(XString(HTTP500Exception().GetMsg()).empty());
    UT_ASSERT(HTTP500Exception("hello %s", "world").GetMsg() == hello);
    UT_ASSERT(HTTP500Exception(XString("hello world")).GetMsg() == hello);
}


void WebbyExceptionTest::TestGetStatus()
{
    UT_ASSERT(HTTPException(42, "hello world").GetStatus() == 42);
    UT_ASSERT(HTTP400Exception("hello world").GetStatus() == 400);
    UT_ASSERT(HTTP401Exception("hello world").GetStatus() == 401);
    UT_ASSERT(HTTP403Exception("hello world").GetStatus() == 403);
    UT_ASSERT(HTTP404Exception("hello world").GetStatus() == 404);
    UT_ASSERT(HTTP415Exception("hello world").GetStatus() == 415);
    UT_ASSERT(HTTP453Exception("hello world").GetStatus() == 453);
    UT_ASSERT(HTTP500Exception("hello world").GetStatus() == 500);
    UT_ASSERT(HTTP501Exception("hello world").GetStatus() == 501);
}


//This is a slighty altered version of UT_ASSERT_THROWS which also checks
//the status code. The original is in cppunit/TestAssert.h.
# define HTTP_ASSERT_THROW( expression, statusCode, ExceptionType )   \
   do {                                                                       \
      bool cpputCorrectExceptionThrown_ = false;                              \
      bool cpputCorrectStatusCode_ = false;                                   \
      CPPUNIT_NS::Message cpputMsg_( "expected exception not thrown" );       \
      cpputMsg_.addDetail( CPPUNIT_NS::AdditionalMessage() );                 \
      cpputMsg_.addDetail( "Expected Exception: "                             \
                           CPPUNIT_GET_PARAMETER_STRING( ExceptionType ) );   \
      const XString cpputStatusStr_ =                                         \
        XString::Format("Expected Status: %d", statusCode);                   \
      cpputMsg_.addDetail( cpputStatusStr_ );                                 \
                                                                              \
      try {                                                                   \
         expression;                                                          \
      } catch ( const ExceptionType & e ) {                                   \
         ExceptionType f = e;                                                 \
         cpputCorrectExceptionThrown_ = typeid(e) == typeid(f);               \
         cpputCorrectStatusCode_ = statusCode == e.GetStatus();               \
         const XString cpputActualStatusStr_ = XString::FromInt(e.GetStatus()); \
         cpputMsg_.addDetail( "Actual Status Code : " +                       \
                              cpputActualStatusStr_ );                        \
      } catch ( const std::exception &e) {                                    \
         cpputMsg_.addDetail( "Actual Exception : " +                         \
                              CPPUNIT_EXTRACT_EXCEPTION_TYPE_( e,             \
                                          "std::exception or derived") );     \
         cpputMsg_.addDetail( std::string("What()  : ") + e.what() );         \
      } catch ( ... ) {                                                       \
         cpputMsg_.addDetail( "Actual Exception : unknown.");                 \
      }                                                                       \
                                                                              \
      if ( cpputCorrectExceptionThrown_ && cpputCorrectStatusCode_)           \
         break;                                                               \
                                                                              \
      CPPUNIT_NS::Asserter::fail( cpputMsg_,                                  \
                                  CPPUNIT_SOURCELINE() );                     \
   } while ( false )


void WebbyExceptionTest::TestThrowHTTP()
{
}
