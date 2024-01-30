#ifndef OBJECT_POOL_HPP
#define OBJECT_POOL_HPP

#include <cstdint>
#include <memory>
#include <pch.hpp>
#include <sys/types.h>
#include <unordered_map>
#include <vector>

namespace Game {
template <typename T>
class ObjectPool {
public:
    ObjectPool() = default;

    ObjectPool(const ObjectPool &other) = delete;

    ObjectPool(ObjectPool &&other) = delete;

    ObjectPool &operator=(const ObjectPool &other) = delete;

    virtual ~ObjectPool() = default;

    void AddObject(std::unique_ptr<T> object);

    void ReturnObject(std::shared_ptr<T> object);

    void GetObjects(std::vector<std::shared_ptr<T>> &objects, int32_t count);

    std::shared_ptr<T> GetObject();

private:
    std::vector<std::shared_ptr<T>> Pool_;
    std::stack<size_t> FreeIndices_;
    std::unordered_map<std::shared_ptr<T>, size_t> ObjectToIndex_;
};
} // namespace Game

#endif
