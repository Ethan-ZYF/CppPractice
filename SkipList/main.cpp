#include <iostream>
#include "SkipList.h"

#include <iostream>
#include "SkipList.h"

using namespace std;

int main() {

    cout << "skiplist test now starts..." << endl;

    SkipList<int, int> skipList(12);

    int length = 10;

    for (int i = 1; i <= length; ++i) {
        skipList.insert(i, i + 200);
    }

    cout << "The number of elements in skiplist is:" << skipList.size() << endl;

    if (skipList.size() != length) {
        cout << "insert failur." << endl;
    } else {
        cout << "insert success." << endl;
    }

    //测试查找
    int value = -1;
    int key = 9;
    bool searchResult = skipList.search(key, value);
    if (searchResult) {
        value = value;
        cout << "search result for key " << key << ":" << value << endl;
    } else {
        cout << "search failure for key " << key << endl;
    }

    //测试范围查找
    cout << "start to range search from 3 to 8" << endl;
    vector<pair<int, int>> rangeResult = skipList.rangeSearch(3, 8);
    cout << "range search result:" << endl;
    for (const auto& pair : rangeResult) {
        cout << "key: " << pair.first << ", value: " << pair.second << endl;
    }
    cout << "range search end" << endl;

    //测试移除,测试不通过
    key = 6;
    cout << endl << "start to remove" << endl;
    bool removeResult = skipList.remove(key);
    if (removeResult) {
        cout << "removed node whose key is " << key << endl;
    } else {
        cout << "removed failure" << endl;
    }

    cout << "start to range search from 3 to 8" << endl;
    rangeResult = skipList.rangeSearch(3, 8);
    cout << "range search result:" << endl;
    for (const auto& pair : rangeResult) {
        cout << "key: " << pair.first << ", value: " << pair.second << endl;
    }
    cout << "range search end" << endl;

    return 0;
}
