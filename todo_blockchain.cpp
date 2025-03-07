#include <iostream>
#include <vector>
#include <ctime>
#include <sstream>
#include <iomanip>
#include <fstream>
#include <openssl/sha.h> // For SHA-256 hashing

const int DIFFICULTY = 2; // Adjust mining difficulty

// Function to calculate SHA-256 hash
std::string calculateHash(const std::string& data) {
    unsigned char hash[SHA256_DIGEST_LENGTH];
    SHA256_CTX sha256;
    SHA256_Init(&sha256);
    SHA256_Update(&sha256, data.c_str(), data.size());
    SHA256_Final(hash, &sha256);

    std::stringstream ss;
    for (int i = 0; i < SHA256_DIGEST_LENGTH; i++) {
        ss << std::hex << std::setw(2) << std::setfill('0') << (int)hash[i];
    }
    return ss.str();
}

// Block Class
class Block {
public:
    std::string previousHash;
    std::string data; // Task description
    std::string hash;
    time_t timestamp;
    int nonce; // Mining nonce

    Block(std::string data, std::string previousHash) {
        this->data = data;
        this->previousHash = previousHash;
        this->timestamp = time(nullptr);
        this->nonce = 0;
        this->hash = mineBlock();
    }

    std::string mineBlock() {
        std::string newHash;
        std::string target(DIFFICULTY, '0'); // Target hash prefix

        do {
            nonce++;
            newHash = calculateHash(data + previousHash + std::to_string(timestamp) + std::to_string(nonce));
        } while (newHash.substr(0, DIFFICULTY) != target);

        return newHash;
    }

    void displayBlock() const {
        std::cout << "Task: " << data << std::endl;
        std::cout << "Timestamp: " << std::asctime(std::localtime(&timestamp));
        std::cout << "Nonce: " << nonce << std::endl;
        std::cout << "Previous Hash: " << previousHash << std::endl;
        std::cout << "Hash: " << hash << std::endl;
        std::cout << "--------------------------" << std::endl;
    }
};

// Blockchain Class
class Blockchain {
private:
    std::vector<Block> chain;

public:
    Blockchain() {
        // Load blockchain from file or create the genesis block
        if (!loadBlockchain()) {
            Block genesis("Genesis Task", "0");
            chain.push_back(genesis);
            saveBlockchain();
        }
    }

    void addBlock(std::string data) {
        Block newBlock(data, chain.back().hash);
        chain.push_back(newBlock);
        saveBlockchain();
    }

    void displayChain() const {
        for (const auto& block : chain) {
            block.displayBlock();
        }
    }

    bool loadBlockchain() {
        std::ifstream file("blockchain_data.txt");
        if (!file) return false;

        chain.clear();
        std::string data, prevHash, hash;
        time_t timestamp;
        int nonce;

        while (file >> timestamp >> nonce >> prevHash >> hash) {
            file.ignore();
            std::getline(file, data);
            Block block(data, prevHash);
            block.timestamp = timestamp;
            block.nonce = nonce;
            block.hash = hash;
            chain.push_back(block);
        }

        file.close();
        return !chain.empty();
    }

    void saveBlockchain() const {
        std::ofstream file("blockchain_data.txt", std::ios::trunc);
        for (const auto& block : chain) {
            file << block.timestamp << " " << block.nonce << " "
                 << block.previousHash << " " << block.hash << "
"
                 << block.data << "
";
        }
        file.close();
    }
};

// Main Program
int main() {
    Blockchain todoList;
    std::cout << "===== Blockchain-Based To-Do List =====
";

    while (true) {
        std::cout << "
1. Add Task
2. View Tasks
3. Exit
Enter your choice: ";
        int choice;
        std::cin >> choice;
        std::cin.ignore();

        switch (choice) {
            case 1: {
                std::string task;
                std::cout << "Enter the task: ";
                std::getline(std::cin, task);
                todoList.addBlock(task);
                std::cout << "Task added successfully with Proof of Work!
";
                break;
            }
            case 2:
                std::cout << "
--- To-Do List Blockchain ---
";
                todoList.displayChain();
                break;
            case 3:
                std::cout << "Exiting...
";
                return 0;
            default:
                std::cout << "Invalid choice! Please try again.
";
                break;
        }
    }

    return 0;
}
