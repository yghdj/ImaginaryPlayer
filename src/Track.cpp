#include "Track.h"

#include "Utils.h"

#include <iomanip>
#include <sstream>

using std::ostream;

namespace MusicPlayer {
   const int Track::kFormatFlagHandle = std::ios_base::xalloc();

   Track::Track() :
      codec_(Codec::Type::MP3)
   {
      setInvalid_("This track's metadata are empty.");
   }

   Track::Track(std::string title, time_t duration, std::string codec) :
      title_(title), duration_(duration)
   {
      try
      {
         codec_ = Codec::getCodecTypeFromString(codec);
      }
      catch (std::invalid_argument& ex)
      {
         setInvalid_(ex.what());
      }
   }

   std::string Track::serialize() const {
      std::stringstream strm;
      strm << this->title_ << ';' << duration_ / 60 << ':' << duration_ % 60 << ';' << Codec::getCodecAsString(codec_);
      return strm.str();
   }

   bool Track::deserialize(const std::string& source)
   {
      // expected: "<Title>;<Duration>;<Codec>"

      auto splitted_source = split(source, ";");

      if (splitted_source.size() < 3)
      {
         setInvalid_("Missing parameters in source file.");
         return false;
      }

      // Get title
      title_ = splitted_source[0];

      // Get codec
      try
      {
         codec_ = Codec::getCodecTypeFromString(splitted_source[2]);
      }
      catch (std::invalid_argument& ex)
      {
         setInvalid_(ex.what());
         return false;
      }
      
      // Get duration (expected in the format mm:ss)
      auto parsed_duration = split(splitted_source[1], ":");
      
      if (parsed_duration.size() < 2)
      {
         setInvalid_("Duration of track is ill-formed in source file. (should be mm:ss)");
         return false;
      }

      try
      {
         duration_ = std::stoll(parsed_duration[0]) * 60 + std::stoll(parsed_duration[1]);
      }
      catch (std::exception)
      {
         setInvalid_("Duration of track is ill-formed in source file. (should be mm:ss)");
         return false;
      }

      return true;
   }

   bool Track::operator=(const Track& other) const
   {
      if (isInvalid() || other.isInvalid())
         return false;

      return title_    == other.title_
          && duration_ == other.duration_
          && codec_    == other.codec_;
   }

   ostream& Track::setFormat(ostream& os, long format) {
      os.iword(kFormatFlagHandle) = format;
      return os;
   }

   ostream& operator<<(ostream& out, const Track& track)
   {
      long format = out.iword(Track::kFormatFlagHandle);

      if (track.isInvalid())
      {
         out << "Invalid track (" << track.getErrorMessage() << ")";
      }
      else
      {
         if (format == Track::kShortFormat) {
            // prints "<Title> (<Duration>)"
            out << track.title_ << " (" << std::setfill('0') << std::setw(2) << track.duration_ / 60
               << ":" << std::setfill('0') << std::setw(2) << track.duration_ % 60 << ")";
         }
         else // long format
         {
            out << "Title: " << track.title_ << std::endl;

            out << "Duration: " << std::setfill('0') << std::setw(2) << track.duration_ / 60
               << ":" << std::setfill('0') << std::setw(2) << track.duration_ % 60
               << std::endl;

            out << "Codec: " << Codec::getCodecAsString(track.codec_) << std::endl;
         }
      }

      return out;
   }

   std::string Track::getErrorMessage() const
   {
      return isInvalid() ? title_ : "";
   }

   bool Track::isInvalid() const
   {
      return duration_ < 0;
   }

   void Track::setInvalid_(const std::string& message)
   {
      duration_ = -1;
      title_ = message;
   }


}
