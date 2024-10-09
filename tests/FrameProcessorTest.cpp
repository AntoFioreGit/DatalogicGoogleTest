#include <iostream>
#include "MathOperation.hpp"
#include <gtest/gtest.h>
#include <glog/logging.h>


#include "io/file_json.h"

class FrameProcessorTest : public ::testing::Test
{
protected:
//inserrire la classe  FrameProcessor
   // MathOperation operation;

    void SetUp() override {/*std::cout << "Eseguo SetUp"<<std::endl;*/}
    void TearDown() override {/*std::cout << "Eseguo TearDown"<<std::endl;*/}
};


TEST_F(FrameProcessorTest, loadConfig)
{
    
     // EXPECT_EQ(operation.somma(1,2), 3);
     bool value = true;
     EXPECT_EQ(true ,true);
}
TEST_F(FrameProcessorTest, loadAlgoParameters)
{
    
     // EXPECT_EQ(operation.somma(1,2), 3);
     bool value = true;
     EXPECT_EQ(true ,true);
}
 
TEST_F(FrameProcessorTest, initialize)
{
    
     // EXPECT_EQ(operation.somma(1,2), 3);
     bool value = true;
     EXPECT_EQ(true ,true);
}
TEST_F(FrameProcessorTest, configure)
{
    
     // EXPECT_EQ(operation.somma(1,2), 3);
     bool value = true;
     EXPECT_EQ(true ,true);
}
TEST_F(FrameProcessorTest, invoke)
{
    
     // EXPECT_EQ(operation.somma(1,2), 3);
     bool value = true;
     EXPECT_EQ(true ,true);
}

TEST_F(FrameProcessorTest, getProfiles)
{
    
     // EXPECT_EQ(operation.somma(1,2), 3);
     bool value = true;
     EXPECT_EQ(true ,true);
}
// TEST_F(TestMathOperation, somma)
// {

//      LOG(INFO) << "Primo log: " ;
//      ASSERT_EQ(operation.somma(1,2), 3);
//       EXPECT_EQ(operation.somma(1,2), 3);
// }
// TEST_F(TestMathOperation, prodotto)
// {
//      ASSERT_EQ(operation.prodotto(1,2), 2);
//       EXPECT_EQ(operation.prodotto(1,2), 2);
// }
// testing::AssertionResult IsEven(int n) {
//     // return testing::AssertionFailure() << n << " is odd";
//     return testing::AssertionSuccess() << n << " is even";
// }
// int lanciaEccezione() {
//    // return 0;
//     throw int(0);
// }
// TEST(globalFunction, globalFuntion)
// {
//      ASSERT_EQ(globalFunction(), 0);
//       // ASSERT_EQ(globalFunction(), 1);
//     //EXPECT_TRUE(IsEven(1));

// float val1=.123456789,val2=.12345676;
//     EXPECT_FLOAT_EQ(val1,val2);
//     int x =0;
//     int y=1;
//     EXPECT_ANY_THROW(lanciaEccezione());
//     Prova p;
//     p.division();
    
// }










