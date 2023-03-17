#include "Shell.h"

#include "Help.h"
#include "Utils.h"
#include "Version.h"

#include <fstream>
#include <sstream>
#include <filesystem>

using std::endl;
using std::string;
using std::unordered_map;
using std::vector;

namespace MusicPlayer
{

#pragma region Constructors

   Shell::Shell() :
      input_(nullptr), output_(nullptr), is_playing_(false),
      random_mode_(false), repeat_mode_(false)
   {
      // construct instruction array
      available_instructions_ = {
         { "help", &Shell::help_ },
         { "add_track", &Shell::addTrack_ },
         { "remove_track", &Shell::removeTrack_ },
         { "show_track", &Shell::showTrack_ },
         { "show_list", &Shell::showPlaylist_ },
         { "play", &Shell::play_ },
         { "pause", &Shell::pause_ },
         { "prev", &Shell::previous_ },
         { "next", &Shell::next_ },
         { "random", &Shell::random_ },
         { "repeat", &Shell::repeat_ },
         { "remove_dupes", &Shell::removeDuplicates_ },
         { "current_directory", &Shell::cd_ },
         { "load", &Shell::loadPlaylist_ },
         { "save", &Shell::savePlaylist_ },
      };

      std::random_device rd;
      rng_.seed(rd());

      currently_playing_ = playlist_.end();
   }

   Shell::Shell(std::istream& in, std::ostream& out) :
      Shell()
   {
      setInputStream(in);
      setOutputStream(out);
   }

#pragma endregion

#pragma region Instructions

   /**
    * \brief Prints the list of available instructions, or if provided with an argument, the description of a specific instruction.
    *
    * \param args Empty or containing the name of the instruction to get the detailed documentation of.
    */
   void Shell::help_(const Shell::ArgumentArray& args)
   {
      if (args.empty())
      {
         *output_ << "Available instructions: " << endl;
         for (const auto& instruction : available_instructions_)
         {
            *output_ << instruction.first << endl;
         }

         *output_ << "Type \"help <instruction>\" to get more info about a specific command." << endl;
      }
      else
      {
         *output_ << HelpMessages::forInstruction(args[0]) << endl;
      }
   }

   /**
    * No-op instruction called on empty input.
    *
    * \param Unused.
    */
   void Shell::noop_(const Shell::ArgumentArray&)
   {
   }

   /**
    * Error handler called on unknown instruction inputted.
    *
    * \param args The unknown instruction that was inputted.
    */
   void Shell::unknownInstruction_(const Shell::ArgumentArray& args)
   {
      *output_ << "Unknown instruction: " << args[0] << endl;
   }

   void Shell::addTrack_(const Shell::ArgumentArray& args)
   {
      for (const string& file_name : args)
      {
         std::ifstream file(file_name, std::ifstream::in);

         if (!file.is_open())
         {
            *output_ << "File \"" << file_name << "\" could not be opened." << endl;
            continue;
         }

         std::ostringstream strm;
         strm << file.rdbuf();
         file.close();

         Track new_track;

         if (!new_track.deserialize(strm.str()))
         {
            *output_ << "File \"" << file_name << "\" was not imported. (Reason: " << new_track.getErrorMessage() << ")" << endl;
            continue;
         }

         playlist_.push_back({ file_name, new_track });

         *output_ << "File \"" << file_name << "\" was successfully added in position " << playlist_.size() << "." << endl;

         // FEAT: add message for duplicate addition

         if (playlist_.size() == 1)
         {
            currently_playing_ = playlist_.begin();
         }
      }
   }

   void Shell::removeTrack_(const Shell::ArgumentArray& args)
   {
      if (args.empty())
      {
         *output_ << "Please specify at least one track name or position in the playlist to remove." << endl;
         return;
      }

      std::set<int> indices_to_remove = parseIndicesFromArgs_(args);

      int idx(0);
      Playlist::iterator it(playlist_.begin());
      while (it != playlist_.end())
      {
         if (indices_to_remove.count(idx))
         {
            bool playing_track_is_being_removed(currently_playing_ == it);

            it = playlist_.erase(it);
            
            if (playing_track_is_being_removed)
            {
               currently_playing_ = it;
               if (currently_playing_ == playlist_.end())
                  is_playing_ = false;
            }
         }
         else
         {
            it++;
         }

         idx++;
      }
   }

