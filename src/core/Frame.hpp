#pragma once

#include <cstddef>
#include <vector>

namespace taperot {

class Frame {
public:
    Frame() = default;
    Frame(std::size_t width, std::size_t height);

    std::size_t width() const noexcept;
    std::size_t height() const noexcept;
    std::size_t planeSize() const noexcept;
    bool empty() const noexcept;

    std::vector<float>& luma() noexcept;
    const std::vector<float>& luma() const noexcept;

    std::vector<float>& chromaBlue() noexcept;
    const std::vector<float>& chromaBlue() const noexcept;

    std::vector<float>& chromaRed() noexcept;
    const std::vector<float>& chromaRed() const noexcept;

private:
    std::size_t width_ = 0;
    std::size_t height_ = 0;
    std::vector<float> luma_;
    std::vector<float> chromaBlue_;
    std::vector<float> chromaRed_;
};

} // namespace taperot
