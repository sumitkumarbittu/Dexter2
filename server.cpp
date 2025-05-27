#include "cpp-httplib/httplib.h"
#include "nlohmann/json.hpp"
#include <string>
#include <vector>
#include <unordered_map>
#include <memory>
#include <fstream>
#include <sstream>
#include <iomanip>
#include <iostream>
#include <cstdlib>

using namespace std;
using namespace httplib;
using json = nlohmann::json;

class LZWTrieCompressor {
public:
    LZWTrieCompressor() { resetDictionary(); }

    vector<int> compress(const vector<string>& lines) {
        resetDictionary();
        vector<int> compressed;
        string current_sequence;

        for (const auto& line : lines) {
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

    vector<string> decompress(const vector<int>& compressed) {
        if (compressed.empty()) return {};

        unordered_map<int, string> dictionary;
        for (int i = 0; i < 256; i++) {
            dictionary[i] = string(1, static_cast<char>(i));
        }

        string result;
        int prev_code = compressed[0];
        result = dictionary[prev_code];
        int next_code = 256;

        for (size_t i = 1; i < compressed.size(); i++) {
            int code = compressed[i];
            string entry;

            if (dictionary.count(code)) {
                entry = dictionary[code];
            } else if (code == next_code) {
                entry = dictionary[prev_code] + dictionary[prev_code][0];
            } else {
                throw runtime_error("Invalid compressed code");
            }

            result += entry;
            dictionary[next_code++] = dictionary[prev_code] + entry[0];
            prev_code = code;
        }

        vector<string> lines;
        stringstream ss(result);
        string line;
        while (getline(ss, line)) {
            lines.push_back(line);
        }
        return lines;
    }

private:
    struct TrieNode {
        int code;
        unordered_map<char, unique_ptr<TrieNode>> children;
        TrieNode() : code(-1) {}
    };

    unique_ptr<TrieNode> root;
    int next_code;

    void resetDictionary() {
        root = make_unique<TrieNode>();
        next_code = 0;
        for (int i = 0; i < 256; i++) {
            string s(1, static_cast<char>(i));
            addToTrie(s);
        }
    }

    bool trieContains(const string& sequence) {
        TrieNode* current = root.get();
        for (char c : sequence) {
            if (current->children.find(c) == current->children.end()) return false;
            current = current->children[c].get();
        }
        return true;
    }

    int getCode(const string& sequence) {
        TrieNode* current = root.get();
        for (char c : sequence) {
            current = current->children[c].get();
        }
        return current->code;
    }

    void addToTrie(const string& sequence) {
        TrieNode* current = root.get();
        for (char c : sequence) {
            if (!current->children.count(c)) {
                current->children[c] = make_unique<TrieNode>();
            }
            current = current->children[c].get();
        }
        current->code = next_code++;
    }
};

string readFile(const string& path) {
    ifstream file(path);
    stringstream buffer;
    buffer << file.rdbuf();
    return buffer.str();
}

int main() {
    Server svr;
    LZWTrieCompressor compressor;

    // Serve HTML page
    svr.Get("/", [](const Request&, Response& res) {
        res.set_content(readFile("index.html"), "text/html");
    });

    // Compress route
    svr.Post("/compress", [&](const Request& req, Response& res) {
        try {
            json j = json::parse(req.body);
            string content = j.at("content");
            stringstream ss(content);
            string line;
            vector<string> lines;
            while (getline(ss, line)) lines.push_back(line);

            vector<int> compressed = compressor.compress(lines);
            ostringstream out;
            for (int n : compressed) out << n << '\n';
            res.set_content(out.str(), "text/plain");
        } catch (...) {
            res.status = 400;
            res.set_content("Compression failed", "text/plain");
        }
    });

    // Decompress route
    svr.Post("/decompress", [&](const Request& req, Response& res) {
        try {
            json j = json::parse(req.body);
            string content = j.at("content");
            stringstream ss(content);
            string line;
            vector<int> codes;
            while (getline(ss, line)) {
                codes.push_back(stoi(line));
            }

            vector<string> decompressed = compressor.decompress(codes);
            ostringstream out;
            for (const string& l : decompressed) out << l << '\n';
            res.set_content(out.str(), "text/plain");
        } catch (...) {
            res.status = 400;
            res.set_content("Decompression failed", "text/plain");
        }
    });

    int port = getenv("PORT") ? stoi(getenv("PORT")) : 8080;
    cout << "Listening on port " << port << endl;
    svr.listen("0.0.0.0", port);
}
