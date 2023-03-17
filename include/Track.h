#pragma once

#include "Codec.h"

#include <iostream>
#include <memory>
#include <string>

namespace MusicPlayer
{

   /**
    * \brief A music track that can be played in the music player.
    */
   class Track
   {
   public:
      Track();

      Track(std::string title, time_t duration, std::string _codec);

      bool deserialize(const std::string& input);

      bool operator=(const Track& other) const;

      // Manipulators for output format
      static inline std::ostream& shortFormat(std::ostream& os)
      {
         return setFormat(os, kShortFormat);
      }

      static inline std::ostream& longFormat(std::ostream& os)
      {
         return setFormat(os, kLongFormat);
      }

      friend std::ostream& operator<<(std::ostream& output, const Track& track);

      bool isInvalid() const;
      std::string getErrorMessage() const;

   private:
      std::string title_;
      time_t duration_;
      Codec::Type codec_;

      static const long kShortFormat = 0;
      static const long kLongFormat = 1;
      static std::ostream& setFormat(std::ostream& os, long format);
      static const int kFormatFlagHandle;

      void setInvalid_(const std::string& message);
   };

}
