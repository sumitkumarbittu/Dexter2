#include <iostream>
#include <string>
#include <vector>
#include <unordered_map>
#include <memory>
#include<fstream>
#include<iomanip>
#include <limits>
#include "database.h"

using namespace std;



void printFileStats(const string& originalFile, const string& compressedFile) {
    // Get original file size
    ifstream original(originalFile, ios::binary | ios::ate);
    if (!original) {
        cerr << "Error opening original file: " << originalFile << endl;
        return;
    }
    size_t originalSize = original.tellg();
    original.close();

    // Get compressed file size
    ifstream compressed(compressedFile, ios::binary | ios::ate);
    if (!compressed) {
        cerr << "Error opening compressed file: " << compressedFile << endl;
        return;
    }
    size_t compressedSize = compressed.tellg();
    compressed.close();

    double ratio = static_cast<double>(compressedSize) / originalSize;

    cout << "Original File: " << originalFile << endl;
    cout << "Original Size: " << originalSize << " bytes" << endl;
    cout << "Compressed File: " << compressedFile << endl;
    cout << "Compressed Size: " << compressedSize << " bytes" << endl;
    cout << "Compression Ratio: " << ratio << endl;
    cout << "Space Saving: " << (1.0 - ratio) * 100.0 << "%" << endl;
}


bool compareFiles(const string& file1, const string& file2) {
    ifstream f1(file1);
    ifstream f2(file2);
    
    if (!f1.is_open() || !f2.is_open()) {
        if (f1.is_open()) f1.close();
        if (f2.is_open()) f2.close();
        return false;
    }
    
    string line1, line2;
    
    while (getline(f1, line1) || getline(f2, line2)) {
        if (f1.eof() && !f2.eof()) {
            f1.close();
            f2.close();
            return false;
        }
        
        if (!f1.eof() && f2.eof()) {
            f1.close();
            f2.close();
            return false;
        }
        
        if (line1 != line2) {
            f1.close();
            f2.close();
            return false;
        }
    }
    
    f1.close();
    f2.close();
    return true;
}


vector<string> readAndPrintFile(const string& filename) {
    ifstream file(filename);
    
    // Check if file opened successfully
    if (!file.is_open()) {
        throw runtime_error("Error: Could not open file '" + filename + "'");
    }
    
    vector<string> lines;
    string line;
    
    cout << "Contents of '" << filename << "':\n";
    cout << "-----------------------------\n";
    
    while (getline(file, line)) {
        // Print the line
        cout << line << '\n';
        // Store the line
        lines.push_back(line);
    }
    
    // Check for errors during reading (other than EOF)
    if (!file.eof() && file.fail()) {
        throw runtime_error("Error: Problem reading file '" + filename + "'");
    }
    
    file.close();
    return lines;
}


void printAndWriteLinesString(const vector<string>& lines, const string& filename, bool showLineNumbers = true) {
    // 1. Print to console
    cout << "\nPrinting " << lines.size() << " lines:\n";
    cout << "--------------------------------\n";
    
    for (size_t i = 0; i < lines.size(); ++i) {
        if (showLineNumbers) {
            cout << setw(4) << (i + 1) << ": ";
        }
        cout << lines[i] << '\n';
    }
    cout << "--------------------------------\n";

    // 2. Write to file
    ofstream outFile(filename);
    if (!outFile.is_open()) {
        throw runtime_error("Error: Could not open file '" + filename + "' for writing");
    }

    for (const auto& line : lines) {
        outFile << line << '\n';
    }

    if (outFile.fail()) {
        outFile.close();
        throw runtime_error("Error: Failed to write to file '" + filename + "'");
    }

    outFile.close();
    cout << "Successfully wrote " << lines.size() 
         << " lines to file '" << filename << "'\n";
}


