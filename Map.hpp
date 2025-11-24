#ifndef MAP_HPP
#define MAP_HPP

#include <iostream>
#include <string>
#include <vector>
#include <random>
#include <cstdlib>
#include <ctime>

namespace cs440 {

template <typename Key_T, typename Mapped_T>
class Map {
public:
    using ValueType = std::pair<const Key_T, Mapped_T>;
    //forward declaration of other classes
    class Iterator;
    class ConstIterator;
    class ReverseIterator;

    //map functions
    Map() : max_level(0), map_length(0) {                                                       //cotr
        //std::cout << "Map()" << std::endl;
        //init all variables and object
        srand(time(nullptr));  
        head = new Node(MAX_LEVEL, Key_T{}, Mapped_T{});                                        //node level zero empty key and mapped
        tail = new Node(0, Key_T{}, Mapped_T{});
        for (int i = 0; i < MAX_LEVEL; i++) {
            head->next[i] = tail;
        }

    }
    Map(const Map& other) : Map() {                                                             //copy cotr
        for (auto it = other.begin(); it != other.end(); ++it) {
            insert(*it);
        }
    }
    Map(std::initializer_list<std::pair<const Key_T, Mapped_T>> init) : Map() {                 //initializer list contructor. For creation of map w/ intial values
        for (const auto& item : init) {
            insert(item);
        }

    }
    ~Map() {                                                                                    //destructor (just use clear and or free?)
        //std::cout <<"~Map()" << std::endl;
        clear();
        delete head;
        delete tail;
    }

    size_t size() const {return map_length;}                                                    //returns number of elements (using skip list so just size of layer 0?)
    bool empty() const {return map_length==0;}                                                  //T/F if empty

    //iterator functions
    Iterator begin() {                                                                          //returns an Iterator pointing to the first element, in order
        return Iterator(head->next[0], this);
    }
    Iterator end() {
        return Iterator(tail, this);
    }
    ConstIterator begin() const {                                                               //returns ConstIterator pointing towards first element
        return ConstIterator(head->next[0], this);
    }
    ConstIterator end() const {                                                                 //end but Const (ONE PAST THE LAST ELEMENT)
        return ConstIterator(tail, this);
    }

    ReverseIterator rbegin() {                                                                  //returns ReverseIterator to the first element in reverse order, last element in normal order
        Node* cur = head;
        while (cur->next[0] != tail) {
            cur = cur->next[0];
        }
        return ReverseIterator(cur, this);
    }

    ReverseIterator rend() {                                                                    //end but reverse to begining (ONE BEFORE THE FIRST ELEMENT)
        return ReverseIterator(head, this);
    }                                         

    //element access
    Iterator find(const Key_T& key) {                                                           //returns an iterator to the given key. If not found return end()
        Node* res = find_node(key);
        if (res != NULL) {
            return Iterator(res, this);
        }
        return Iterator(tail, this);
    }
    ConstIterator find(const Key_T& key) const {
        const Node* res = find_node(key);
        if (res != NULL) {
            return ConstIterator(res, this);
        }
        return ConstIterator(tail, this);
    }
    Mapped_T &at (const Key_T& key) {                                                           //returns reference to mapped object at specified key. If not found send std::out_of_range
        Node* node = find_node(key);
        if (!node) {
            throw std::out_of_range("key not found");
        }
        return const_cast<Mapped_T&>(node->data.second);
    }
    const Mapped_T &at (const Key_T& key) const {                                               //returns const reference " " " " " "
        const Node* node = find_node(key);
        if (!node) {
            throw std::out_of_range("key not found");
        }
        return node->data.second;

    }
    Mapped_T &operator[] (const Key_T& key) {                                                   //if key is in map, return a referece to the corresponding mapped object. If not, value initialized a mapped object for that key and return a reference to it (after insert)
        //std::cout << "opeerator[]" << '\n';
        auto res = insert({key, Mapped_T{}});
        //std::cout << "post insert!" << '\n';
        return const_cast<Mapped_T&>((*res.first).second);
    }

