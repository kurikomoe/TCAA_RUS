#pragma once

#include <windows.h>

#include <openssl/evp.h>
#include <cstdlib>
#include <cstdio>
#include <iomanip>
#include <iostream>
#include <vector>
#include <fstream>
#include <string>
#include <sstream>

const auto* gameAssembly = "GameAssembly.dll";
const auto* gameAssemblySha256 = "GameAssembly.dll.sha256";

std::string calculateSHA256(const std::string& filePath) {
    EVP_MD_CTX* mdctx = EVP_MD_CTX_new(); // Create a new context
    if (!mdctx) {
        throw std::runtime_error("Failed to create EVP_MD_CTX");
    }

    // Initialize the context for SHA-256
    if (EVP_DigestInit_ex(mdctx, EVP_sha256(), nullptr) != 1) {
        EVP_MD_CTX_free(mdctx);
        throw std::runtime_error("Failed to initialize EVP context for SHA-256");
    }

    std::ifstream file(filePath, std::ios::binary);
    if (!file.is_open()) {
        EVP_MD_CTX_free(mdctx);
        throw std::runtime_error("Unable to open file: " + filePath);
    }

    const size_t bufferSize = 16 * 1024; // 16KB buffer
    std::vector<char> buffer(bufferSize);

    // Read the file and update the hash calculation
    while (file.good()) {
        file.read(buffer.data(), buffer.size());
        if (EVP_DigestUpdate(mdctx, buffer.data(), file.gcount()) != 1) {
            EVP_MD_CTX_free(mdctx);
            throw std::runtime_error("Failed to update hash");
        }
    }

    unsigned char hash[EVP_MAX_MD_SIZE];
    unsigned int hashLength = 0;

    // Finalize the hash calculation
    if (EVP_DigestFinal_ex(mdctx, hash, &hashLength) != 1) {
        EVP_MD_CTX_free(mdctx);
        throw std::runtime_error("Failed to finalize hash calculation");
    }

    EVP_MD_CTX_free(mdctx); // Clean up the context

    // Convert the hash to a hexadecimal string
    std::ostringstream oss;
    for (unsigned int i = 0; i < hashLength; ++i) {
        oss << std::hex << std::setw(2) << std::setfill('0') << static_cast<int>(hash[i]);
    }

    return oss.str();
}

void check_version() {
    try {
        std::ifstream sha256_file(gameAssemblySha256, std::ios::in);
        std::string sha256;
        sha256_file >> sha256;

        std::string cur_sha256 = calculateSHA256(gameAssembly);

        std::cout << "Current Game sha256:" << cur_sha256 << "\n";
        std::cout << "Target Game  sha256:" << sha256 << "\n";

        if (cur_sha256 != sha256) {
            MessageBoxW(
                NULL,
                L"警告：游戏版本错误，本补丁无法应用于该版本游戏",
                L"TCAA_CHS Fatal Error",
                MB_OK|MB_ICONERROR|MB_SYSTEMMODAL);
            exit(-1);
        }
    } catch (const std::exception& e) {
        std::cerr << "Error: " << e.what() << std::endl;
    }
}
