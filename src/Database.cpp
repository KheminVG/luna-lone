#include "Database.h"
#include <fstream>
#include <sstream>
#include <iostream>
#include <random>

Database::Database(const std::string& dbPath) 
{
    if (sqlite3_open(dbPath.c_str(), &db) != SQLITE_OK) {
        std::cerr << "Failed to open database: " << sqlite3_errmsg(db) << std::endl;
        db = nullptr;
    }
}

Database::~Database() 
{
    if (db) { sqlite3_close(db); }
}

bool Database::init() 
{
    if (!db) return false;

    std::ifstream schema("data/db/schema/database.sql");
    
    if (!schema.is_open()) {
        std::cerr << "Failed to open database schema file." << std::endl;
        return false;
    }

    std::stringstream buffer;
    buffer << schema.rdbuf();
    std::string sql = buffer.str();
    schema.close();

    char *errMsg = nullptr;
    if (sqlite3_exec(db, sql.c_str(), nullptr, nullptr, &errMsg) != SQLITE_OK) {
        std::cerr << "Failed to initialize database schema: " << errMsg << std::endl;
        sqlite3_free(errMsg);
        return false;
    }

    return true;
}

bool Database::addAudioEntry(const std::string &displayName, const std::string &filePath) 
{
    if (!db) return false;

    const char *sql = "INSERT INTO audio_files (display_name, file_path) VALUES (?, ?);";
    sqlite3_stmt *stmt = nullptr;

    if (sqlite3_prepare_v2(db, sql, -1, &stmt, nullptr) != SQLITE_OK) {
        std::cerr << "Failed to prepare statement: " << sqlite3_errmsg(db) << std::endl;
        return false;
    }

    sqlite3_bind_text(stmt, 1, displayName.c_str(), -1, SQLITE_STATIC);
    sqlite3_bind_text(stmt, 2, filePath.c_str(), -1, SQLITE_STATIC);

    if (sqlite3_step(stmt) != SQLITE_DONE) {
        std::cerr << "Failed to execute statement: " << sqlite3_errmsg(db) << std::endl;
        sqlite3_finalize(stmt);
        return false;
    }

    sqlite3_finalize(stmt);
    return true;
}

bool Database::addTagEntry(const std::string &tagName) 
{
    if (!db) return false;

    const char *sql = "INSERT INTO tags (name) VALUES (?);";
    sqlite3_stmt *stmt = nullptr;

    if (sqlite3_prepare_v2(db, sql, -1, &stmt, nullptr) != SQLITE_OK) {
        std::cerr << "Failed to prepare statement: " << sqlite3_errmsg(db) << std::endl;
        return false;
    }

    sqlite3_bind_text(stmt, 1, tagName.c_str(), -1, SQLITE_STATIC);

    if (sqlite3_step(stmt) != SQLITE_DONE) {
        std::cerr << "Failed to execute statement: " << sqlite3_errmsg(db) << std::endl;
        sqlite3_finalize(stmt);
        return false;
    }

    sqlite3_finalize(stmt);
    return true;
}

int Database::getTagId(const std::string &tagName) 
{
    if (!db) return -1;

    const char *sql = "SELECT id FROM tags WHERE name = ?;";
    sqlite3_stmt *stmt = nullptr;
    int tagId = -1;

    if (sqlite3_prepare_v2(db, sql, -1, &stmt, nullptr) != SQLITE_OK) {
        std::cerr << "Failed to prepare statement: " << sqlite3_errmsg(db) << std::endl;
        return -1;
    }

    sqlite3_bind_text(stmt, 1, tagName.c_str(), -1, SQLITE_STATIC);

    if (sqlite3_step(stmt) == SQLITE_ROW) {
        tagId = sqlite3_column_int(stmt, 0);
    } else {
        std::cerr << "Tag not found: " << tagName << std::endl;
    }

    sqlite3_finalize(stmt);
    return tagId;
}

bool Database::linkTagToAudio(int audioId, int tagId) 
{
    if (!db) return false;

    const char *sql = "INSERT INTO audio_tags (audio_id, tag_id) VALUES (?, ?);";
    sqlite3_stmt *stmt = nullptr;

    if (sqlite3_prepare_v2(db, sql, -1, &stmt, nullptr) != SQLITE_OK) {
        std::cerr << "Failed to prepare statement: " << sqlite3_errmsg(db) << std::endl;
        return false;
    }

    sqlite3_bind_int(stmt, 1, audioId);
    sqlite3_bind_int(stmt, 2, tagId);

    if (sqlite3_step(stmt) != SQLITE_DONE) {
        std::cerr << "Failed to execute statement: " << sqlite3_errmsg(db) << std::endl;
        sqlite3_finalize(stmt);
        return false;
    }

    sqlite3_finalize(stmt);
    return true;
}