    //modifiers
    std::pair<Iterator, bool> insert(const ValueType& value) {                                  //inserts given pair into the map. If key doesnt already exist returns iterator pointing to new element and 'true'. If already exists no insertion is done but returns iterator pointing to the element with the key and 'false'
        Node* current = head;
        std::vector<Node*> update(MAX_LEVEL + 1); //vector to hold each level
        //std::cout << "insert: pre for" << '\n';
        for (int i = max_level; i >= 0; i--) {
            while (current->next[i] != tail && current->next[i]->data.first < value.first) {
                //std::cout << "i = " << i << '\n';
                current = current->next[i];
            }
            //for each level find the next largest at each level. save that location
            //std::cout << "insert: post while" << '\n';
            update[i] = current;
        }
        Node* next = current->next[0];
        //check dupe
        if (next != tail && next->data.first == value.first) {
            return {Iterator(next, this), false};
        }
        int new_level = random_level();
        if (new_level > max_level) {
            for (int i = max_level + 1; i <= new_level; i++) {
                update[i] = head;
            }
            max_level = new_level;
        }
        Node* newNode = new Node(new_level, value);
        for (int i = 0; i <= new_level; i++) {
            newNode->next[i] = update[i]->next[i];
            update[i]->next[i] = newNode;
        }

        map_length++;
        return {Iterator(newNode, this), true};
    }
    template <typename IT_T>
    void insert(IT_T range_beg, IT_T range_end) {                                               //inserts the given object or range of objects into the map. the range of object inserted includes the object range_beg points to but not range_end. Have to return std::vector<std::pair<Key_T, Mapped_T>>?
        for (auto it = range_beg; it != range_end; it++) {
            insert(ValueType((*it).first, (*it).second));
        }
    }                    
    void erase(Iterator pos) {                                                                  //removes given object from the map. If key not in map throw std::out_of_range
        if (pos.current == tail) {
            throw std::out_of_range("cannot erase tail");
        }
        erase(pos.current->data.first);
    }
    void erase(const Key_T& key) {
        Node* current = head;
        std::vector<Node*> update(MAX_LEVEL + 1);
        for (int i = max_level; i >= 0; i--) {
            while (current->next[i] != tail && current->next[i]->data.first < key) {
                current = current->next[i];
            }
    
            update[i] = current;
        }
        Node* next = current->next[0];
        //found confirm
        if (next == tail || !(next->data.first == key)) {
            throw std::out_of_range("Key not found");
        }

        for (int i = 0; i <= max_level; i++) {
            if (update[i]->next[i] != next) break;
            update[i]->next[i] = next->next[i];
        }

        delete next;

        while (max_level > 0 && head->next[max_level] == tail) {
            max_level--;
        }
        map_length--;
    }
    void clear() {                                                                              //resets map
        Node* current = head->next[0]; 
        while (current != tail) {
            Node * next = current->next[0];
            delete current;
            current = next;
        }
        //nullify all levels next
        for (int i = 0; i < MAX_LEVEL; i++) {
            head->next[i] = tail;
        }
        map_length = 0;
        max_level = 0;
    }

    void print() {                                                                              //debug print command
        std::cout << "Map hight = " << max_level << " | level 0 size = " << map_length << std::endl;
        Node* cur = head->next[0];
        while (cur != tail) {
            std::cout << "<" << cur->data.first << ", " << cur->data.second << ">, ";
            cur = cur->next[0];
        }
        
        std::cout << "\n";
    }

    void printHT() {                                                                            //print the head and tail only
        Node* cur = head->next[0];
        if (cur != tail) {
            std::cout << "HEAD KEY: " << cur->data.first;
        }
        while (cur->next[0] != tail) {
            cur = cur->next[0];
        }
        std::cout << " | TAIL KEY: " << cur->data.first << "\n";
    }