void printAndWriteLinesInt(const vector<int>& numbers, const string& filename, bool showLineNumbers = true) {
    // 1. Print to console
    cout << "\nPrinting " << numbers.size() << " numbers:\n";
    cout << "--------------------------------\n";
    
    for (size_t i = 0; i < numbers.size(); ++i) {
        if (showLineNumbers) {
            cout << setw(4) << (i + 1) << ": ";
        }
        cout << numbers[i] << '\n';
    }
    cout << "--------------------------------\n";

    // 2. Write to file
    ofstream outFile(filename);
    if (!outFile.is_open()) {
        throw runtime_error("Error: Could not open file '" + filename + "' for writing");
    }

    for (const auto& num : numbers) {
        outFile << num << '\n';
    }

    if (outFile.fail()) {
        outFile.close();
        throw runtime_error("Error: Failed to write to file '" + filename + "'");
    }

    outFile.close();
    cout << "Successfully wrote " << numbers.size() 
         << " numbers to file '" << filename << "'\n";
}


class LZWTrieCompressor {
public:
    LZWTrieCompressor() {
        resetDictionary();
    }

    vector<int> compress(const vector<string>& lines) {
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

    vector<string> decompress(const vector<int>& compressed) {
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

    int getCode(const string& sequence) {
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

    void addToTrie(const string& sequence) {
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
};



void showMenu(CompressionDatabase& db) {
    LZWTrieCompressor lzw;
    string inputFile, outputFile;
    vector<string> original, decompressed;
    vector<int> compressed;

    while (true) {
        cout << "\nLZW Compression Tool\n";
        cout << "===================\n";
        cout << "1. Compress a file\n";
        cout << "2. Decompress a file\n";
        cout << "3. View compression history\n";
        cout << "4. Clear history\n";
        cout << "5. Exit\n";
        cout << "Enter your choice: ";

        int choice;
        cin >> choice;
        cin.ignore(numeric_limits<streamsize>::max(), '\n');

        try {
            switch (choice) {
                case 1: {
                    inputFile = "original.txt";
                    outputFile = "compressed.txt";

                    original = readAndPrintFile(inputFile);
                    compressed = lzw.compress(original);
                    printAndWriteLinesInt(compressed, outputFile);

                    // Get stats and save to database
                    ifstream in(inputFile, ios::binary | ios::ate);
                    ifstream out(outputFile, ios::binary | ios::ate);
                    if (in && out) {
                        size_t originalSize = in.tellg();
                        size_t compressedSize = out.tellg();
                        in.close();
                        out.close();

                        double ratio = static_cast<double>(compressedSize) / originalSize;
                        double spaceSaving = 1.0 - ratio;

                        db.saveCompressionStats(inputFile, outputFile, 
                                              originalSize, compressedSize, 
                                              ratio, spaceSaving);

                        cout << "\nCompression successful!\n";
                        printFileStats(inputFile, outputFile);
                    } else {
                        cerr << "Error calculating file sizes\n";
                    }
                    break;
                }
                case 2: {
                    inputFile = "compresses.txt";
                    outputFile = "decompressed.txt";

                    ifstream in(inputFile);
                    vector<int> compressedData;
                    int num;
                    while (in >> num) {
                        compressedData.push_back(num);
                    }
                    in.close();

                    decompressed = lzw.decompress(compressedData);
                    printAndWriteLinesString(decompressed, outputFile);

                    cout << "\nDecompression successful!\n";
                    cout << "Original == Decompressed: " 
                         << (compareFiles(inputFile.substr(0, inputFile.find_last_of('_')), outputFile) ? "true" : "false") 
                         << endl;
                    break;
                }
                case 3:
                    db.displayHistory();
                    break;
                case 4:
                    db.clearHistory();
                    break;
                case 5:
                    cout << "Exiting...\n";
                    return;
                default:
                    cout << "Invalid choice. Please try again.\n";
            }
        } catch (const exception& e) {
            cerr << "Error: " << e.what() << endl;
        }
    }
}



int main() {
    CompressionDatabase db("compression.db");
    if (!db.isOpen()) {
        cerr << "Failed to open database. Continuing without database support.\n";
    }

    showMenu(db);
    return 0;
}
