#include "lzw_compressor.h"
#include <stdexcept>
#include <string>
#include <unordered_map>
#include <memory>

using namespace std;

LZWTrieCompressor::LZWTrieCompressor() {
    resetDictionary();
}

vector<int> LZWTrieCompressor::compress(const vector<string>& lines) {
    resetDictionary();
    vector<int> compressed;
    string current_sequence;

    // Process each line
    for (const auto& line : lines) {
        // Process each character in the line
        for (char c : line) {
            string test_sequence = current_sequence + c;

            if (trieContains(test_sequence)) {
                current_sequence = test_sequence;
            } else {
                if (!current_sequence.empty()) {
                    compressed.push_back(getCode(current_sequence));
                }
                addToTrie(test_sequence);
                current_sequence = string(1, c);
            }
        }
        
        // Handle line ending
        string test_sequence = current_sequence + "\n";
        if (trieContains(test_sequence)) {
            current_sequence = test_sequence;
        } else {
            if (!current_sequence.empty()) {
                compressed.push_back(getCode(current_sequence));
            }
            addToTrie(test_sequence);
            current_sequence = "\n";
        }
    }

    if (!current_sequence.empty()) {
        compressed.push_back(getCode(current_sequence));
    }

    return compressed;
}

vector<string> LZWTrieCompressor::decompress(const vector<int>& compressed) {
    if (compressed.empty()) return {};

    unordered_map<int, string> dictionary;
    for (int i = 0; i < 256; i++) {
        dictionary[i] = string(1, static_cast<char>(i));
    }

    string result;
    int prev_code = compressed[0];
    if (!dictionary.count(prev_code)) {
        throw runtime_error("Invalid starting code");
    }
    result = dictionary[prev_code];

    int next_code = 256;
    for (size_t i = 1; i < compressed.size(); i++) {
        int code = compressed[i];
        string entry;

        if (dictionary.count(code)) {
            entry = dictionary[code];
        }
        else if (code == next_code) {
            entry = dictionary[prev_code] + dictionary[prev_code][0];
        }
        else {
            throw runtime_error("Invalid compressed code");
        }

        result += entry;

        dictionary[next_code] = dictionary[prev_code] + entry[0];

        next_code++;
        prev_code = code;
    }

    // Split the result into lines
    vector<string> lines;
    string line;
    for (char c : result) {
        if (c == '\n') {
            lines.push_back(line);
            line.clear();
        } else {
            line += c;
        }
    }
    if (!line.empty()) {
        lines.push_back(line);
    }

    return lines;
}

void LZWTrieCompressor::resetDictionary() {
    root = make_unique<TrieNode>();
    next_code = 0;
    for (int i = 0; i < 256; i++) {
        string s(1, static_cast<char>(i));
        addToTrie(s);
    }
}

bool LZWTrieCompressor::trieContains(const string& sequence) {
    if (sequence.empty()) return false;
    
    TrieNode* current = root.get();
    for (char c : sequence) {
        if (current->children.find(c) == current->children.end()) {
            return false;
        }
        current = current->children[c].get();
    }
    return true;
}

int LZWTrieCompressor::getCode(const string& sequence) {
    if (sequence.empty()) {
        throw runtime_error("Empty sequence in getCode");
    }

    TrieNode* current = root.get();
    for (size_t i = 0; i < sequence.size(); i++) {
        char c = sequence[i];
        if (current->children.find(c) == current->children.end()) {
            throw runtime_error("Sequence not in trie");
        }
        if (i == sequence.size() - 1) {
            return current->children[c]->code;
        }
        current = current->children[c].get();
    }
    throw runtime_error("Unexpected error in getCode");
}

void LZWTrieCompressor::addToTrie(const string& sequence) {
    if (sequence.empty()) return;

    TrieNode* current = root.get();
    for (size_t i = 0; i < sequence.size(); i++) {
        char c = sequence[i];
        if (current->children.find(c) == current->children.end()) {
            current->children[c] = make_unique<TrieNode>();
        }
        if (i == sequence.size() - 1) {
            current->children[c]->code = next_code++;
        }
        current = current->children[c].get();
    }
}