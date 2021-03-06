#include "cat.h"

cache::cache(int cap) {
    cap_ = cap;
    lru_ = shared_ptr<lru>(new lru(cap_));
}

void cache::set(string key, any value) {
    lock_guard<mutex> lock(mutex_);
    if (lru_ == nullptr) {
        lru_ = shared_ptr<lru>(new lru(cap_));
    }
    lru_->set(key, value);
}

any cache::get(string key) {
    lock_guard<mutex> lock(mutex_);
    if (lru_ == nullptr) {
        return any();
    }
    return lru_->get(key);
}