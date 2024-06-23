#include <algorithm>
#include <cmath>
#include <fstream>
#include <iostream>
#include <iterator>
#include <numeric>
#include <sstream>
#include <string>
#include <vector>

/**
 * Binary function.
 * \param - PreviousCount: Number of already counted chars.
 * \param - Current charecter.
 * \return - Number of new lines based on the PreviousCount and the new characher C.
 */
auto f(int PreviousCount, char C) -> int { return (C != '\n') ? PreviousCount : PreviousCount + 1; }

auto ReadFileToString(std::string const &FileName) -> std::string
{
   std::ifstream FileStream(FileName);
   std::ostringstream Buffer{};
   Buffer << FileStream.rdbuf();
   return Buffer.str();
}

auto CountLines(std::string const &S) -> int { return std::accumulate(S.cbegin(), S.cend(), 0, f); }

auto CountLines2(std::string const &FileName) -> int
{
   std::ifstream In(FileName);
   In.unsetf(std::ios::skipws);

   return std::count(std::istream_iterator<char>(In), std::istream_iterator<char>(), '\n');
}

auto CountLinesInFiles(std::vector<std::string> const &Files) -> std::vector<int> { return {}; }

struct struct_a
{
   int X{};
};

struct struct_b
{
   int Y{};
};

auto Increment(struct_a &StructA) -> void { StructA.X++; }
auto Increment(struct_b &StructB) -> void { StructB.Y++; }

template <typename Object, typename Function>
decltype(auto) call_on_object(Object &&object, Function function)
{
   return function(std::forward<Object>(object));
}

auto main(int argc, char const *argv[]) -> int
{
   auto const NumFiles = argc - 1;
   std::cout << "Hello World!" << NumFiles << std::endl;

   if (NumFiles > 0)
   {
      std::vector<std::string> vFiles{};
      for (int Idx = 1;  //!<
           Idx < argc;   //!<
           ++Idx)        //!<
      {
         vFiles.push_back(std::string(argv[Idx]));
      }

      for (auto const &File : vFiles)
      {
         std::cout << "File " << File << " contains " << CountLines(ReadFileToString(File)) << " lines" << std::endl;
      }
   }

   struct_a A{};
   std::cout << A.X << std::endl;
   call_on_object(A, Increment);
   std::cout << A.X << std::endl;

   struct_b Y{};
   // std::cout << call_on_object(Y, Increment) << std::endl;
   // std::cout << call_on_object(Y, Increment2) << std::endl;
}
