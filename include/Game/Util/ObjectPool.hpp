#ifndef OBJECT_POOL_HPP
#define OBJECT_POOL_HPP

#include <cstdint>
#include <memory>
#include <pch.hpp>
#include <sys/types.h>
#include <unordered_map>
#include <vector>

namespace Game::Util {
template <class T>
class ObjectPool {
public:
    ObjectPool() = default;

    virtual ~ObjectPool() = default;

    void AddObject(std::shared_ptr<T> object);

    void ReturnObject(std::shared_ptr<T> object);

    void GetObjects(std::vector<std::shared_ptr<T>> &objects, int32_t count);

    std::shared_ptr<T> GetObject();

private:
    std::vector<std::shared_ptr<T>> Pool_;
    std::stack<size_t> FreeIndices_;
    std::unordered_map<std::shared_ptr<T>, size_t> ObjectToIndex_;
};
} // namespace Game::Util

#endif
