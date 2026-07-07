#include "cli/Options.hpp"
#include "core/Frame.hpp"
#include "core/Random.hpp"
#include "effects/ChromaDegrade.hpp"
#include "effects/Dropout.hpp"
#include "effects/EffectPipeline.hpp"
#include "effects/HeadSwitchNoise.hpp"
#include "effects/LineJitter.hpp"

#include <cmath>
#include <exception>
#include <iostream>
#include <memory>
#include <sstream>
#include <string>
#include <string_view>
#include <utility>
#include <vector>

namespace {

using taperot::Frame;
using taperot::Random;

struct TestFailure final : public std::exception {
    explicit TestFailure(std::string message)
        : message_(std::move(message))
    {
    }

    const char* what() const noexcept override
    {
        return message_.c_str();
    }

    std::string message_;
};

void expect(bool condition, std::string_view message)
{
    if (!condition) {
        throw TestFailure(std::string(message));
    }
}

template <typename Left, typename Right>
void expectEqual(const Left& left, const Right& right, std::string_view message)
{
    if (!(left == right)) {
        std::ostringstream out;
        out << message;
        throw TestFailure(out.str());
    }
}

taperot::cli::ParseResult parseArgs(std::vector<std::string> args)
{
    std::vector<char*> argv;
    argv.reserve(args.size());
    for (auto& arg : args) {
        argv.push_back(arg.data());
    }

    return taperot::cli::parseOptions(static_cast<int>(argv.size()), argv.data());
}

Frame makePatternFrame(std::size_t width = 24, std::size_t height = 16)
{
    Frame frame(width, height);

    for (std::size_t y = 0; y < height; ++y) {
        for (std::size_t x = 0; x < width; ++x) {
            const auto index = y * width + x;
            frame.luma()[index] = static_cast<float>((x + y * 3) % 31) / 30.0F;
            frame.chromaBlue()[index] = static_cast<float>(static_cast<int>(x % 11) - 5) / 10.0F;
            frame.chromaRed()[index] = static_cast<float>(static_cast<int>((x * 2 + y) % 13) - 6) / 12.0F;
        }
    }

    return frame;
}

double planeDifference(const std::vector<float>& left, const std::vector<float>& right)
{
    expectEqual(left.size(), right.size(), "plane sizes must match");

    double total = 0.0;
    for (std::size_t index = 0; index < left.size(); ++index) {
        total += std::fabs(static_cast<double>(left[index] - right[index]));
    }
    return total;
}

double frameDifference(const Frame& left, const Frame& right)
{
    expectEqual(left.width(), right.width(), "frame widths must match");
    expectEqual(left.height(), right.height(), "frame heights must match");

    return planeDifference(left.luma(), right.luma()) +
        planeDifference(left.chromaBlue(), right.chromaBlue()) +
        planeDifference(left.chromaRed(), right.chromaRed());
}

double regionDifference(const Frame& left, const Frame& right, std::size_t startY, std::size_t endY)
{
    expectEqual(left.width(), right.width(), "region frame widths must match");
    expectEqual(left.height(), right.height(), "region frame heights must match");

    double total = 0.0;
    for (std::size_t y = startY; y < endY; ++y) {
        for (std::size_t x = 0; x < left.width(); ++x) {
            const auto index = y * left.width() + x;
            total += std::fabs(static_cast<double>(left.luma()[index] - right.luma()[index]));
            total +=
                std::fabs(static_cast<double>(left.chromaBlue()[index] - right.chromaBlue()[index]));
            total +=
                std::fabs(static_cast<double>(left.chromaRed()[index] - right.chromaRed()[index]));
        }
    }
    return total;
}

bool framesEqual(const Frame& left, const Frame& right)
{
    return left.width() == right.width() && left.height() == right.height() &&
        left.luma() == right.luma() && left.chromaBlue() == right.chromaBlue() &&
        left.chromaRed() == right.chromaRed();
}

template <typename Effect>
void expectPreservesDimensions(Effect& effect)
{
    Frame frame = makePatternFrame();
    const auto width = frame.width();
    const auto height = frame.height();
    const auto planeSize = frame.planeSize();
    Random random(1234);

    effect.apply(frame, random);

    expectEqual(frame.width(), width, "effect changed frame width");
    expectEqual(frame.height(), height, "effect changed frame height");
    expectEqual(frame.luma().size(), planeSize, "effect changed luma size");
    expectEqual(frame.chromaBlue().size(), planeSize, "effect changed blue chroma size");
    expectEqual(frame.chromaRed().size(), planeSize, "effect changed red chroma size");
}

template <typename MakeEffect>
void expectDeterministic(MakeEffect makeEffect)
{
    Frame first = makePatternFrame();
    Frame second = makePatternFrame();
    Random firstRandom(777);
    Random secondRandom(777);
    auto firstEffect = makeEffect();
    auto secondEffect = makeEffect();

    firstEffect.apply(first, firstRandom);
    secondEffect.apply(second, secondRandom);

    expect(framesEqual(first, second), "effect output should be deterministic for the same seed");
}

void testCliParsing()
{
    const auto valid = parseArgs({ "taperot", "input.mp4", "output.mp4", "--format", "betamax",
        "--generation", "3", "--tracking", "0.5", "--tape-wear", "1.0", "--chroma-bleed",
        "0.25", "--timebase-jitter", "0.75", "--dropouts", "0.4", "--head-switch-noise",
        "on", "--interlace", "off", "--composite", "off", "--seed", "12345" });

    expect(valid.options.has_value(), "valid CLI options should parse");
    expect(valid.error.empty(), "valid CLI options should not produce an error");
    expectEqual(valid.options->inputPath, std::string("input.mp4"), "input path should parse");
    expectEqual(valid.options->outputPath, std::string("output.mp4"), "output path should parse");
    expectEqual(taperot::cli::toString(valid.options->format), std::string("betamax"),
        "format should parse");
    expectEqual(valid.options->generation, 3, "generation should parse");
    expect(valid.options->headSwitchNoise, "head-switch-noise should parse");
    expectEqual(valid.options->seed, 12345U, "seed should parse");

    const auto invalidTracking =
        parseArgs({ "taperot", "input.mp4", "output.mp4", "--tracking", "2.0" });
    expect(!invalidTracking.options.has_value(), "invalid tracking should not parse");
    expect(!invalidTracking.error.empty(), "invalid tracking should explain the error");

    const auto invalidGeneration =
        parseArgs({ "taperot", "input.mp4", "output.mp4", "--generation", "0" });
    expect(!invalidGeneration.options.has_value(), "invalid generation should not parse");
    expect(!invalidGeneration.error.empty(), "invalid generation should explain the error");
}

void testRandomDeterminism()
{
    Random first(42);
    Random second(42);

    for (int index = 0; index < 10; ++index) {
        expectEqual(first.integer(-4, 9), second.integer(-4, 9),
            "integer random sequence should be deterministic");
    }

    for (int index = 0; index < 10; ++index) {
        expect(first.unit() == second.unit(), "unit random sequence should be deterministic");
    }
}

void testFrameDimensions()
{
    Frame frame(8, 5);

    expectEqual(frame.width(), 8U, "frame width should be stored");
    expectEqual(frame.height(), 5U, "frame height should be stored");
    expectEqual(frame.planeSize(), 40U, "frame plane size should be width times height");
    expectEqual(frame.luma().size(), 40U, "luma plane should match frame size");
    expectEqual(frame.chromaBlue().size(), 40U, "blue chroma plane should match frame size");
    expectEqual(frame.chromaRed().size(), 40U, "red chroma plane should match frame size");
}

void testEffectDimensionsAndDeterminism()
{
    taperot::effects::ChromaDegrade chroma({ 0.8 });
    taperot::effects::LineJitter jitter({ 0.8, 5 });
    taperot::effects::Dropout dropout({ 0.8 });
    taperot::effects::HeadSwitchNoise headSwitch({ 0.8, 0.20 });

    expectPreservesDimensions(chroma);
    expectPreservesDimensions(jitter);
    expectPreservesDimensions(dropout);
    expectPreservesDimensions(headSwitch);

    expectDeterministic([] { return taperot::effects::ChromaDegrade({ 0.8 }); });
    expectDeterministic([] { return taperot::effects::LineJitter({ 0.8, 5 }); });
    expectDeterministic([] { return taperot::effects::Dropout({ 0.8 }); });
    expectDeterministic([] { return taperot::effects::HeadSwitchNoise({ 0.8, 0.20 }); });
}

void testChromaDegrade()
{
    Frame before = makePatternFrame();
    Frame after = before;
    Random random(10);
    taperot::effects::ChromaDegrade effect({ 0.9 });

    effect.apply(after, random);

    const double lumaDelta = planeDifference(before.luma(), after.luma());
    const double chromaDelta = planeDifference(before.chromaBlue(), after.chromaBlue()) +
        planeDifference(before.chromaRed(), after.chromaRed());

    expect(chromaDelta > 0.0, "chroma degradation should modify chroma");
    expect(chromaDelta > lumaDelta * 4.0 + 0.001,
        "chroma degradation should modify chroma more than luma");
}

void testLineJitterChangesPixels()
{
    Frame before = makePatternFrame();
    Frame after = before;
    Random random(99);
    taperot::effects::LineJitter effect({ 1.0, 6 });

    effect.apply(after, random);

    expect(frameDifference(before, after) > 0.0, "line jitter should change a patterned frame");
}

void testDropoutChangesPixels()
{
    Frame before = makePatternFrame();
    Frame after = before;
    Random random(99);
    taperot::effects::Dropout effect({ 1.0 });

    effect.apply(after, random);

    expect(frameDifference(before, after) > 0.0, "dropout should change a patterned frame");
}

void testHeadSwitchNoiseBottomRegion()
{
    Frame before = makePatternFrame(24, 20);
    Frame after = before;
    Random random(99);
    taperot::effects::HeadSwitchNoise effect({ 1.0, 0.20 });

    effect.apply(after, random);

    const double topDelta = regionDifference(before, after, 0, 16);
    const double bottomDelta = regionDifference(before, after, 16, 20);

    expect(bottomDelta > 0.0, "head-switch noise should affect the bottom band");
    expect(topDelta == 0.0, "head-switch noise should not affect the upper frame");
}

class RecordingEffect final : public taperot::effects::Effect {
public:
    RecordingEffect(std::vector<int>& order, int value)
        : order_(order)
        , value_(value)
    {
    }

