#include <iostream>
#include "MathOperation.hpp"
#include <gtest/gtest.h>

 namespace testing {


class TestMathOperation2 : public ::testing::Test
{
protected:
    MathOperation operation;

    void SetUp() override {/*std::cout << "Eseguo SetUp"<<std::endl;*/}
    void TearDown() override {/*std::cout << "Eseguo TearDown"<<std::endl;*/}
};


TEST_F(TestMathOperation2, somma)
{
     ASSERT_EQ(operation.somma(1,2), 3);
      EXPECT_EQ(operation.somma(1,2), 3);
}

TEST_F(TestMathOperation2, prodotto)
{
     ASSERT_EQ(operation.prodotto(1,2), 2);
      EXPECT_EQ(operation.prodotto(1,2), 2);
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
  //  EXPECT_ANY_THROW(lanciaEccezione());
    
}

}

