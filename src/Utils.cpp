#include "Utils.h"

#include <algorithm>

using std::string;
using std::vector;

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
   vector<string> split(const string& original, const string& delimiter)
   {
      size_t pos(0);
      vector<string> parsed;

      while (pos < original.size()) {
         size_t next_pos = original.find(delimiter, pos);

         if (next_pos == string::npos) {
            // no more matches in string, just add last part to array

            parsed.push_back(std::move(original.substr(pos)));
            pos = original.size();
         }
         else {
            parsed.push_back(std::move(original.substr(pos, next_pos - pos)));
            pos = next_pos + delimiter.size();
         }
      }

      // remove empty strings
      parsed.erase(std::remove_if(parsed.begin(), parsed.end(), [](const string& word) {return word.empty(); }), parsed.end());

      return parsed;
   }

}
