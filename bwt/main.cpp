#include <iostream>
#include <vector>
#include <algorithm>
#include <cstring>
#include <fstream>

using namespace std;

struct rotation {
    int index;
    char* suffix;
};

int compare_rotations(const void* x, const void* y) {
    return strcmp(((struct rotation*)x) -> suffix, ((struct rotation*)y) -> suffix);
}

struct rotation suffixes[4000000];

int* computeSuffixArray(char* input_text, int len_text) {
    for (int i = 0; i < len_text; i++) {
        suffixes[i].index = i;
        suffixes[i].suffix = (input_text + i);
    }

    qsort(suffixes, len_text, sizeof(struct rotation),compare_rotations);

    int* suffix_arr = (int*)malloc(len_text * sizeof(int));
    for (int i = 0; i < len_text; i++) {
        suffix_arr[i] = suffixes[i].index;
    }

    return suffix_arr;
}

vector<char> bwt(char* input_text, int* suffix_arr, int n) {
    vector<char> bwt(n);
    for (auto i = 0; i < n; i++) {
        bwt[i] = input_text[suffix_arr[i] != 0 ? suffix_arr[i] - 1 : n - 1];
    }
    return bwt;
}

string readFromFile(string filename) {
    ifstream input_file(filename);
    if (input_file.is_open()) {
        return {(std::istreambuf_iterator<char>(input_file)), std::istreambuf_iterator<char>()};
    }
    exit(EXIT_FAILURE);
}


void printTextToFile(string filename, vector<char> text) {
    ofstream outputFile(filename);
    if (outputFile.is_open()) {
        for (auto symbol : text)
            outputFile << symbol;
    }
}

struct node {
    int data;
    struct node* next;
};

int compare_suffixes(const void* a, const void* b) {
    return strcmp((const char*)a, (const char*)b);
}

struct node* getNode(int i) {
    auto nn = (struct node*)malloc(sizeof(struct node));
    nn->data = i;
    nn->next = nullptr;
    return nn;
}

void addAtLast(struct node** head, struct node* nn)
{
    if (*head == nullptr) {
        *head = nn;
        return;
    }
    struct node* temp = *head;
    while (temp->next != nullptr) {
        temp = temp->next;
    }
    temp->next = nn;
}

void updateLeftShift(struct node** head, int index, int* leftShift) {
    leftShift[index] = (*head)->data;
    (*head) = (*head)->next;
}

struct node* nodes[4000000] = {nullptr };

vector<char> invert(char bwt_arr[], int len_bwt) {
    vector<char> text(len_bwt);
    char* sortedBwt = (char*)malloc(len_bwt * sizeof(char));
    strcpy(sortedBwt, bwt_arr);
    int* leftShift = (int*)malloc(len_bwt * sizeof(int));

    qsort(sortedBwt, len_bwt, sizeof(char), compare_suffixes);

    for (auto i = 0; i < len_bwt; i++) {
        addAtLast(&nodes[bwt_arr[i]], getNode(i));
    }

    for (auto i = 0; i < len_bwt; i++) {
        updateLeftShift(&nodes[sortedBwt[i]], i, leftShift);
    }

    int x = 4;
    for (auto i = 0; i < len_bwt; i++) {
        x = leftShift[x];
        text[i] = bwt_arr[x];
    }

    return text;
}

vector<char> zleEncode(char bytes[], int size) {
    vector<char> result;
    int count = 0;
    for (auto i = 0; i < size; ++i) {
        auto byte = bytes[i];
        if (byte == '\x00') {
            count += 1;
            continue;
        }
        if (count != 0) {

            while (count > 0) {
                result.push_back((count & 1) ? '\xff' : '\xfe');
                count >>= 1;
            }
            count = 0;
        }
        if (byte == '\xff' || byte == '\xfe') {
            result.push_back('\x00');
        }
        result.push_back(byte);
    }

    if (count != 0) {
        while (count > 0) {
            result.push_back((count & 1) == 1 ? '\xff' : '\xfe');
            count >>= 1;
        }
    }

    return result;
}

