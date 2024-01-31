#include "ObjectPool.hpp"
#include <stdexcept>
namespace Game::Util {
template <typename T>
void ObjectPool<T>::AddObject(std::unique_ptr<T> object) {
    Pool_.push_back(std::move(object));
    FreeIndices_.push(Pool_.size() - 1);
    ObjectToIndex_.insert(std::make_pair(Pool_.back(), Pool_.size() - 1));
}

template <typename T>
void ObjectPool<T>::ReturnObject(std::shared_ptr<T> object) {
    auto it = ObjectToIndex_.find(object);
    if (it != ObjectToIndex_.end()) {
        FreeIndices_.push(it->second);
        return;
    }
    throw std::invalid_argument("Object not found in pool");
}

template <typename T>
void ObjectPool<T>::GetObjects(std::vector<std::shared_ptr<T>> &objects,
                               int32_t count) {
    if (count > FreeIndices_.size()) {
        throw std::out_of_range("Not enough objects in pool");
    }
    objects.reserve(objects.size() + count);
    for (int32_t i = 0; i < count; i++) {
        auto index = FreeIndices_.top();
        FreeIndices_.pop();
        objects.push_back(Pool_[index]);
    }
}

template <typename T>
std::shared_ptr<T> ObjectPool<T>::GetObject() {
    auto index = FreeIndices_.top();
    FreeIndices_.pop();
    return Pool_[index];
}
} // namespace Game::Util
