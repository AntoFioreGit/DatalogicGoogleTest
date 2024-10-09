#include <iostream>
#include "MathOperation.hpp"
#include <gtest/gtest.h>
#include <glog/logging.h>

#include "io/file_json.h"
#include "common/frame_processor.h"
 namespace testing {

// Returns an AssertionResult object to indicate that an assertion has
// succeeded.
AssertionResult AssertionSuccess();

// Returns an AssertionResult object to indicate that an assertion has
// failed.
AssertionResult AssertionFailure();

}
class TestMathOperation : public ::testing::Test
{
protected:
    MathOperation operation;

    void SetUp() override {/*std::cout << "Eseguo SetUp"<<std::endl;*/}
    void TearDown() override {/*std::cout << "Eseguo TearDown"<<std::endl;*/}
};

class TestMathOperation1: public ::testing::Test
{
protected:
    MathOperation operation;

    void SetUp() override {/*std::cout << "Eseguo SetUp"<<std::endl;*/}
    void TearDown() override {/*std::cout << "Eseguo TearDown"<<std::endl;*/}
};
TEST_F(TestMathOperation1, somma)
{
     ASSERT_EQ(operation.somma(1,2), 3);
      EXPECT_EQ(operation.somma(1,2), 3);
}
 
TEST_F(TestMathOperation, somma)
{

     LOG(INFO) << "Primo log: " ;
     ASSERT_EQ(operation.somma(1,2), 3);
      EXPECT_EQ(operation.somma(1,2), 3);
}
TEST_F(TestMathOperation, prodotto)
{
     ASSERT_EQ(operation.prodotto(1,2), 2);
      EXPECT_EQ(operation.prodotto(1,2), 2);
}
testing::AssertionResult IsEven(int n) {
    // return testing::AssertionFailure() << n << " is odd";
    return testing::AssertionSuccess() << n << " is even";
}
int lanciaEccezione() {
   // return 0;
    throw int(0);
}
TEST(globalFunction, globalFuntion)
{
     ASSERT_EQ(globalFunction(), 0);
      // ASSERT_EQ(globalFunction(), 1);
    //EXPECT_TRUE(IsEven(1));

float val1=.123456789,val2=.12345676;
    EXPECT_FLOAT_EQ(val1,val2);
    int x =0;
    int y=1;
    EXPECT_ANY_THROW(lanciaEccezione());
   
}


int main(int argc, char **argv)
{
    ::testing::InitGoogleTest(&argc, argv);
    google::SetLogDestination(google::GLOG_INFO,"./" );
    google::InitGoogleLogging(argv[0]);

    FrameProcessor bb;

    //  nlohmann::json json_config;
    // if (!rs::io::readJsonFile("gggg", json_config))
    // {
    //     LOG(ERROR) << "Failed to parse configuration file: " << "gggg";
    //     return false;
    // }







    return RUN_ALL_TESTS();
}
