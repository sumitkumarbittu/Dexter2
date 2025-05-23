#pragma once
#include <vector>
#include <string>
#include <unordered_map>
#include <memory>

class LZWTrieCompressor {
public:
    LZWTrieCompressor();
    std::vector<int> compress(const std::vector<std::string>& lines);
    std::vector<std::string> decompress(const std::vector<int>& compressed);

private:
    struct TrieNode {
        int code;
        std::unordered_map<char, std::unique_ptr<TrieNode>> children;
        TrieNode() : code(-1) {}
    };

    std::unique_ptr<TrieNode> root;
    int next_code;

    void resetDictionary();
    bool trieContains(const std::string& sequence);
    int getCode(const std::string& sequence);
    void addToTrie(const std::string& sequence);
};