   void Shell::removeDuplicates_(const ArgumentArray& args)
   {
      std::set<string> filenames;

      Playlist::iterator current = playlist_.begin();
      while (current != playlist_.end())
      {
         if (filenames.count(current->first))
         {
            bool currentlyPlayingTrackIsRemoved(current == currently_playing_);
            current = playlist_.erase(current);
            if (currentlyPlayingTrackIsRemoved)
               currently_playing_ = current;
         }
         else
         {
            filenames.insert(current->first);
            current++;
         }
      }
   }

   void Shell::showTrack_(const ArgumentArray& args)
   {
      if (playlist_.empty())
      {
         *output_ << "There is no track yet to show in the playlist." << endl;
         return;
      }

      if (args.empty())
      {
         // no argument: show currently playing (if existing)
         if (currently_playing_ != playlist_.end())
         {
            *output_ << "Now playing: " << (is_playing_ ? "" : " (paused)") << endl;
            *output_ << "Track " << (std::distance(playlist_.begin(), currently_playing_) + 1) << "(" << playlist_.size() << ")" << endl;
            *output_ << Track::longFormat << currently_playing_->second;
         }
      }
      else
      {
         // only show one track at a time
         std::set<int> indices_to_show = parseIndicesFromArgs_(args);

         Playlist::iterator current_entry(playlist_.begin());
         std::unordered_map<string, int> tracks_shown;

         for (int idx = 0; idx < playlist_.size(); idx++, current_entry++)
         {
            if (indices_to_show.count(idx))
            {
               *output_ << "#" << idx + 1 << ": [" << current_entry->first << "]";
               
               if (current_entry == currently_playing_)
                  *output_ << (is_playing_ ? " (now playing)" : " (paused)");
                
               *output_ << endl << endl;

               if (!tracks_shown.count(current_entry->first))
               {
                  *output_ << Track::longFormat << current_entry->second;
                  tracks_shown.emplace(current_entry->first, idx + 1);
               }
               else
               {
                  *output_ << "(duplicate from track #" << tracks_shown[current_entry->first] << ")" << endl;
               }

               *output_ << endl;
            }
         }
      }
   }

   void Shell::showPlaylist_(const ArgumentArray& args)
   {
      *output_ << "Random mode: " << (random_mode_ ? "on" : "off") << endl;
      *output_ << "Repeat mode: " << (repeat_mode_ ? "on" : "off") << endl << endl;

      int idx = 1;
      for (const auto& entry : playlist_)
      {
         *output_ << idx++ << ") ";

         if (currently_playing_ != playlist_.end() && &*currently_playing_ == &entry)
         {
            *output_ << (is_playing_ ? "[|>]" : "[||]") << " ";
         }

         *output_ << Track::shortFormat << entry.second << " [" << entry.first << "]" << endl;
      }
   }

   void Shell::play_(const ArgumentArray&)
   {
      if (currently_playing_ != playlist_.end())
      {
         is_playing_ = true;
      }
      else
      {
         *output_ << "No track in playlist yet!" << endl;
      }
   }

   void Shell::pause_(const ArgumentArray&)
   {
      if (currently_playing_ != playlist_.end())
      {
         is_playing_ = false;
      }
      else
      {
         *output_ << "No track in playlist yet!" << endl;
      }
   }

