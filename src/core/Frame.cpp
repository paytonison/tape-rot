#include "core/Frame.hpp"

#include <limits>
#include <stdexcept>

namespace taperot {
namespace {

std::size_t checkedPlaneSize(std::size_t width, std::size_t height)
{
    if (width == 0 || height == 0) {
        throw std::invalid_argument("frame dimensions must be non-zero");
    }

    if (width > std::numeric_limits<std::size_t>::max() / height) {
        throw std::overflow_error("frame dimensions are too large");
    }

    return width * height;
}

} // namespace

Frame::Frame(std::size_t width, std::size_t height)
    : width_(width)
    , height_(height)
    , luma_(checkedPlaneSize(width, height), 0.0F)
    , chromaBlue_(luma_.size(), 0.0F)
    , chromaRed_(luma_.size(), 0.0F)
{
}

std::size_t Frame::width() const noexcept
{
    return width_;
}

std::size_t Frame::height() const noexcept
{
    return height_;
}

std::size_t Frame::planeSize() const noexcept
{
    return luma_.size();
}

bool Frame::empty() const noexcept
{
    return luma_.empty();
}

std::vector<float>& Frame::luma() noexcept
{
    return luma_;
}

const std::vector<float>& Frame::luma() const noexcept
{
    return luma_;
}

std::vector<float>& Frame::chromaBlue() noexcept
{
    return chromaBlue_;
}

const std::vector<float>& Frame::chromaBlue() const noexcept
{
    return chromaBlue_;
}

std::vector<float>& Frame::chromaRed() noexcept
{
    return chromaRed_;
}

const std::vector<float>& Frame::chromaRed() const noexcept
{
    return chromaRed_;
}

} // namespace taperot
