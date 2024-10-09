#include <gtest/gtest.h>
#include <glog/logging.h>

class Environment : public ::testing::Environment {
 public:
  ~Environment() override {}

  // Override this to define how to set up the environment.
  void SetUp() override {

        google::SetLogDestination(google::GLOG_INFO,"./" );
    google::InitGoogleLogging("");
  }

  // Override this to define how to tear down the environment.
  void TearDown() override {}
};
::testing::Environment* const timer_env = ::testing::AddGlobalTestEnvironment(new Environment);