   void Shell::previous_(const ArgumentArray& args)
   {
      if (currently_playing_ == playlist_.end())
      {
         *output_ << "No track in playlist yet!" << endl;
         return;
      }

      if (random_mode_)
      {
         goToRandomTrack_();
         return;
      }

      size_t number_of_jumps;

      if (args.empty())
      {
         number_of_jumps = 1;
      }
      else
      {
         try
         {
            size_t pos;
            number_of_jumps = std::stoi(args[0], &pos);
            if (pos != args[0].size())
               number_of_jumps = -1;
         }
         catch (std::exception)
         {
            number_of_jumps = -1;
         }
      }

      if (number_of_jumps < 0)
      {
         // error
         *output_ << "Only positive integral numbers are allowed as arguments for this command." << endl;
         return;
      }

      if (std::distance(playlist_.begin(), currently_playing_) < number_of_jumps)
      {
         if (!repeat_mode_)
         {
            currently_playing_ = playlist_.begin();
         }
         else
         {
            size_t excess_number = number_of_jumps - std::distance(playlist_.begin(), currently_playing_);

            excess_number %= playlist_.size();

            currently_playing_ = std::prev(playlist_.end(), excess_number);
         }
      }
      else
      {
         currently_playing_ = std::prev(currently_playing_, number_of_jumps);
      }
   }

   void Shell::next_(const ArgumentArray& args)
   {
      if (currently_playing_ == playlist_.end())
      {
         *output_ << "No track in playlist yet!" << endl;
         return;
      }

      if (random_mode_)
      {
         goToRandomTrack_();
         return;
      }

      size_t number_of_jumps;

      if (args.empty())
      {
         number_of_jumps = 1;
      }
      else
      {
         try
         {
            size_t pos;
            number_of_jumps = std::stoi(args[0], &pos);
            if (pos != args[0].size())
               number_of_jumps = -1;
         }
         catch (std::exception)
         {
            number_of_jumps = -1;
         }
      }

      if (number_of_jumps < 0)
      {
         // error
         *output_ << "Only positive integral numbers are allowed as arguments for this command." << endl;
         return;
      }

      if (std::distance(currently_playing_, std::prev(playlist_.end())) < number_of_jumps)
      {
         if (!repeat_mode_)
         {
            currently_playing_ = std::prev(playlist_.end());
         }
         else
         {
            size_t excess_number = number_of_jumps - std::distance(currently_playing_, playlist_.end());

            excess_number %= playlist_.size();

            currently_playing_ = std::next(playlist_.begin(), excess_number);
         }
      }
      else
      {
         currently_playing_ = std::next(currently_playing_, number_of_jumps);
      }
   }

   void Shell::random_(const ArgumentArray&)
   {
      random_mode_ = true;
      *output_ << "Random mode on." << endl;
   }

   void Shell::repeat_(const ArgumentArray&)
   {
      repeat_mode_ = true;
      *output_ << "Repeat mode on." << endl;
   }

   void Shell::cd_(const ArgumentArray& args)
   {
      if (args.empty())
      {
         *output_ << std::filesystem::current_path() << endl;
      }
      else
      {
         auto target_path = std::filesystem::current_path() / args[0];
         if (std::filesystem::exists(target_path))
         {
            std::filesystem::current_path(target_path);
            *output_ << "Moved to " << target_path << endl;
         }
         else
         {
            *output_ << "The path " << target_path << " does not exist." << endl;
         }
      }
   }

   void Shell::loadPlaylist_(const ArgumentArray& arg)
   {
      if (arg.size() != 1)
      {
         *output_ << "This command only accept one argument." << endl;
         return;
      }
      
      std::ifstream file(arg[0], std::ifstream::in);

      if (!file.is_open())
      {
         *output_ << "File \"" << arg[0] << "\" could not be opened." << endl;
         return;
      }

     string track_record;
     while (std::getline(file, track_record))
     {
         ArgumentArray splitted = split(track_record, "||");
         string track_file = splitted[0];
         string track_infos = splitted[1];

         Track new_track;
         new_track.deserialize(track_infos);

         playlist_.push_back({track_file, new_track});
     }

     file.close();
   }

