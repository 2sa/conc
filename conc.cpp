#include <iostream>
#include <cstdlib>
#include <cstdio>
#include <string>
#include <vector>

bool IsSentenceStartWord(std::string word)
{
    auto c = word[0];

    if (c <= 'Z' && c >= 'A')
    {
        return true;
    }
    else
    {
        return false;
    }
}

bool IsSentenceEndWord(std::string word)
{
    auto c = word[word.length() - 1];

    return c == '.' || c == '!' || c == '?' || c == '"';
}

template <typename T>
class Map
{
public:
    Map* left;
    Map* right;
    Map* parent;

    T* data;

    Map(Map* l, Map* r, T* d):left(l), right(r), data(d), parent(nullptr)
    {}

    Map(Map* l, Map* r, Map* p, T* d):left(l), right(r), data(d), parent(p)
    {}

    T* Insert(T* key)
    {
        if (parent == nullptr)
        {
            data = key;
            parent = this;
            return data;
        }

        if (*key < *data)
        {
            if (left != nullptr)
            {
                return left->Insert(key);
            }
            else
            {
                left = new Map(nullptr, nullptr, this, key);
                return left->data;
            }
        }

        if (*key > *data)
        {
            if (right != nullptr)
            {
                return right->Insert(key);
            }
            else
            {
                right = new Map(nullptr, nullptr, this, key);
                return right->data;
            }
        }

        return data;
    }

    T* Find(T* key)
    {
        if (key == nullptr || parent == nullptr)
        {
            return nullptr;
        }

        if (*data == *key)
        {
            return data;
        }

        if (*key < *data)
        {
            if (left != nullptr)
            {
                return left->Find(key);
            }
        }

        if (*key > *data)
        {
            if (right != nullptr)
            {
                return right->Find(key);
            }
        }

        return nullptr;
    }

    void LRR(Map* root)
    {
        if (root == nullptr)
        {
            return;
        }

        root->left->LRR(root->left);
        std::cout << (std::string)(*data) << std::endl;
        root->right->LRR(root->right);
    }

    void Print()
    {
        LRR(this);
    }
};

class IndexPositionItem
{
public:
    std::string keyPtr;
    IndexPositionItem* prev;
    IndexPositionItem* next;

    int pos;

    IndexPositionItem() :prev(nullptr), next(nullptr) {}
    IndexPositionItem(int p) :prev(nullptr), next(nullptr), pos(p) {}
};

class IndexKey
{
public:
    std::string word;
    std::vector<IndexPositionItem*> items;

    IndexKey() : word("")
    {}

    IndexKey(std::string w) : word(w)
    {}

    void AddPosition(IndexPositionItem* pos)
    {
        items.push_back(pos);
    }

    bool operator < (IndexKey k)
    {
        return word < k.word;
    }

    bool operator > (IndexKey k)
    {
        return word > k.word;
    }

    bool operator == (IndexKey k)
    {
        return word == k.word;
    }

    bool operator != (IndexKey k)
    {
        return word != k.word;
    }

    operator std::string()
    {
        std::string itemsPositions;
        for each (auto p in items)
        {
            itemsPositions += " " + std::to_string(p->pos);
        }
        return std::string(word) + " [" + itemsPositions + " ]";
    }
};

class QueryString
{
public:
    std::vector<std::string> queryWords;

    size_t Size()
    {
        return queryWords.size();
    }

    void BuildFromString(std::string query)
    {
        queryWords.clear();
        queryWords.push_back("");

        for each(auto c in query)
        {
            if (c == ' ')
            {
                queryWords.push_back("");
                continue;
            }
            else
            {
                queryWords[queryWords.size() - 1].push_back(c);
            }
        }

        for each (auto s in queryWords)
        {
            std::cout << s << " ";
        }

        std::cout << std::endl << "Done" << std::endl;
    }

    std::string FirstWord()
    {
        return queryWords.at(0);
    }
};


class SearchResult
{
public:
    std::vector<IndexPositionItem*> results;

    size_t count;
    size_t current;

    QueryString* query;

    std::vector<IndexPositionItem*>* resultsFromIndex;

    SearchResult() :current(0)
    {}

    IndexPositionItem* GetPositionInfo()
    {
        return resultsFromIndex->at(current);
    }

    IndexPositionItem* GetNextPosition()
    {
        if (current < resultsFromIndex->size() - 1)
        {
            return resultsFromIndex->at(++current);
        }

        return nullptr;
    }

    void AddResult(IndexPositionItem* res)
    {
        results.push_back(res);
    }

    size_t QuerySize()
    {
        return query->Size();
    }
};

class InvertedIndex
{
public:
    InvertedIndex():base(new Map<IndexKey>(nullptr, nullptr, new IndexKey()))
    {}

    void BuildFromStdin()
    {
        std::string word;
        IndexKey* result = nullptr;
        int pos = 0;

        IndexPositionItem* prev = nullptr;

        while (std::cin >> word)
        {
            result = base->Insert(new IndexKey(word));

            IndexPositionItem* curr = new IndexPositionItem(pos);
            
            pos += word.length();

            curr->keyPtr = result->word;

            if (prev != nullptr)
            {
                prev->next = curr;
            }

            curr->prev = prev;

            prev = curr;

            result->AddPosition(curr);
        }
    }

