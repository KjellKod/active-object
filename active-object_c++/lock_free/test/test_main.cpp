#include <iostream>
#include <gtest/gtest.h>

int main(int argc, char **argv) {
  std::cout << "Running main() from circularfifo test main.cpp\n";

  testing::InitGoogleTest(&argc, argv);
  return RUN_ALL_TESTS();
}
