//
// Created by zeliboba on 5/22/24.
//

#include "bwt.hpp"
#include "mtf2_storage.hpp"
#include <vector>
#include <algorithm>
#include <array>
#include <stdexcept>

// todo: this code right now is doing lots of unnecessary memory allocations
// it's also not very efficiently integrated into SubPrePCS

using namespace std;

std::vector<Token> getAlphabet()
{
    std::vector<Token> result(BinaryAlphabetSize);
    for (auto i = 0; i < BinaryAlphabetSize; ++i)
        result[i] = (Token)i;
    return result;
}

int compareSuffixes(const void *aIn, const void *bIn, void *eofIn)
{
    Token a = *(Token *)aIn;
    Token b = *(Token *)bIn;
    Token eof = *(Token *)eofIn;
    if (a == eof && b == eof)
    {
        return 0;
    }
    if (a == eof)
    {
        return -1;
    }
    if (b == eof)
    {
        return 1;
    }
    return a < b ? -1 : a == b ? 0
                               : 1;
}

int compareRotations(const void *x, const void *y, void *eofIn)
{
    auto firstSuffix = ((struct rotation *)x)->suffix;
    auto secondSuffix = ((struct rotation *)y)->suffix;
    auto i = 0;
    Token eof = *(Token *)eofIn;

    while (true)
    {
        if (firstSuffix[i] == eof && secondSuffix[i] == eof)
        {
            return 0;
        }
        if (firstSuffix[i] == eof)
        {
            return -1;
        }
        if (secondSuffix[i] == eof)
        {
            return 1;
        }
        if (firstSuffix[i] != secondSuffix[i])
        {
            return firstSuffix[i] < secondSuffix[i] ? -1 : 1;
        }
        i++;
    }
}

struct node *getNode(int i)
{
    auto nn = (struct node *)malloc(sizeof(struct node));
    nn->data = i;
    nn->next = nullptr;
    return nn;
}

void addAtLast(struct node **head, struct node *nn)
{
    if (*head == nullptr)
    {
        *head = nn;
        return;
    }
    struct node *temp = *head;
    while (temp->next != nullptr)
    {
        temp = temp->next;
    }
    temp->next = nn;
}

void updateLeftShift(struct node **head, int index, int *leftShift)
{
    struct node *temp = *head;
    leftShift[index] = temp->data;
    *head = (*head)->next;
    free(temp);
}

void smartstrcpy(Token old_array[], Token *new_array, int length)
{
    copy(old_array, old_array + length, new_array);
}

Token search(Token input_char, Token *list)
{
    for (auto i = 0; i < BinaryAlphabetSize; i++)
    {
        if (list[i] == input_char)
        {
            return (Token)i;
        }
    }
    throw std::runtime_error("Character not found in alphabet");
}

// todo: could be optimized A LOT
void moveToFront(Token curr_index, Token *list)
{
    Token *record = (Token *)malloc(sizeof(Token) * (BinaryAlphabetSize));

    for (auto i = 0; i < BinaryAlphabetSize; ++i)
    {
        record[i] = list[i];
    }

    for (auto i = 1; i <= curr_index; ++i)
    {
        list[i] = record[i - 1];
    }

    list[0] = record[static_cast<Token>(curr_index)];
}

int getTokenIndex(vector<Token> vector, Token token)
{
    auto it = find(vector.begin(), vector.end(), token);

    if (it != vector.end())
    {
        return it - vector.begin();
    }
    return -1;
}

BWT::BWT(int chunkSize, int mtfDegree) : chunkSize(chunkSize), mtfDegree(mtfDegree), nodes{nullptr}, suffixes(new rotation[chunkSize + 1]) {}

BWT::~BWT() {
    delete[] suffixes;
    for (auto i = 0; i < BinaryAlphabetSize + 1; i++)
    {
        struct node *temp = nodes[i];
        while (temp != nullptr)
        {
            struct node *next = temp->next;
            free(temp);
            temp = next;
        }
    }
}

std::vector<Token> BWT::Encode(std::vector<Token>& inputText) {
    auto chunksCount = inputText.size() / chunkSize + 1;
    vector<Token> bwtResult;
    for (auto j = 0; j < chunksCount; ++j)
    {
        vector<Token> input(inputText.begin() + j * chunkSize, inputText.begin() + min((j + 1) * chunkSize, (int)inputText.size()));
        input.push_back(BWTEndOfFileToken);
        auto bwtVector = bwtEncode(input);
        bwtResult.insert(bwtResult.end(), bwtVector.begin(), bwtVector.end());
    }
    auto mtfResult = mtfEncode(bwtResult);
    auto res = zleEncode(mtfResult);
    return res;
}

std::vector<Token> BWT::Decode(std::vector<Token>& encodedText) {
    std::vector<Token> toDecode = encodedText;

    auto zleDecodedText = zleDecode(toDecode);
    auto bwtInputText = mtfDecode(zleDecodedText);
    vector<Token> answer;
    for (auto j = 0; j <= bwtInputText.size() / (chunkSize + 1); ++j)
    {
        auto bwtTextLength = min(chunkSize + 1, (int)bwtInputText.size() - j * (chunkSize + 1));

        std::vector<Token> outputText(bwtTextLength);
        for (auto i = 0; i < bwtTextLength; ++i)
        {
            outputText[i] = bwtInputText[j * (chunkSize + 1) + i];
        }
        auto result = bwtDecode(outputText);
        auto indexOfLastChar = getTokenIndex(result, BWTEndOfFileToken);
        if (indexOfLastChar == -1)
        {
            throw std::runtime_error("End of file token not found in BWT decoded text");
        }
        answer.insert(answer.end(), result.begin() + indexOfLastChar + 1, result.end());
        answer.insert(answer.end(), result.begin(), result.begin() + indexOfLastChar);
    }
    return answer;
}

