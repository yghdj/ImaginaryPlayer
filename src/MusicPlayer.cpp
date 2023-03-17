// MusicPlayer.cpp : This file contains the 'main' function. Program execution begins and ends there.
//

#include "Shell.h"

#include <iostream>
#include <string_view>

using namespace std::string_view_literals;

int main()
{
   MusicPlayer::Shell main_shell(std::cin, std::cout);

   main_shell.run();

   return 0;
}
