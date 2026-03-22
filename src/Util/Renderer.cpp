#include "Util/Renderer.hpp"

#include <queue>

#include "Util/Image.hpp"
#include "Util/Logger.hpp"
#include "Util/TransformUtils.hpp"

namespace Util {
Renderer::Renderer(const std::vector<std::shared_ptr<GameObject>> &children)
    : m_Children(children) {}

void Renderer::AddChild(const std::shared_ptr<GameObject> &child) {
    m_Children.push_back(child);
}

void Renderer::RemoveChild(std::shared_ptr<GameObject> child) {
    m_Children.erase(std::remove(m_Children.begin(), m_Children.end(), child),
                     m_Children.end());
}

void Renderer::AddChildren(
    const std::vector<std::shared_ptr<GameObject>> &children) {
    m_Children.reserve(m_Children.size() + children.size());
    m_Children.insert(m_Children.end(), children.begin(), children.end());
}

void Renderer::Update() {
    // Lazily initialize the SpriteBatch
    if (!m_Batch) {
        m_Batch = std::make_unique<Core::SpriteBatch>(8192);
    }

    struct StackInfo {
        std::shared_ptr<GameObject> m_GameObject;
        Transform m_ParentTransform;
    };

    std::vector<StackInfo> stack;
    stack.reserve(m_Children.size());

    for (const auto &child : m_Children) {
        stack.push_back(StackInfo{child, Transform{}});
    }

    auto compareFunction = [](const StackInfo &a, const StackInfo &b) {
        return a.m_GameObject->GetZIndex() > b.m_GameObject->GetZIndex();
    };
    std::priority_queue<StackInfo, std::vector<StackInfo>,
                        decltype(compareFunction)>
        renderQueue(compareFunction);

    while (!stack.empty()) {
        auto curr = stack.back();
        stack.pop_back();
        renderQueue.push(curr);

        for (const auto &child : curr.m_GameObject->GetChildren()) {
            stack.push_back(
                StackInfo{child, curr.m_GameObject->GetTransform()});
        }
    }

    glm::mat4 viewProjection = Util::ComputeViewProjection();

    // Begin batched rendering
    Image::SetBatch(m_Batch.get());
    m_Batch->Begin(viewProjection);

    // Draw all in render queue by z-order
    while (!renderQueue.empty()) {
        auto curr = renderQueue.top();
        renderQueue.pop();

        curr.m_GameObject->Draw();
    }

    // Flush batched draw calls
    m_Batch->End();
    Image::SetBatch(nullptr);
}

int Renderer::GetDrawCallCount() const {
    return m_Batch ? m_Batch->GetDrawCallCount() : 0;
}
} // namespace Util
