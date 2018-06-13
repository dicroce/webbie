
#include "framework.h"

class test_server_response : public test_fixture
{
public:

    RTF_FIXTURE(test_server_response);
      TEST(test_server_response::TestDefaultConstructor);
      TEST(test_server_response::TestCopyConstructor);
      TEST(test_server_response::TestAssignmentOperator);
      TEST(test_server_response::TestWriteResponse);
      TEST(test_server_response::TestSuccess);
      TEST(test_server_response::TestFailure);
      TEST(test_server_response::TestNVPairs);
      TEST(test_server_response::TestSetBody);
    RTF_FIXTURE_END();

    virtual ~test_server_response() throw() {}

    virtual void setup() {}
    virtual void teardown() {}

    void TestDefaultConstructor();
    void TestCopyConstructor();
    void TestAssignmentOperator();
    void TestWriteResponse();
    void TestSuccess();
    void TestFailure();
    void TestNVPairs();
  void TestSetBody();
};
