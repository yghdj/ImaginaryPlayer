#include "Help.h"

#include <cstdarg>
#include <sstream>

using std::string;
using std::endl;

//        { "help", &Shell::help_ },
//        { "add_track", &Shell::addTrack_ },
//        { "remove_track", &Shell::removeTrack_ },
//        { "show_track", &Shell::showTrack_ },
//        { "show_list", &Shell::showPlaylist_ },
//        { "play", &Shell::play_ },
//        { "pause", &Shell::pause_ },
//        { "prev", &Shell::previous_ },
//        { "next", &Shell::next_ },
//        { "random", &Shell::random_ },
//        { "repeat", &Shell::repeat_ },
//        { "remove_dupes", &Shell::removeDuplicates_ },
//        { "change_directory", &Shell::cd_ },
//        { "load", &Shell::loadPlaylist_ }

namespace {
    void addUsage(std::stringstream& out, const char* command, const int lines, ...) {
        out << "\t" << command << endl;

        va_list args;
        va_start(args, lines);

        for(int i = 0; i < lines; i++) {
            out << "\t\t" << va_arg(args, const char*) << endl;
        }

        va_end(args);
    }

    void addUsage(std::stringstream& out, const char* command, const char* detailed_help) {
        addUsage(out, command, 1, detailed_help);
    }
}

namespace MusicPlayer {
    string HelpMessages::forInstruction(const string& instruction) {
        std::stringstream message_builder;

        message_builder << "Usage:" << endl;

        if(instruction == "help")
        {
            addUsage(message_builder, "help", "Prints the list of all available commands.");
            addUsage(message_builder, "help <command>", "Prints a help message for the provided command name.");
        }
        else if(instruction == "add_track")
        {
            addUsage(
                message_builder,
                "add_track <track 1 file name> [<track 2 file name> ...]",
                2,
                "Adds one or multiple track(s) at the end of the playlist.",
                "The file names provided can be paths, and must be without whitespaces."
            );
        }
        else if(instruction == "remove_track") {
            addUsage(message_builder, "remove_track <track file name> [<track file name> ...]", "Removes all tracks imported from the file name(s) specified.");
            addUsage(message_builder, "remove_track <track position> [<track position> ...]", "Removes the track located at the specified position(s) in the playlist.");
        }
        else if(instruction == "show_track") {
            addUsage(message_builder, "show_track", "Prints detailed infos about the currently selected track.");
            addUsage(message_builder, "show_track <track name or position> [<track name or position> ...]", "Prints detailed infos about the track in the playlist with the specified file name, or at the specified position.");
        }
        else if(instruction == "show_list") {
            addUsage(message_builder, "show_list", "Prints the playlist contents.");
        }
        else if(instruction == "play") {
            addUsage(message_builder, "play", "Plays the currently selected track.");
        }
        else if(instruction == "pause") {
            addUsage(message_builder, "pause", "Pauses the currently playing track.");
        }
        else if(instruction == "prev") {
            addUsage(message_builder, "prev", "Changes the selected track to the previous one on the list.");
            addUsage(message_builder, "prev <number>", "Rewinds the playlist to N tracks before the currently selected one.");
        }
        else if(instruction == "next") {
            addUsage(message_builder, "next", "Changes the selected track to the next one on the list.");
            addUsage(message_builder, "next <number>", "Fast forwards the playlist to N tracks after the currently selected one.");
        }
        else if(instruction == "random") {
            addUsage(message_builder, "random", "Toggles random mode: the tracks will be played in random order.");
        }
        else if(instruction == "repeat") {
            addUsage(message_builder, "repeat", "Toggles repeat mode: when going past one end of the playlist, the playback will move to the other end of the playlist.");
        }
        else if(instruction == "remove_dupes") {
            addUsage(message_builder, "remove_dupes", "Removes duplicated tracks in the playlist, keeping only the first occurence.");
        }
        else if(instruction == "current_directory") {
            addUsage(message_builder, "current_directory", "Displays the current directory.");
            addUsage(message_builder, "current_directory <path>", "Changes the current directory to the requested location.");
        }
        else if(instruction == "load") {
            addUsage(message_builder, "load <playlist file>", "Loads a playlist from a *.playlist file.");
        }
        else if(instruction == "save") {
            addUsage(message_builder, "save <path>", "Saves a playlist to a file on disk.");
        }
        else {
            message_builder << "This instruction has no documentation yet." << endl;
        }

        return message_builder.str();
    }
}