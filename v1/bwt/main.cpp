#include <iostream>
#include <vector>
#include <algorithm>
#include <fstream>
#include <string>
#include <array>

using namespace std;

// TODO: think about large alphabet
const int alphabetSize = 256;
const int chunkSize = 100000;

array<char, alphabetSize> get_alphabet()
{
    array<char, alphabetSize> result{};
    for (auto i = 0; i < alphabetSize; ++i)
        result[i] = (char)i;
    return result;
}

array<char, alphabetSize> alphabet = get_alphabet();

struct rotation
{
    int index;
    char *suffix;
};

int compare_suffixes(const void *aIn, const void *bIn, void *eofIn)
{
    char a = *(char *)aIn;
    char b = *(char *)bIn;
    char eof = *(char *)eofIn;
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

int compare_rotations(const void *x, const void *y, void *eofIn)
{
    auto firstSuffix = ((struct rotation *)x)->suffix;
    auto secondSuffix = ((struct rotation *)y)->suffix;
    auto i = 0;
    char eof = *(char *)eofIn;

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

struct rotation suffixes[4000000];

int *computeSuffixArray(char *input_text, int len_text, char eof)
{
    for (int i = 0; i < len_text; i++)
    {
        suffixes[i].index = i;
        suffixes[i].suffix = (input_text + i);
    }
    qsort_r(suffixes, len_text, sizeof(struct rotation), compare_rotations, &eof);

    int *suffix_arr = (int *)malloc(len_text * sizeof(int));
    for (int i = 0; i < len_text; i++)
    {
        suffix_arr[i] = suffixes[i].index;
    }

    return suffix_arr;
}

vector<char> bwt(char *input_text, int *suffix_arr, int n)
{
    vector<char> bwt(n);
    for (auto i = 0; i < n; i++)
    {
        bwt[i] = input_text[suffix_arr[i] != 0 ? suffix_arr[i] - 1 : n - 1];
    }
    return bwt;
}

string readFromFile(string filename)
{
    ifstream input_file(filename, ios_base::binary);
    if (input_file.is_open())
    {
        return {(istreambuf_iterator<char>(input_file)), istreambuf_iterator<char>()};
    }
    exit(EXIT_FAILURE);
}

void printTextToFile(string filename, vector<char> text)
{
    ofstream outputFile(filename, ios_base::binary);
    if (outputFile.is_open())
    {
        for (auto symbol : text)
            outputFile << symbol;
    }
}

struct node
{
    int data;
    struct node *next;
};

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
    leftShift[index] = (*head)->data;
    (*head) = (*head)->next;
}

struct node *nodes[256] = {nullptr};

void smartstrcpy(char old_array[], char *new_array, int length)
{
    copy(old_array, old_array + length, new_array);
}

vector<char> invert(char bwt_arr[], int len_bwt, char eof)
{
    vector<char> text(len_bwt);
    char *sortedBwt = (char *)malloc(len_bwt * sizeof(char));
    smartstrcpy(bwt_arr, sortedBwt, len_bwt);
    int *leftShift = (int *)malloc(len_bwt * sizeof(int));

    qsort_r(sortedBwt, len_bwt, sizeof(char), compare_suffixes, &eof);

    for (auto i = 0; i < len_bwt; i++)
    {
        addAtLast(&nodes[bwt_arr[i]], getNode(i));
    }

    /**
     * Баг с leftShift: он разбивается на несколько компонент связности, хотя так быть не должно.
     * Я должен с любого номера уметь попадать в любой другой
     *
     * Проблема: кривой sortedBwt, так как EOF-токен не меньше всех остальных, и из-за этого ломается логика
     */
    for (auto i = 0; i < len_bwt; i++)
    {
        updateLeftShift(&nodes[sortedBwt[i]], i, leftShift);
    }

    /**
     * x - номер исходной строки в отсортированном массиве всех циклических сдвигов этой строки.
     * Чтобы найти этот номер, необходимо найти EOF-токен в bwt_arr.
     * Проблема с циклическими сдвигами возникает, если таких токенов несколько.
     *
     * Можно попытаться записывать номер EOF-токена в файл при кодировании, тогда я точно буду знать x.
     * Проблема в том, что таких номеров всего chunks_count. Можно записывать в отдельный файл.
     */
    int x = 0;
    for (auto i = x; i < len_bwt; ++i)
    {
        if (bwt_arr[i] == eof)
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

    return text;
}

vector<char> zleEncode(char bytes[], int size)
{
    vector<char> result;
    int count = 0;
    for (auto i = 0; i < size; ++i)
    {
        auto byte = bytes[i];
        if (byte == '\x00')
        {
            count += 1;
            continue;
        }
        if (count != 0)
        {

            while (count > 0)
            {
                result.push_back((count & 1) ? '\xff' : '\xfe');
                count >>= 1;
            }
            count = 0;
        }
        if (byte == '\xff' || byte == '\xfe')
        {
            result.push_back('\x00');
        }
        result.push_back(byte);
    }

    if (count != 0)
    {
        while (count > 0)
        {
            result.push_back((count & 1) == 1 ? '\xff' : '\xfe');
            count >>= 1;
        }
    }

    return result;
}

vector<char> zleDecode(char bytes[], int size)
{
    vector<char> result;
    int run_length = 0;
    int run_bin_len = 0;
    int i = 0;
    while (i < size)
    {
        auto byte = bytes[i];
        if (byte == '\xff' || byte == '\xfe')
        {
            run_length |= (byte == '\xff' ? 1 : 0) << run_bin_len;
            run_bin_len += 1;
            i += 1;
            continue;
        }
        for (auto j = 0; j < run_length; j++)
        {
            result.push_back('\x00');
        }
        run_length = 0;
        run_bin_len = 0;

        if (byte == '\x00')
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
        result.push_back('\x00');
    }

    return result;
}

char search(char input_char, char *list)
{
    for (auto i = 0; i < 256; i++)
    {
        if (list[i] == input_char)
        {
            return (char)i;
        }
    }
    return '\x00';
}

void moveToFront(char curr_index, char *list)
{
    char *record = (char *)malloc(sizeof(char) * 256);

    for (auto i = 0; i < 256; ++i)
    {
        record[i] = list[i];
    }

    for (auto i = 1; i <= static_cast<unsigned char>(curr_index); ++i)
    {
        list[i] = record[i - 1];
    }

    list[0] = record[static_cast<unsigned char>(curr_index)];
}

vector<char> mtfEncode(char *input_text, int len_text, char *list)
{
    vector<char> result(len_text);

    for (auto i = 0; i < len_text; i++)
    {
        result[i] = search(input_text[i], list);
        moveToFront(result[i], list);
    }
    return result;
}

vector<char> mtfDecode(char arr[], int n, char *list)
{
    vector<char> result(n);

    for (auto i = 0; i < n; i++)
    {
        result[i] = list[static_cast<unsigned char>(arr[i])];
        moveToFront(arr[i], list);
    }
    return result;
}

int getTokenIndex(vector<char> vector, char token)
{
    auto it = find(vector.begin(), vector.end(), token);

    if (it != vector.end())
    {
        return it - vector.begin();
    }
    return -1;
}

void encode(string input, string output, char eof)
{
    // TODO: fix suffixes with static size
    // TODO: fix chunks
    string inputText = readFromFile(input);
    auto chunksCount = inputText.length() / chunkSize + 1;
    vector<char> bwtResult;
    for (auto j = 0; j < chunksCount; ++j)
    {
        string input(inputText.substr(j * chunkSize, chunkSize) + eof);
        char input_text[input.length()];
        for (auto i = 0; i < input.length(); ++i)
        {
            input_text[i] = input.at(i);
        }
        int chunkLength = sizeof(input_text);
        auto bwtVector = bwt(input_text, computeSuffixArray(input_text, chunkLength, eof),
                             chunkLength);
        bwtResult.insert(bwtResult.end(), bwtVector.begin(), bwtVector.end());
    }
    auto mtfResult = mtfEncode(bwtResult.data(), bwtResult.size(), alphabet.data());
    // auto res = zleEncode(mtfResult.data(), mtfResult.size());
    printTextToFile(output, mtfResult);
}

void decode(string input, string output, char eof)
{
    auto encodedText = readFromFile(input);
    char text[encodedText.length()];
    for (auto i = 0; i < encodedText.length(); ++i)
    {
        text[i] = encodedText.at(i);
    }

    auto zleDecodedText = zleDecode(text, encodedText.length());
    auto bwtInputText = mtfDecode(zleDecodedText.data(), zleDecodedText.size(), alphabet.data());
    vector<char> answer;
    for (auto j = 0; j <= bwtInputText.size() / (chunkSize + 1); ++j)
    {
        auto bwtTextLength = min(chunkSize + 1, (int)bwtInputText.size() - j * (chunkSize + 1));

        char outputText[bwtTextLength];
        for (auto i = 0; i < bwtTextLength; ++i)
        {
            outputText[i] = bwtInputText[j * (chunkSize + 1) + i];
        }
        auto result = invert(outputText, bwtTextLength, eof);
        auto indexOfLastChar = getTokenIndex(result, eof);
        answer.insert(answer.end(), result.begin() + indexOfLastChar + 1, result.end());
        answer.insert(answer.end(), result.begin(), result.begin() + indexOfLastChar);
    }
    printTextToFile(output, answer);
}

string to_lower(string str)
{
    for (auto &c : str)
        c = tolower(c);
    return str;
}

void test(string input, string output)
{
    auto eofToken = 'q';
    string inputText = readFromFile(input) + eofToken;
    char text[inputText.length()];
    for (auto i = 0; i < inputText.length(); ++i)
    {
        text[i] = inputText.at(i);
    }
    auto bwtResult = bwt(text, computeSuffixArray(text, inputText.length(), eofToken), inputText.length());
    char outputText[bwtResult.size()];
    auto decode = invert(outputText, bwtResult.size(), eofToken);
    for (auto i = 0; i < inputText.length(); ++i)
    {
        if (decode[i] != inputText.at(i))
        {
            cout << i << " decoded: " << decode[i] << " encoded: " << inputText.at(i) << '\n';
        }
    };
}

int main(int argc, char *argv[])
{
    if (argc != 5)
        return 1;
    string type = to_lower(argv[1]);
    if (type == "encode")
        encode(argv[2], argv[3], stoi(argv[4]));
    else if (type == "decode")
        decode(argv[2], argv[3], stoi(argv[4]));
    else if (type == "test")
        test(argv[2], argv[3]);
    else
        return 2;
    return 0;
}
