#include "map.hpp"
#include <iostream>
#include <string>

using namespace cs440;

void insertTest(Map<int, int>& m) {
    std::cout << "insert test" << std::endl;
    if(m.empty()) { std::cout << "EMPTY" << std::endl; }
    m.insert({2, 1});
    m.insert({6, 2});
    m.insert({1, 6});
    m.insert({4, 8});
    std::cout << "duplicates begin" << std::endl;
    m.insert({2, 4});
    m.insert({4, 3});
    if(!(m.empty())) { std::cout << "NOT EMPTY" << std::endl; }
    m.clear();
    std::cout << "insert test END, cleared" << std::endl;
}

void BeginEndTest(Map<int, int>& m) {
    std::cout << "BE test" << std::endl;
    m.insert({3, 3});
    m.insert({6, 7});
    auto it = m.begin();
    std::cout << "begin key: " << (*it).first << "\n";
    it = m.end();
    std::cout << "end key: " << (*it).first << "\n";
    m.clear();
    std::cout << "BE test END, cleared\n" << std::endl;
}

void insertMany(Map<int, int>& m) {
    std::cout << "insert many test" << std::endl;
    for (int i = 0; i < 20; i++) {
        m.insert({i,i});
    }
    m.print();
    auto it = m.find(10);
    if (it != m.end()) {
        std::cout << "Found 10: " << (*it).second << "\n";
    }
    m.clear();
    std::cout << "insertmany END, cleared\n" << std::endl;
}


void findTest(Map<int, int>& m) {
    std::cout << "findTest" << std::endl;
    m.insert({6, 7});
    m.insert({4, 9});
    m.insert({2, 1});
    m.insert({9, 2});
    m.insert({1, 6});
    m.insert({7, 8});

    std::cout << "m.at(4) = 9 | " << m.at(4) << "\n";
    std::cout << "m.at(9) = 2 | " << m.at(9) << "\n";
    std::cout << "m.at(2) = 1 | " << m.at(2) << "\n";

    try {
        m.at(99);
        std::cout << "ERROR: Should have thrown!\n";
    } catch (std::out_of_range& e) {
        std::cout << "Exception caught correctly\n";
    }
    m.clear();
    std::cout << "findtest END, cleared\n" << std::endl;
}

void cotrInitTest() {
    Map<int, int> m1;
    m1.insert({1, 10});
    m1.insert({2, 20});
    
    Map<int, int> m2(m1);
    std::cout << "m2 size: " << m2.size() << "\n";  // 2
    std::cout << "m2[1]: " << m2.at(1) << "\n";  // 10
    
    // Test assignment
    Map<int, int> m3;
    m3 = m1;
    std::cout << "m3 size: " << m3.size() << "\n";  // 2
    
    // Test self-assignment
    m1 = m1;
    std::cout << "m1 still works: " << m1.size() << "\n";  // 2
    
    // Test initializer list
    Map<std::string, int> m4{{"hello", 1}, {"world", 2}};
    std::cout << "m4 size: " << m4.size() << "\n";  // 2
}

void comparisonTests() {
    Map<int, int> m1{{1, 10}, {2, 20}};
    Map<int, int> m2{{1, 10}, {2, 20}};
    Map<int, int> m3{{1, 10}, {2, 30}};
    
    std::cout << "m1 == m2: " << (m1 == m2) << "\n";  // true
    std::cout << "m1 != m3: " << (m1 != m3) << "\n";  // true
    std::cout << "m1 < m3: " << (m1 < m3) << "\n";    // true
}

int main() {
    /*
    Map<int, int> m;
    insertTest(m);
    BeginEndTest(m);
    insertMany(m);
    findTest(m);
    cotrInitTest();
    */
   comparisonTests();
    return 0;
}