    std::string_view name() const noexcept override
    {
        return "recording effect";
    }

    void apply(Frame&, Random&) override
    {
        order_.push_back(value_);
    }

private:
    std::vector<int>& order_;
    int value_;
};

void testPipelineOrder()
{
    std::vector<int> order;
    taperot::effects::EffectPipeline pipeline;
    pipeline.add(std::make_unique<RecordingEffect>(order, 1));
    pipeline.add(std::make_unique<RecordingEffect>(order, 2));

    Frame frame = makePatternFrame();
    Random random(5);
    pipeline.apply(frame, random);

    expectEqual(pipeline.size(), 2U, "pipeline should report its size");
    expect(order == std::vector<int>({ 1, 2 }), "pipeline should apply effects in order");
}

} // namespace

int main()
{
    const std::vector<std::pair<std::string_view, void (*)()>> tests = {
        { "CLI option parsing", testCliParsing },
        { "random determinism", testRandomDeterminism },
        { "frame dimensions", testFrameDimensions },
        { "effect dimensions and determinism", testEffectDimensionsAndDeterminism },
        { "chroma degradation behavior", testChromaDegrade },
        { "line jitter behavior", testLineJitterChangesPixels },
        { "dropout behavior", testDropoutChangesPixels },
        { "head-switch noise locality", testHeadSwitchNoiseBottomRegion },
        { "pipeline order", testPipelineOrder },
    };

    int failures = 0;
    for (const auto& test : tests) {
        try {
            test.second();
            std::cout << "[pass] " << test.first << '\n';
        } catch (const std::exception& error) {
            ++failures;
            std::cerr << "[fail] " << test.first << ": " << error.what() << '\n';
        }
    }

    if (failures != 0) {
        std::cerr << failures << " test(s) failed\n";
        return 1;
    }

    std::cout << tests.size() << " test(s) passed\n";
    return 0;
}
