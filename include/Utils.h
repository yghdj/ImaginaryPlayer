#pragma once

#include <string>
#include <vector>

namespace MusicPlayer {
   /**
    * \brief Splits a string into multiple substrings according to a delimiter.
    *
    * Values corresponding to a single whitespace are not included in the returned array.
    *
    * \param original The string to split.
    * \param delimiter The delimiter to split the string around.
    * \return A vector of the splitted parts.
    */
   std::vector<std::string> split(const std::string& original, const std::string& delimiter);
}
