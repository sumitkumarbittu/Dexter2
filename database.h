#pragma once
#include <string>
#include <vector>

struct CompressionRecord {
    std::string originalFile;
    std::string compressedFile;
    size_t originalSize;
    size_t compressedSize;
    double ratio;
    double spaceSaving;
};

class CompressionDatabase {
public:
    CompressionDatabase(const std::string& dbPath);
    ~CompressionDatabase();
    
    bool isOpen() const;
    void saveCompressionStats(
        const std::string& originalFile,
        const std::string& compressedFile,
        size_t originalSize,
        size_t compressedSize,
        double ratio,
        double spaceSaving
    );
    std::vector<CompressionRecord> getHistory() const;
    void clearHistory();

private:
    void* db_; // Use sqlite3* in actual implementation
};