vector<char> zleDecode(char bytes[], int size) {
    vector<char> result;
    int run_length = 0;
    int run_bin_len = 0;
    int i = 0;
    while (i < size) {
        auto byte = bytes[i];
        if (byte == '\xff' || byte == '\xfe') {
            run_length |= (byte == '\xff' ? 1 : 0) << run_bin_len;
            run_bin_len += 1;
            i += 1;
            continue;
        }
        for (auto j = 0; j < run_length; j++) {
            result.push_back('\x00');
        }
        run_length = 0;
        run_bin_len = 0;

        if (byte == '\x00') {
            result.push_back(bytes[i + 1]);
            i += 2;
            continue;
        }
        result.push_back(byte);
        i += 1;
    }

    for (auto j = 0; j < run_length; j++) {
        result.push_back('\x00');
    }

    return result;
}

char search(char input_char, char* list)
{
    for (auto i = 0; i < 256; i++) {
        if (list[i] == input_char) {
            return (char) i;
        }
    }
    return '\x00';
}

void moveToFront(char curr_index, char* list) {
    char* record = (char*)malloc(sizeof(char) * 256);

    for (auto i = 0; i < 256; ++i) {
        record[i] = list[i];
    }

    for (auto i = 1; i <= curr_index; ++i) {
        list[i] = record[i - 1];
    }

    list[0] = record[curr_index];
}

vector<char> mtfEncode(char* input_text, int len_text, char* list) {
    vector<char> result(len_text);

    for (auto i = 0; i < len_text; i++) {
        result[i] = search(input_text[i], list);
        moveToFront(result[i], list);
    }
    return result;
}

vector<char> mtfDecode(char arr[], int n, char* list)
{
    vector<char> result(n);

    for (auto i = 0; i < n; i++) {
        result[i] = list[arr[i]];
        moveToFront(arr[i], list);
    }
    return result;
}

int getTokenIndex(vector<char> vector, char token) {
    auto it = find(vector.begin(), vector.end(), token);

    if (it != vector.end()) {
        return it - vector.begin();
    }
    return -1;
}

int main() {
    // TODO: think about large alphabet
    auto alphabetSize = 256;
    char eof = '&';
    int chunkSize = 100000;
    char alphabet[alphabetSize];
    for (auto i = 0; i < alphabetSize; ++i) {
        alphabet[i] = (char) i;
    }
    // TODO: fix const paths
    // TODO: fix suffixes with static size
    // TODO: fix chunks
    auto inputText = readFromFile("/home/lexlippi/CLionProjects/bwt_encoder/war-and-peace-decapitalize.txt");
    auto chunksCount = inputText.length() / chunkSize + 1;
    vector<char> bwtResult;
    for (auto j = 0; j < chunksCount; ++j) {
        string input(inputText.substr(j * chunkSize, chunkSize) + eof);
        char input_text[input.length()];
        for (auto i = 0; i < input.length(); ++i) {
            input_text[i] = input.at(i);
        }
        int chunkLength = sizeof(input_text);
        auto bwtVector = bwt(input_text, computeSuffixArray(input_text, chunkLength),
                             chunkLength);
        bwtResult.insert(bwtResult.end(), bwtVector.begin(), bwtVector.end());
    }
    auto mtfResult = mtfEncode(bwtResult.data(), bwtResult.size(), alphabet);
    auto res = zleEncode(mtfResult.data(), mtfResult.size());
    printTextToFile("/home/lexlippi/CLionProjects/bwt_encoder/encoded.txt", res);

//    auto encodedText = readFromFile("/home/lexlippi/CLionProjects/bwt_encoder/example3.txt");
//    char text[encodedText.length()];
//    for (auto i = 0; i < encodedText.length(); ++i) {
//        text[i] = encodedText.at(i);
//    }
//
//    auto zleDecodedText = zleDecode(text, encodedText.length());
//    auto bwtInputText = mtfDecode(zleDecodedText.data(), zleDecodedText.size(), alphabet);
//    vector<char> answer;
//    for (auto j = 0; j <= bwtInputText.size() / (chunkSize + 1); ++j) {
//        auto bwtTextLength = min(chunkSize + 1, (int) bwtInputText.size() - j * (chunkSize + 1));
//
//        char outputText [bwtTextLength];
//        for (auto i = 0; i < bwtTextLength; ++i) {
//            outputText[i] = bwtInputText[j * (chunkSize + 1) + i];
//        }
//        auto result = invert(outputText, bwtTextLength);
//        auto indexOfLastChar = getTokenIndex(result, eof);
//        answer.insert(answer.end(), result.begin() + indexOfLastChar + 1, result.end());
//        answer.insert(answer.end(), result.begin(), result.begin() + indexOfLastChar);
//
//    }
//    printTextToFile("/home/lexlippi/CLionProjects/bwt_encoder/example2.txt", answer);
    return 0;
}