    //comparison (and theyre friends!)
    friend bool operator== (const Map& lhs, const Map& rhs) {                                   //equal if have same number of elements and all elements compare equal
        if (lhs.size() != rhs.size()) {
            return false;
        }
        auto it1 = lhs.begin();
        auto it2 = rhs.begin();

        while (it1 != lhs.end() && it2 != rhs.end()) {
            if ((*it1).first != (*it2).first || (*it1).second != (*it2).second) {
                return false;
            }
            ++it1;
            ++it2;
        }
        return true;
    }
    friend bool operator!= (const Map& lhs, const Map& rhs) {                                   //not equal
        return !(lhs == rhs);
    }
    friend bool operator< (const Map& lhs, const Map& rhs) {                                    //must use lexicographic sorting. Coorisponding elements are compared one by one. Then by size if otherwise equal. COMPAIRS THE PAIRS
        auto it1 = lhs.begin();
        auto it2 = rhs.begin();
        while (it1 != lhs.end() && it2 != rhs.end()) {
            if ((*it1).first < (*it2).first) return true;
            if ((*it2).first < (*it1).first) return false;

            if ((*it1).second < (*it2).second) return true;
            if ((*it2).second < (*it1).second) return false;
            //if equal overall keep on itterating
            ++it1;
            ++it2;
        }

        if ((it1 == lhs.end()) ^ (it2 != rhs.end())) {
            return it1 == rhs.end();
        }
        return false;
        
    }

private:
    //skiplist/node definitions
    static constexpr int MAX_LEVEL = 16;                                                        //determineable at runtime max level for skiplist
    static constexpr double P = 0.5;

    struct Node {
        ValueType data;
        std::vector<Node*> next; //because skiplist has multiple levels, need a vector for multiple different levels and forwards

        template<typename... Args>
        Node(int level, Args&&... args)                                                         //cotr. takes any number of args and puts them into data, then creates a vector with size level+1 (bc level 0 is base) and initilizes the value of each level to nullptr
            : data(std::forward<Args>(args)...), next(level+1, nullptr) {}
    };

    Node* head;                                                                                 //head of skiplist
    Node* tail;                                                                                 //tail needed aparently
    size_t max_level;                                                                           //levels
    size_t map_length;                                                                          //number of nodes

    Node* find_node(const Key_T& key) {
        Node* cur = head;
        for (int i = max_level; i >= 0; i--) {
            while (cur->next[i] != tail && cur->next[i]->data.first < key) {
                cur = cur->next[i];
            }
        }
        cur = cur->next[0];
        if (cur != tail && cur->data.first == key) {
            return cur;
        }
        return nullptr;
    }

    const Node* find_node(const Key_T& key) const {
        const Node* cur = head;
        for (int i = max_level; i >= 0; i--) {
            while (cur->next[i] != tail && cur->next[i]->data.first < key) {
                cur = cur->next[i];
            }
        }
        cur = cur->next[0];
        if (cur != tail && cur->data.first == key) {
            return cur;
        }
        return nullptr;
    }

    int random_level() {
        int lvl = 0;
        while ((rand() / double(RAND_MAX)) < P && lvl < MAX_LEVEL) {
            lvl++;
        }
        return lvl;
    }

};

template <typename Key_T, typename Mapped_T>
class Map<Key_T, Mapped_T>::Iterator {
public:
    Iterator(const Iterator &) = default;                                                       //copy constructor
    ~Iterator() = default;                                                                      //take a wild guess
    Iterator& operator=(const Iterator& other) = default;                                       //assignment
    Iterator &operator++() {                                                                    //increments iterator one lement and returns a reference to the incremented iterator (preincrement). Undefinded if poining to end of list
        if (current != map_ptr->tail) {
            current = current->next[0];
        }
        return *this;
    }

    Iterator &operator++(int) {                                                                 //increments iterator one element and returns an iterator pointing to the element prior to incrememnting the iterator (postcrement). Undefined if iterator is pointing to the end of the list
        Iterator temp = *this;
        ++(*this);
        return temp;

    }