int* BWT::computeSuffixArray(std::vector<Token>& input_text)
{
    int len_text = static_cast<int>(input_text.size());
    for (int i = 0; i < input_text.size(); i++)
    {
        suffixes[i].index = i;
        suffixes[i].suffix = (&(*input_text.begin()) + i);
    }
    qsort_r(suffixes, len_text, sizeof(struct rotation),
            reinterpret_cast<__compar_d_fn_t>(compareRotations), &BWTEndOfFileToken);

    int *suffix_arr = (int *)malloc(len_text * sizeof(int));
    for (int i = 0; i < len_text; i++)
    {
        suffix_arr[i] = suffixes[i].index;
    }

    return suffix_arr;
}

vector<Token> BWT::bwtEncode(std::vector<Token>& input_text)
{
    int n = static_cast<int>(input_text.size());
    int* suffix_arr = computeSuffixArray(input_text);

    vector<Token> bwt(n);
    for (auto i = 0; i < n; i++)
    {
        bwt[i] = input_text[suffix_arr[i] != 0 ? suffix_arr[i] - 1 : n - 1];
    }

    free(suffix_arr);

    return bwt;
}

vector<Token> BWT::bwtDecode(std::vector<Token>& bwt_arr)
{
    int len_bwt = static_cast<int>(bwt_arr.size());
    vector<Token> text(len_bwt);
    Token *sortedBwt = (Token *)malloc(len_bwt * sizeof(Token));
    smartstrcpy(&(*bwt_arr.begin()), sortedBwt, len_bwt);
    int *leftShift = (int *)malloc(len_bwt * sizeof(int));

    qsort_r(sortedBwt, len_bwt, sizeof(Token), reinterpret_cast<__compar_d_fn_t>(compareSuffixes), &BWTEndOfFileToken);

    for (auto i = 0; i < len_bwt; i++)
    {
        addAtLast(&nodes[bwt_arr[i]], getNode(i));
    }

    for (auto i = 0; i < len_bwt; i++)
    {
        updateLeftShift(&nodes[sortedBwt[i]], i, leftShift);
    }

    int x = 0;
    for (auto i = x; i < len_bwt; ++i)
    {
        if (bwt_arr[i] == BWTEndOfFileToken)
        {
            x = i;
            break;
        }
    }

    for (auto i = 0; i < len_bwt; i++)
    {
        x = leftShift[x];
        text[i] = bwt_arr[x];
    }

    free(sortedBwt);
    free(leftShift);

    return text;
}

vector<Token> BWT::zleEncode(std::vector<Token>& bytes)
{
    int size = static_cast<int>(bytes.size());

    vector<Token> result;
    int count = 0;
    for (auto i = 0; i < size; ++i)
    {
        auto byte = bytes[i];
        if (byte == 0)
        {
            count += 1;
            continue;
        }
        if (count != 0)
        {
            while (count > 0)
            {
                result.push_back((count & 1) ? 254 : 253);
                count >>= 1;
            }
            count = 0;
        }
        if (byte == 254 || byte == 253)
        {
            result.push_back(0);
        }
        result.push_back(byte);
    }

    if (count != 0)
    {
        while (count > 0)
        {
            result.push_back((count & 1) == 1 ? 254 : 253);
            count >>= 1;
        }
    }

    return result;
}

vector<Token> BWT::zleDecode(std::vector<Token>& bytes)
{
    int size = static_cast<int>(bytes.size());

    vector<Token> result;
    int run_length = 0;
    int run_bin_len = 0;
    int i = 0;
    while (i < size)
    {
        auto byte = bytes[i];
        if (byte == 254 || byte == 253)
        {
            run_length |= (byte == 254 ? 1 : 0) << run_bin_len;
            run_bin_len += 1;
            i += 1;
            continue;
        }
        for (auto j = 0; j < run_length; j++)
        {
            result.push_back(0);
        }
        run_length = 0;
        run_bin_len = 0;

        if (byte == 0)
        {
            result.push_back(bytes[i + 1]);
            i += 2;
            continue;
        }
        result.push_back(byte);
        i += 1;
    }

    for (auto j = 0; j < run_length; j++)
    {
        result.push_back(0);
    }

    return result;
}

vector<Token> BWT::mtfEncode(std::vector<Token>& input_text)
{
    int len_text = static_cast<int>(input_text.size());
    auto mtfAlphabet = MTF2Storage(getAlphabet(), BinaryAlphabetSize, mtfDegree);

    vector<Token> result(len_text);

    for (auto i = 0; i < len_text; i++)
    {
        auto element = mtfAlphabet.Find(input_text[i]);
        auto pos = static_cast<int>(std::distance(mtfAlphabet.Elements.begin(), element));
        result[i] = pos;
        mtfAlphabet.MoveToFront(element);
    }

    return result;
}

vector<Token> BWT::mtfDecode(std::vector<Token>& arr)
{
    int n = static_cast<int>(arr.size());
    auto mtfAlphabet = MTF2Storage(getAlphabet(), BinaryAlphabetSize, mtfDegree);

    vector<Token> result(n);
    for (auto i = 0; i < n; i++)
    {
        if (arr[i] >= mtfAlphabet.Elements.size() || arr[i] < 0)
        {
            throw std::out_of_range("Index out of bounds in mtfDecode: " + to_string(arr[i]));
        }

        result[i] = mtfAlphabet.Elements[arr[i]];
        mtfAlphabet.MoveToFront(mtfAlphabet.Elements.begin() + arr[i]);
    }

    return result;
}