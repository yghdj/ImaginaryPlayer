#include "Codec.h"

namespace MusicPlayer {
   std::unordered_map<Codec::Type, std::string> Codec::_string_representations =
   {
      {Codec::Type::AAC, "AAC"},
      {Codec::Type::ALAC, "ALAC"},
      {Codec::Type::AMR, "AMR"},
      {Codec::Type::FLAC, "FLAC"},
      {Codec::Type::G_711, "G.711"},
      {Codec::Type::G_722, "G.722"},
      {Codec::Type::MP3, "MP3"},
      {Codec::Type::OPUS, "Opus"},
      {Codec::Type::VORBIS, "Vorbis"},
   };
}