std::vector<TagEntry> Database::getTagsForAudio(int audioId) 
{
    std::vector<TagEntry> tags;
    if (!db) return tags;

    const char *sql = 
        "SELECT t.id, t.name FROM tags t "
        "JOIN audio_tags at ON t.id = at.tag_id "
        "WHERE at.audio_id = ?;";
    
    sqlite3_stmt *stmt = nullptr;

    if (sqlite3_prepare_v2(db, sql, -1, &stmt, nullptr) != SQLITE_OK) {
        std::cerr << "Failed to prepare statement: " << sqlite3_errmsg(db) << std::endl;
        return tags;
    }

    sqlite3_bind_int(stmt, 1, audioId);

    while (sqlite3_step(stmt) == SQLITE_ROW) {
        TagEntry entry;
        entry.id = sqlite3_column_int(stmt, 0);
        entry.tagName = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 1));
        tags.push_back(entry);
    }

    sqlite3_finalize(stmt);
    return tags;
}

std::vector<AudioEntry> Database::getAllAudioEntries()
{
    std::vector<AudioEntry> audioEntries;
    if (!db) return audioEntries;

    const char *sql = "SELECT id, display_name, file_path FROM audio_files ORDER BY display_name;";
    sqlite3_stmt *stmt = nullptr;

    if (sqlite3_prepare_v2(db, sql, -1, &stmt, nullptr) != SQLITE_OK) {
        std::cerr << "Failed to prepare statement: " << sqlite3_errmsg(db) << std::endl;
        return audioEntries;
    }

    while (sqlite3_step(stmt) == SQLITE_ROW) {
        AudioEntry entry;
        entry.id = sqlite3_column_int(stmt, 0);
        entry.displayName = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 1));
        entry.filePath = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 2));
        audioEntries.push_back(entry);
    }

    sqlite3_finalize(stmt);
    return audioEntries;
}

std::vector<TagEntry> Database::getAllTagEntries()
{
    std::vector<TagEntry> tagEntries;
    if (!db) return tagEntries;

    const char *sql = "SELECT id, name FROM tags ORDER BY name;";
    sqlite3_stmt *stmt = nullptr;

    if (sqlite3_prepare_v2(db, sql, -1, &stmt, nullptr) != SQLITE_OK) {
        std::cerr << "Failed to prepare statement: " << sqlite3_errmsg(db) << std::endl;
        return tagEntries;
    }

    while (sqlite3_step(stmt) == SQLITE_ROW) {
        TagEntry entry;
        entry.id = sqlite3_column_int(stmt, 0);
        entry.tagName = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 1));
        tagEntries.push_back(entry);
    }

    sqlite3_finalize(stmt);
    return tagEntries;
}

std::string Database::getAudioFilePath(int audioId)
{
    std::string filePath = "";
    if (!db) return filePath;

    const char *sql = "SELECT file_path FROM audio_files WHERE id = ?;";
    sqlite3_stmt *stmt = nullptr;

    if (sqlite3_prepare_v2(db, sql, -1, &stmt, nullptr) != SQLITE_OK) {
        std::cerr << "Failed to prepare statement:" << sqlite3_errmsg(db) << std::endl;
        return filePath;
    }

    sqlite3_bind_int(stmt, 1, audioId);

    if (sqlite3_step(stmt) == SQLITE_ROW) {
        filePath = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 0));
    } else {
        std::cerr << "File path not found." << std::endl;
    }

    sqlite3_finalize(stmt);
    return filePath;
}

std::vector<std::pair<AudioEntry, int>> Database::getWeightedAudioMatches(const std::vector<int> &tagIds)
{
    std::vector<std::pair<AudioEntry, int>> results;

    if (tagIds.empty()) return results;

    std::string sql = 
        "SELECT a.id, a.display_name, a.file_path, COUNT(at.tag_id) AS score "
        "FROM audio_files a "
        "JOIN audio_tags at ON a.id = at.audio_id "
        "WHERE at.tag_id IN (";

        for (size_t i = 0; i < tagIds.size(); ++i)
        {
            sql += "?";
            if (i + 1 < tagIds.size()) sql += ",";
        }

        sql += ") GROUP BY a.id ORDER BY score DESC;";

        sqlite3_stmt *stmt = nullptr;

        if (sqlite3_prepare_v2(db, sql.c_str(), -1, &stmt, nullptr) != SQLITE_OK)
        {
            std::cerr << "Failed to prepare statement:" << sqlite3_errmsg(db) << std::endl;
            return results;
        }

        int idx = 1;

        for (int tagId : tagIds)
        {
            if (sqlite3_bind_int(stmt, idx++, tagId) != SQLITE_OK)
            {
                std::cerr << "Failed to bind statement:" << sqlite3_errmsg(db) << std::endl;
                sqlite3_finalize(stmt);
                return results;
            }
        }

        while (sqlite3_step(stmt) == SQLITE_ROW) {
            AudioEntry entry;
            entry.id = sqlite3_column_int(stmt, 0);
            entry.displayName = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 1));
            entry.filePath = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 2));
            int score = sqlite3_column_int(stmt, 3);

            results.emplace_back(entry, score);
        }

        sqlite3_finalize(stmt);
        return results;
}

