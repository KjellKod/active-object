//#include <QtCore/QCoreApplication>
#include <gtest/gtest.h>

#include <iostream>

int main(int argc, char *argv[]) {

  // commented away since a.exec never returns ----
  //QCoreApplication a(argc, argv);
  //a.exec();

  testing::InitGoogleTest(&argc, argv);

  int return_value = RUN_ALL_TESTS();

  std::cout << "FINISHED WITH THE TESTING" << std::endl;
  return return_value;
}
