#include <unordered_map>
#include <iostream>
#include <any>
#include <string>
#include <list>
#include <shared_mutex>

using namespace std;

class db {
public:
    virtual any get(string key) = 0;
    virtual void set(string key, any value) = 0;
    virtual void del(string key) = 0;
    virtual ~db() = 0;
};

db::~db() {}

class lru: public db {
private:
    shared_mutex mutex_;
    size_t cap_;
    unordered_map<string, any> kvmap_;
    unordered_map<string, list<string>::iterator> kimap_;
    list<string> list_;
public:
    lru(size_t cap = 1024);
    any get(string key);
    void set(string key, any value);
    void del(string key);
    ~lru();
};

lru::lru(size_t cap) {
    cap_ = cap;
}

any lru::get(string key) {
    shared_lock<shared_mutex> lock(mutex_);
    cout << "db get key: " << key << endl;
    if (kimap_.find(key) == kimap_.end()) {
        cout << "not found key: " << key << endl;
        return any();
    }
    list_.erase(kimap_[key]);
    list_.push_front(key);
    kimap_[key] = list_.begin();
    return kvmap_[key];
}

void lru::set(string key, any value) {
    unique_lock<shared_mutex> lock(mutex_);
    cout << "db set key: " << key << endl;
    if (kimap_.find(key) == kimap_.end()) {
        if (list_.size() == cap_) {
            del(list_.back());
        }
    } else {
        list_.erase(kimap_[key]);
    }
    list_.push_front(key);
    kvmap_[key] = value;
    kimap_[key] = list_.begin();
}

void lru::del(string key) {
    unique_lock<shared_mutex> lock(mutex_);
    cout << "db del key: " << key << endl;
    if (kimap_.find(key) == kimap_.end()) {
        return;
    }
    list_.erase(kimap_[key]);
    kimap_.erase(key);
    kvmap_.erase(key);
}

lru::~lru() {}

int main() {
    db* idb = new lru();
    idb->set("a", 1);
    cout << any_cast<int>(idb->get("a")) << endl;
    delete idb;
}