    void BuildFromString(std::string text)
    {
        std::vector<std::string> words;
        words.clear();
        words.push_back("");

        IndexKey* result = nullptr;
        int pos = 0;

        IndexPositionItem* prev = nullptr;

        for each(auto c in text)
        {
            if (c == ' ')
            {
                words.push_back("");
                continue;
            }
            else
            {
                words[words.size() - 1].push_back(c);
            }
        }

        for each (auto word in words)
        {
            result = base->Insert(new IndexKey(word));

            IndexPositionItem* curr = new IndexPositionItem(pos);

            pos += word.length();

            curr->keyPtr = result->word;

            if (prev != nullptr)
            {
                prev->next = curr;
            }

            curr->prev = prev;

            prev = curr;

            result->AddPosition(curr);
        }
    }

    SearchResult* FindFirstPos(std::string word)
    {
        IndexKey key;
        key.word = word;
        IndexKey* result = base->Find(&key);
        
        if (result != nullptr)
        {
            auto res = new SearchResult();
            res->count = result->items.size();
            res->resultsFromIndex = &result->items;
            return res;
        }
        else
        {
            return nullptr;
        }
    }

    SearchResult* GetSearchResult(QueryString* query)
    {
        auto searchResult = new SearchResult();
        searchResult->query = query;

        auto first = FindFirstPos(query->FirstWord());

        auto result = first->GetPositionInfo();

        auto currentWord = first->GetPositionInfo();

        if (query->queryWords.size() == 1)
        {
            while (result != nullptr)
            {
                searchResult->AddResult(result);
                result = first->GetNextPosition();
            }
        }
        else
        {
            while (result != nullptr)
            {
                for (size_t i = 1; i < query->queryWords.size() && currentWord != nullptr; ++i)
                {
                    if (CheckOrder(currentWord, query->queryWords[i]))
                    {
                        if (i == query->queryWords.size() - 1)
                        {
                            searchResult->AddResult(result);

                            result = first->GetNextPosition();
                            currentWord = result;
                            i = 1;
                        }
                        else
                        {
                            currentWord = currentWord->next;
                        }
                    }
                    else
                    {
                        result = first->GetNextPosition();
                        currentWord = result;
                        i = 1;
                    }
                }
            }
        }

        return searchResult;
    }

    bool CheckOrder(IndexPositionItem* curr, std::string next)
    {
        if (curr->next == nullptr)
        {
            return true;
        }

        return next == curr->next->keyPtr;
    }

    void Traverse(std::string startWord)
    {
        auto position = FindFirstPos(startWord)->GetPositionInfo();

        while (position != nullptr)
        {
            std::cout << position->keyPtr << " ";

            position = position->next;
        }
    }

    IndexPositionItem* FindSentenceBegin(IndexPositionItem* wordPosition)
    {
        IndexPositionItem* result = wordPosition;

        while (result->prev != nullptr && !IsSentenceStartWord(result->keyPtr))
        {
            result = result->prev;
        }

        return result;
    }
    
    void GetSentence(std::string word)
    {
        auto position = FindFirstPos(word)->GetPositionInfo();

        GetSentence(position);
    }

    void GetSentence(IndexPositionItem* position)
    {
        position = FindSentenceBegin(position);

        while (position != nullptr)
        {
            std::cout << position->keyPtr << " ";

            if (IsSentenceEndWord(position->keyPtr))
            {
                break;
            }

            position = position->next;
        }
    }

    void GetWordsAround(IndexPositionItem* position, size_t wordsAround, size_t querySize)
    {
        size_t wordsCount = wordsAround + querySize;
        size_t leftSteps = 0;
        
        auto startPosition = position;

        while (startPosition->prev != nullptr && leftSteps != wordsAround)
        {
            leftSteps++;
            wordsCount++;
            startPosition = startPosition->prev;
        }
    
        for (size_t i = 0; i < wordsCount; ++i)
        {
            std::cout << startPosition->keyPtr << " ";

            if (startPosition->next == nullptr)
            {
                return;
            }

            startPosition = startPosition->next;
        }
    }

    void PrintResultsWordAround(SearchResult* res, size_t wordAround)
    {
        for each (auto result in res->results)
        {
            GetWordsAround(result, wordAround, res->QuerySize());
            std::cout << std::endl;
        }
    }

    void PrintResults(SearchResult* results)
    {
        for each(auto res in results->results)
        {
            GetSentence(res);
            std::cout << std::endl;
        }
    }

    Map<IndexKey>* base;
};

int main()
{
    InvertedIndex idx;

    idx.BuildFormStdin();
    //idx.BuildFromString("Is who is ijghwdiuegdiweu weudewdewdwd. Arterterherjthj ejrkthretr. And who is he? ewlkqejgrwketg wejhkrgtwe. werhgtehwrt");
    //idx.base->Print();
    //idx.Traverse("Reading");

    
    auto query = new QueryString();
    query->BuildFromString("the");
    /*
    auto results = idx.GetSearchResult(query);
    idx.PrintResults(results);
    */
    
    auto results = idx.GetSearchResult(query);
    idx.PrintResultsWordAround(results, 2);

    return 0;
}
