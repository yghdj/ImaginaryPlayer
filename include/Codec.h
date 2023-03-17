#pragma once

#include <algorithm>
#include <string>
#include <unordered_map>
#include <stdexcept>

namespace MusicPlayer {

   /**
    * \brief Represents a protocol or technology to decode and encode audio data.
    */
   class Codec {
   public:

      /**
       * \brief List of all supported codecs.
       *
       * This list was created using this reference page:
       * https://developer.mozilla.org/en-US/docs/Web/Media/Formats/Audio_codecs
       *
       */
      enum class Type {
         AAC,
         ALAC,
         AMR,
         FLAC,
         G_711,
         G_722,
         MP3,
         OPUS,
         VORBIS
      };

      /**
       * \brief Returns the name of a codec as a string.
       *
       * \param codec The codec type to print.
       * \return The name of the codec.
       */
      static inline std::string getCodecAsString(Codec::Type codec) {
         return _string_representations.count(codec) ? _string_representations[codec] : "Unknown";
      }

      /**
       * \brief Returns the name of a codec as a string.
       *
       * \param codec The codec type to print.
       * \return The name of the codec.
       */
      static inline Codec::Type getCodecTypeFromString(const std::string& source) {
         auto match = std::find_if(
            _string_representations.begin(),
            _string_representations.end(),
            [&source](const auto& pair)
            {
               return pair.second == source;
            }
         );

         if (match != _string_representations.end()) {
            return match->first;
         }
         else {
            throw std::invalid_argument("The codec type " + source + " is not supported");
         }
      }

   private:

      static std::unordered_map<Type, std::string> _string_representations;

   };

}