std::pair<bool, AudioEntry> Database::chooseBestWeightedMatch(const std::vector<int> &tagIds)
{
    AudioEntry match;
    bool matched = false;

    auto weighted = getWeightedAudioMatches(tagIds);
    if (weighted.empty()) return std::pair(matched, match);

    int best_score = weighted.front().second;

    std::vector<AudioEntry> ties;
    for (auto &pair : weighted) {
        if (pair.second == best_score)
            ties.push_back(pair.first);
        else
            break;
    }

    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_int_distribution<> dist(0, ties.size() - 1);

    matched = true;
    return std::pair(matched, ties[dist(gen)]);
}

bool Database::deleteAudioEntry(int audioId)
{
    if (!db) return false;

    std::string path;
    
    const char *selectSql = "SELECT file_path FROM audio_files WHERE id = ?;";
    sqlite3_stmt *selectStmt = nullptr;

    if (sqlite3_prepare_v2(db, selectSql, -1, &selectStmt, nullptr) != SQLITE_OK) {
        std::cerr << "Failed to prepare select statement: " << sqlite3_errmsg(db) << std::endl;
        return false;
    }

    sqlite3_bind_int(selectStmt, 1, audioId);

    if (sqlite3_step(selectStmt) == SQLITE_ROW) {
        path = reinterpret_cast<const char*>(sqlite3_column_text(selectStmt, 0));
    } else {
        std::cerr << "Audio entry not found. " << std::endl;
        sqlite3_finalize(selectStmt);
        return false;
    }

    sqlite3_finalize(selectStmt);

    const char *deleteSql = "DELETE FROM audio_files WHERE id = ?;";
    sqlite3_stmt *deleteStmt = nullptr;

    if (sqlite3_prepare_v2(db, deleteSql, -1, &deleteStmt, nullptr) != SQLITE_OK) {
        std::cerr << "Failed to prepare delete statement: " << sqlite3_errmsg(db) << std::endl;
        return false;
    }

    sqlite3_bind_int(deleteStmt, 1, audioId);

    if (sqlite3_step(deleteStmt) != SQLITE_DONE) {
        std::cerr << "Failed to execute delete statement: " << sqlite3_errmsg(db) << std::endl;
        sqlite3_finalize(deleteStmt);
        return false;
    }

    sqlite3_finalize(deleteStmt);

    if (!path.empty()) {
        if (std::remove(path.c_str()) != 0) {
            std::cerr << "Failed to delete audio file from filesystem: " << path << std::endl;
        }
    }

    return true;
}

bool Database::deleteTagEntry(int tagId)
{
    if (!db) return false;

    const char *sql = "DELETE FROM tags WHERE id = ?;";
    sqlite3_stmt *stmt = nullptr;

    if (sqlite3_prepare_v2(db, sql, -1, &stmt, nullptr) != SQLITE_OK) {
        std::cerr << "Failed to prepare statement: " << sqlite3_errmsg(db) << std::endl;
        return false;
    }

    sqlite3_bind_int(stmt, 1, tagId);

    if (sqlite3_step(stmt) != SQLITE_DONE) {
        std::cerr << "Failed to execute statement: " << sqlite3_errmsg(db) << std::endl;
        sqlite3_finalize(stmt);
        return false;
    }

    sqlite3_finalize(stmt);
    return true;
}

bool Database::clearAudioTags(int audioId)
{
    if (!db) return false;

    const char *deleteSql = "DELETE FROM audio_tags WHERE audio_id = ?;";
    sqlite3_stmt *deleteStmt = nullptr;

    if (sqlite3_prepare_v2(db, deleteSql, -1, &deleteStmt, nullptr) != SQLITE_OK) {
        std::cerr << "Failed to prepare statement: " << sqlite3_errmsg(db) << std::endl;
        return false;
    }

    sqlite3_bind_int(deleteStmt, 1, audioId);

    if (sqlite3_step(deleteStmt) != SQLITE_DONE) {
        std::cerr << "Failed to execute statement: " << sqlite3_errmsg(db) << std::endl;
        sqlite3_finalize(deleteStmt);
        return false;
    }

    sqlite3_finalize(deleteStmt);
    return true;
}