    Iterator &operator--() {                                                                    //Decrements iterator one element returns a reference to the decremented iterator (predecrement). Undefined if iterator is pointing to begining of list. If iterator has the special value returned by the end() function, the iterator must point to the last element after this function
        if (current == map_ptr->tail) {
            Node* temp = map_ptr->head;
            while (temp->next[0] != map_ptr->tail) {
                temp = temp->next[0];
            }
            current = temp;
        }
        else {
            Node* temp = map_ptr->head;
            while (temp->next[0] != current) {
                temp = temp->next[0];
            }
            current = temp;
        }
        return *this;
    }

    Iterator &operator--(int) {                                                                 //decrements the iterator one element and returns iterator pointing to the element prior to decrementing (postdecrement). Undefined if pointing to beginning of the list. " " " "
        Iterator temp = *this;
        --(*this);
        return temp;
    }

    ValueType &operator*() const {                                                              //returns reference to the ValueType object containied in this object. Undefined if iterator is pointing to the end of the list. Can be used to changed the Mapped_T member of the element
        return const_cast<ValueType&>(current->data);
    } 
    ValueType *operator->() const {                                                             //special member access operator for the lement. If iterator is pointing to end of the list the behavior is undefined. Can be used to change Mappted_T member    
        return const_cast<ValueType&>(&(current->data));
    }
    friend bool operator==(const Iterator& lhs, const Iterator& rhs) {
        return lhs.current == rhs.current;
    }
    friend bool operator!=(const Iterator& lhs, const Iterator& rhs) {
        return !(lhs == rhs);
    }

private:
    friend class Map<Key_T, Mapped_T>;
    friend class ConstIterator;
    friend class ReverseIterator;
    Iterator(Node* node, const Map* mapPtr) : current(node), map_ptr(mapPtr) {}                 //private ctor for map
    Node* current;
    const Map* map_ptr;
};

template <typename Key_T, typename Mapped_T>
class Map<Key_T, Mapped_T>::ConstIterator {
public:
    ConstIterator(const ConstIterator &) = default;                                             //Your class must have a copy constructor, but you do not need to define this if the implicit one works for your implementation.
    ConstIterator(const Iterator& other) : current(other.current), map_ptr(other.map_ptr) {}    //conversion operator
    ~ConstIterator() = default;                                                                 //take another wild guess
    ConstIterator& operator=(const ConstIterator &) = default;                                  //Your class must have an assignment operator, but you do not need to define this if the implicit one works for your implementation
    ConstIterator &operator++() {                                                               //Increments the iterator one element, and returns a reference to the incremented iterator (preincrement). If the iterator is pointing to the end of the list, the behavior is undefined. 
        if (current != map_ptr->tail) {
            current = current->next[0];
        }
        return *this;
    }          

    ConstIterator operator++(int) {                                                             //Increments the iterator one element, and returns an iterator pointing to the element prior to incrementing the iterator (postincrement). If the iterator is pointing to the end of the list, the behavior is undefined. 
        ConstIterator temp = *this;
        ++(*this);
        return temp;
    }     

    ConstIterator &operator--() {                                                               //Decrements the iterator one element, and returns a reference to the decremented iterator (predecrement). If the iterator is pointing to the beginning of the list, the behavior is undefined. if the iterator has the special value returned by the end() function, then the iterator must point to the last element after this function. 
        if (current == map_ptr->tail) {
            Node* temp = map_ptr->head;
            while (temp->next[0] != map_ptr->tail) {
                temp = temp->next[0];
            }
            current = temp;
        }
        else {
            Node* temp = map_ptr->head;
            while (temp->next[0] != current) {
                temp = temp->next[0];
            }
            current = temp;
        }
        return *this;
    }

    ConstIterator operator--(int) {                                                             //Decrements the iterator one element, and returns an iterator pointing to the element prior to decrementing (postdecrement). If the iterator is pointing to the beginning of the list, the behavior is undefined. if the iterator has the special value returned by the end() function, then the iterator must point to the last element after this function. 
        ConstIterator temp = *this;
        --(*this);
        return temp;
    }      

