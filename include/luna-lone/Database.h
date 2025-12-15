#pragma once
#include <string>
#include <vector>
#include <sqlite3.h>
#include "DataTypes.h"

class Database {
public:
    Database(const std::string& dbPath);
    ~Database();

    bool init();
    bool addAudioEntry(const std::string &displayName, const std::string &filePahth);

    bool addTagEntry(const std::string &tagName);
    int getTagId(const std::string &tagName);

    bool linkTagToAudio(int audioId, int tagId);
    std::vector<TagEntry> getTagsForAudio(int audioId);
    std::string getAudioFilePath(int audioId);

    std::vector<AudioEntry> getAllAudioEntries();
    std::vector<TagEntry> getAllTagEntries();
    std::vector<std::pair<AudioEntry, int>> getWeightedAudioMatches(const std::vector<int> &tagIds);
    std::pair<bool, AudioEntry> chooseBestWeightedMatch(const std::vector<int> &tagIds);

    bool deleteAudioEntry(int audioId);
    bool deleteTagEntry(int tagId);

    bool clearAudioTags(int audioId);

private:
    sqlite3* db = nullptr;
};