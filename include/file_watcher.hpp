

#pragma once
#include <filesystem>
#include <chrono>
#include <unordered_map>
#include <string>
#include <functional>

// Define available file changes
enum class FileStatus {created, modified, erased};

//modified fileWatcher class from https://solarianprogrammer.com/2019/01/13/cpp-17-filesystem-write-file-watcher-monitor/
 class FileWatcher {
    public: 
    std::string pathToWatch{};
    int delay;

    FileWatcher(std::string pathToWatch, int delay) : pathToWatch{pathToWatch}, delay{delay} {
        for(auto &file : std::filesystem::recursive_directory_iterator(pathToWatch)) {
            paths_[file.path().string()] = std::filesystem::last_write_time(file);
        }
    }
    
    void setStandardResponse(){
       auto act = [] (std::string path_to_watch, FileStatus status) -> void {
        // Process only regular files, all other file types are ignored
        if(!std::filesystem::is_regular_file(std::filesystem::path(path_to_watch)) && status != FileStatus::erased) {
            return;
        }

        switch(status) {
            case FileStatus::created:
                std::cout << "File created: " << path_to_watch << '\n';
                break;
            case FileStatus::modified:
                std::cout << "File modified: " << path_to_watch << '\n';
                break;
            case FileStatus::erased:
                std::cout << "File erased: " << path_to_watch << '\n';
                break;
            default:
                std::cout << "Error! Unknown file status.\n";
            }};
        
        
        action = act;
    }
  
    void checkDirectoryPeriodically() {
        static auto last_check = std::chrono::steady_clock::now();
        auto now = std::chrono::steady_clock::now();

        // The Gatekeeper: Return immediately if 5 seconds haven't passed
        if (std::chrono::duration_cast<std::chrono::seconds>(now - last_check).count() < delay) {
            return; 
        }
        // Reset the clock for the next interval
        last_check = now;

        auto it = paths_.begin();
        while (it != paths_.end()) {
            if (!std::filesystem::exists(it->first)) {
                action(it->first, FileStatus::erased);
                it = paths_.erase(it);
            }
            else {
                it++;
            }
        }

        // Check if a file was created or modified
        for(auto &file : std::filesystem::recursive_directory_iterator(pathToWatch)) {
            auto current_file_last_write_time = std::filesystem::last_write_time(file);

            // File creation
            if(!contains(file.path().string())) {
                paths_[file.path().string()] = current_file_last_write_time;
                action(file.path().string(), FileStatus::created);
            // File modification
            } else {
                if(paths_[file.path().string()] != current_file_last_write_time) {
                    paths_[file.path().string()] = current_file_last_write_time;
                    action(file.path().string(), FileStatus::modified);
                }
            }
        }
    }

    private:
    std::unordered_map<std::string, std::filesystem::file_time_type> paths_;
    bool contains(const std::string &key) {
        auto el = paths_.find(key);
        return el != paths_.end();
    }
    std::function<void (std::string, FileStatus)> action{};
    
};

 