    const ValueType &operator*() const {                                                        //Returns a reference to the current element of the iterator. If the iterator is pointing to the end of the list, the behavior is undefined.
        return current->data;
    }

    const ValueType *operator->() const {                                                       //Special member access operator for the element. If the iterator is pointing to the end of the list, the behavior is undefined. 
        return &(current->data);
    }                            

    //comparison operators
    friend bool operator==(const ConstIterator& lhs, const ConstIterator& rhs) {
        return lhs.current == rhs.current;
    }
    friend bool operator==(const Iterator& lhs, const ConstIterator& rhs) {
        return lhs.current == rhs.current;
    }
    friend bool operator==(const ConstIterator& lhs, const Iterator& rhs) {
        return lhs.current == rhs.current;
    }

    friend bool operator!=(const ConstIterator& lhs, const ConstIterator& rhs) {
        return !(lhs == rhs);
    }
    friend bool operator!=(const Iterator& lhs, const ConstIterator& rhs) {
        return !(lhs == rhs);
    }
    friend bool operator!=(const ConstIterator& lhs, const Iterator& rhs) {
        return !(lhs == rhs);
    }

private:
    friend class Map<Key_T, Mapped_T>;
    const Node* current;
    const Map* map_ptr;
    ConstIterator(const Node* node, const Map* map) : current(node), map_ptr(map) {}
};

template <typename Key_T, typename Mapped_T>
class Map<Key_T, Mapped_T>::ReverseIterator {
public:
    ReverseIterator(const ReverseIterator &) = default;                                         //copy constructor
    ~ReverseIterator() = default;                                                               //take a third wild guess
    ReverseIterator& operator=(const ReverseIterator &) = default;                              //assignment
    ReverseIterator &operator++() {                                                             //icrements and returns reference to the incremented iterator (preincrement). Undefined if pointing to the end
        //-- logic but with head special case
        if (current != map_ptr->head) {
            Node* temp = map_ptr->head;
            while (temp->next[0] != current) {
                temp = temp->next[0];
            }
            current = temp;
        }
        return *this;
        
    }

    ReverseIterator operator++(int) {                                                           //increments and returns iterator pointing to the element prior to incrementing (postincrement), Undefined if iterator is pointing to end of the list
        ReverseIterator temp = *this;
        ++(*this);
        return temp;
    }

    ReverseIterator &operator--() {                                                             //If the iterator is pointing to the beginning of the list, the behavior is undefined. If the iterator has the special value returned by the end() function, then the iterator must point to the last element after this function. 
        if(current != map_ptr->tail) {
            current = current->next[0];
        }
        return *this;
    }

    ReverseIterator operator--(int) {                                                           //Decrements the iterator one element, and returns an iterator pointing to the element prior to decrementing (postdecrement). If the iterator is pointing to the beginning of the list, the behavior is undefined. If the iterator has the special value returned by the end() function, then the iterator must point to the last element after this function. 
        ReverseIterator temp = *this;
        --(*this);
        return temp;
    }

    ValueType &operator*() const {                                                              //Returns a reference to the ValueType object contained in this element of the list. If the iterator is pointing to the end of the list, the behavior is undefined. This can be used to change the Mapped_T member of the element. 
        return const_cast<ValueType&>(current->data);
    }
    
    ValueType *operator->() const {                                                             //Special member access operator for the element. If the iterator is pointing to the end of the list, the behavior is undefined. This can be used to change the Mapped_T member of the element. 
        return const_cast<ValueType&>(&(current->data));
    }

    //reverseiterator comparison operators
    friend bool operator==(const ReverseIterator& lhs, const ReverseIterator& rhs) {
        return lhs.current == rhs.current;
    }

    friend bool operator!=(const ReverseIterator& lhs, const ReverseIterator& rhs) {
        return !(lhs == rhs);
    }

private:
    friend class Map<Key_T, Mapped_T>;
    Node* current;
    const Map* map_ptr;
    ReverseIterator(Node* node, const Map* map) : current(node), map_ptr(map) {}
};
}

#endif