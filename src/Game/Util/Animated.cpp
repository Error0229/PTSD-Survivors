#include "Game/Util/Animated.hpp"
#include "Game/Config.hpp"
#include "Util/Image.hpp"
#include <cstdint>
#include <linux/soundcard.h>
#include <stdexcept>

namespace Game::Util {
std::shared_ptr<Animation> Animated::GetAnimation(const std::string &name) {
    if (name == NULL_STRING) {
        if (m_CurrentAnimation == NULL_STRING) {
            throw std::logic_error("Animation not found");
        }
        return m_Animation[m_CurrentAnimation];
    }
    auto it = m_Animation.find(name);
    if (it == m_Animation.end()) {
        throw std::logic_error("Animation not found");
    }
    return it->second;
}

void Animated::Load(const std::string &name,
                    const std::vector<std::string> &frames, bool isLoop,
                    time_t frameTime) {
    std::vector<std::shared_ptr<::Util::Image>> images;
    for (auto &frame : frames) {
        images.push_back(std::make_shared<::Util::Image>(frame));
    }
    m_CurrentAnimation = name;
    m_Animation[name] = std::make_shared<Animation>(images, isLoop, frameTime);
}

void Animated::Load(const std::string &name,
                    std::shared_ptr<Animation> animation) {
    m_Animation[name] = animation;
}

void Animated::SetAnimation(const std::string &name) {
    if (m_Animation.find(name) == m_Animation.end())
        throw std::logic_error("Animation not found");
    m_CurrentAnimation = name;
}
void Animated::Play(const std::string &name) {
    if (auto anim = GetAnimation(name)) {
        anim->Play();
    }
}

void Animated::Stop(const std::string &name) {
    if (auto anim = GetAnimation(name)) {
        anim->Stop();
    }
}
void Animated::Pause(const std::string &name) {
    if (auto anim = GetAnimation(name)) {
        anim->Pause();
    }
}
void Animated::SetFrame(int32_t frame, const std::string &name) {
    if (auto anim = GetAnimation(name)) {
        anim->SetFrame(frame);
    }
}
int32_t Animated::GetFrame(const std::string &name) {
    if (auto anim = GetAnimation(name)) {
        return anim->GetFrame();
    }
    throw std::logic_error("Animation not found");
}
int32_t Animated::FrameCount(const std::string &name) {
    if (auto anim = GetAnimation(name)) {
        return anim->FrameCount();
    }
    throw std::logic_error("Animation not found");
}
void Animated::Update(const std::string &name) {
    if (auto anim = GetAnimation(name)) {
        anim->Update();
    }
}
} // namespace Game::Util
