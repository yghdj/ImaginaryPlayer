#pragma once

#include "Track.h"

#include <functional>
#include <iostream>
#include <list>
#include <random>
#include <set>
#include <string>
#include <tuple>
#include <vector>

namespace MusicPlayer
{
   class Shell
   {
   public:
      using ArgumentArray = std::vector<std::string>;
      using Instruction = std::function<void(Shell*, const ArgumentArray&)>;

      using Playlist = std::list<std::pair<std::string, Track>>;

      Shell();

      Shell(std::istream& in, std::ostream& out);

      void setInputStream(std::istream& in)
      {
         input_ = &in;
      }

      void setOutputStream(std::ostream& out)
      {
         output_ = &out;
      }

      void run();

   private:
      std::unordered_map<std::string, Instruction> available_instructions_;

      Playlist playlist_;
      Playlist::iterator currently_playing_;
      bool is_playing_;
      bool random_mode_;
      bool repeat_mode_;

      std::mt19937 rng_;

      std::istream* input_;
      std::ostream* output_;

      void printWelcomeMessage_();
      std::tuple<Instruction, ArgumentArray> getInstruction_();
      std::set<int> parseIndicesFromArgs_(const ArgumentArray&);
      void goToRandomTrack_();

      // Instructions
      void help_(const ArgumentArray&);
      void noop_(const ArgumentArray&);
      void unknownInstruction_(const ArgumentArray&);

      void addTrack_(const ArgumentArray&);
      void removeTrack_(const ArgumentArray&);
      void removeDuplicates_(const ArgumentArray&);
      void showTrack_(const ArgumentArray&);
      void showPlaylist_(const ArgumentArray&);

      void play_(const ArgumentArray&);
      void pause_(const ArgumentArray&);
      void next_(const ArgumentArray&);
      void previous_(const ArgumentArray&);

      void random_(const ArgumentArray&);
      void repeat_(const ArgumentArray&);

#ifdef _DEBUG
      void cd_(const ArgumentArray&);
      void loadPlaylist_(const ArgumentArray&);
#endif
   };

}
