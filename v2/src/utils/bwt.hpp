//
// Created by zeliboba on 5/22/24.
//

#ifndef DIPLOMA_BWT_HPP
#define DIPLOMA_BWT_HPP

#include <vector>
#include <string>
#include <array>
#include "defs.hpp"

// todo: clean up, hide these structs
struct rotation
{
    int index;
    Token *suffix;
};

struct node
{
    int data;
    struct node *next;
};

class BWT
{
public:
    BWT(int chunkSize, int mtfDegree = 1);
    ~BWT();

    std::vector<Token> Encode(std::vector<Token>& input);

    std::vector<Token> Decode(std::vector<Token>& encodedText);

private:
    int chunkSize;
    int mtfDegree;
    node *nodes[BinaryAlphabetSize + 1] = {nullptr};
    rotation* suffixes{nullptr};

    int* computeSuffixArray(std::vector<Token>& input);

    std::vector<Token> bwtEncode(std::vector<Token>& input);

    std::vector<Token> bwtDecode(std::vector<Token>& encodedText);

    std::vector<Token> zleEncode(std::vector<Token>& input);

    std::vector<Token> zleDecode(std::vector<Token>& input);

    std::vector<Token> mtfEncode(std::vector<Token>& input);

    std::vector<Token> mtfDecode(std::vector<Token>& input);
};

#endif //DIPLOMA_BWT_HPP