   void Shell::savePlaylist_(const ArgumentArray& args)
   {
      if (args.size() != 1)
      {
         *output_ << "This command only accept one argument." << endl;
         return;
      }
      
      std::ofstream file(args[0], std::ofstream::out | std::ofstream::trunc);

      if (!file.is_open())
      {
         *output_ << "File \"" << args[0] << "\" could not be opened." << endl;
         return;
      }

      for(auto& entry: playlist_) {
         const Track& track_to_save = entry.second;
         file << entry.first << "||" << track_to_save.serialize() << endl;
      }

      file.close();
   }

#pragma endregion

#pragma region General methods

   void Shell::run()
   {
      if (!input_ || !output_)
         return;

      printWelcomeMessage_();

      bool exit_requested(false);
      while (!exit_requested)
      {
         Instruction submitted;
         ArgumentArray arguments;

         std::tie(submitted, arguments) = getInstruction_();

         try
         {
            submitted(this, arguments);
         }
         catch (std::exception& ex)
         {
            *output_ << "ERROR: " << ex.what() << std::endl;
         }
      }
   }

   void Shell::printWelcomeMessage_()
   {
      if (!output_)
         return;

      *output_ << "Music Player version " << VERSION_STRING << endl;
      *output_ << "Print help with command: " << "help" << endl;
   }

   std::tuple<Shell::Instruction, Shell::ArgumentArray> Shell::getInstruction_()
   {
      *output_ << ">>>> ";

      std::string full_input;
      std::getline(*input_, full_input);

      vector<string> parsed = split(full_input, " ");

      if (parsed.empty())
         // noop
         return std::make_tuple(&Shell::noop_, ArgumentArray());

      if (available_instructions_.count(parsed[0]))
      {
         return std::make_tuple(
            available_instructions_.at(parsed[0]),
            ArgumentArray(parsed.begin() + 1, parsed.end())
         );
      }
      else
      {
         // instruction not found among the available ones
         // call unknown instruction handler with submitted command as argument
         return std::make_tuple(
            &Shell::unknownInstruction_,
            ArgumentArray(1, parsed[0])
         );
      }
   }

   /**
    * Parses a list of number or string arguments into indices of tracks in the playlist.
    * 
    * \param The argument list.
    * \return A set of indices containing all tracks referenced in the argument list.
    */
   std::set<int> Shell::parseIndicesFromArgs_(const ArgumentArray& args)
   {
      std::set<int> found_indices{};

      for (const string& arg : args)
      {
         // find out if argument is number-like
         int index;

         try
         {
            size_t pos;
            index = std::stoi(arg, &pos);
            if (pos != arg.size())
               index = -1;
         }
         catch (std::exception)
         {
            index = -1;
         }

         if (index > 0)
         {
            // argument is number-like

            if (index > playlist_.size())
            {
               *output_ << "Impossible to find track at position " << index << " : "
                  << "there are only " << playlist_.size() << " elements in the playlist." << endl;
               continue;
            }

            found_indices.insert(index - 1);
         }
         else
         {
            auto match_on_filename = [&arg](const std::pair<string, Track>& entry) -> bool { return entry.first == arg; };

            // argument is file name
            Playlist::iterator match = std::find_if(playlist_.begin(), playlist_.end(), match_on_filename);
            if (match == playlist_.end())
            {
               *output_ << "Impossible to find track \"" << arg << "\": the track doesn't exist in the playlist." << endl;
               continue;
            }

            while (match != playlist_.end())
            {
               found_indices.insert(std::distance(playlist_.begin(), match));
               match = std::find_if(std::next(match), playlist_.end(), match_on_filename);
            }
         }
      }

      return found_indices;
   }

   void Shell::goToRandomTrack_()
   {
      std::uniform_int_distribution<> distrib(1, playlist_.size());
      int next_track_index = distrib(rng_) - 1;

      currently_playing_ = std::next(playlist_.begin(), next_track_index);
      *output_ << "Moved to track #" << next_track_index + 1 << endl;
   }

#pragma endregion
}