#include "cli/Options.hpp"

#include <charconv>
#include <cmath>
#include <limits>
#include <sstream>
#include <string_view>
#include <vector>

namespace taperot::cli {
namespace {

bool isOption(std::string_view value)
{
    return value.rfind("--", 0) == 0;
}

std::optional<TapeFormat> parseFormat(std::string_view value)
{
    if (value == "vhs") {
        return TapeFormat::Vhs;
    }
    if (value == "betamax") {
        return TapeFormat::Betamax;
    }
    if (value == "umatic") {
        return TapeFormat::Umatic;
    }
    if (value == "broadcast") {
        return TapeFormat::Broadcast;
    }
    if (value == "camcorder") {
        return TapeFormat::Camcorder;
    }
    return std::nullopt;
}

std::optional<bool> parseOnOff(std::string_view value)
{
    if (value == "on") {
        return true;
    }
    if (value == "off") {
        return false;
    }
    return std::nullopt;
}

std::optional<int> parsePositiveInt(std::string_view value)
{
    int parsed = 0;
    const auto* begin = value.data();
    const auto* end = value.data() + value.size();
    const auto result = std::from_chars(begin, end, parsed);
    if (result.ec != std::errc() || result.ptr != end || parsed < 1) {
        return std::nullopt;
    }
    return parsed;
}

std::optional<std::uint32_t> parseSeed(std::string_view value)
{
    std::uint64_t parsed = 0;
    const auto* begin = value.data();
    const auto* end = value.data() + value.size();
    const auto result = std::from_chars(begin, end, parsed);
    if (result.ec != std::errc() || result.ptr != end ||
        parsed > std::numeric_limits<std::uint32_t>::max()) {
        return std::nullopt;
    }
    return static_cast<std::uint32_t>(parsed);
}

std::optional<double> parseUnitRange(std::string_view value)
{
    std::string text(value);
    std::size_t consumed = 0;

    try {
        const double parsed = std::stod(text, &consumed);
        if (consumed != text.size() || !std::isfinite(parsed) || parsed < 0.0 || parsed > 1.0) {
            return std::nullopt;
        }
        return parsed;
    } catch (...) {
        return std::nullopt;
    }
}

std::string requireValue(int argc, char* argv[], int index, std::string_view option)
{
    const int valueIndex = index + 1;
    if (valueIndex >= argc || isOption(argv[valueIndex])) {
        return "missing value for " + std::string(option);
    }
    return {};
}

template <typename Assign>
bool parseRangedOption(
    int argc,
    char* argv[],
    int& index,
    std::string_view option,
    std::string& error,
    Assign assign)
{
    error = requireValue(argc, argv, index, option);
    if (!error.empty()) {
        return false;
    }

    const auto parsed = parseUnitRange(argv[index + 1]);
    if (!parsed.has_value()) {
        error = std::string(option) + " must be a number from 0.0 to 1.0";
        return false;
    }

    assign(*parsed);
    ++index;
    return true;
}

template <typename Assign>
bool parseOnOffOption(
    int argc,
    char* argv[],
    int& index,
    std::string_view option,
    std::string& error,
    Assign assign)
{
    error = requireValue(argc, argv, index, option);
    if (!error.empty()) {
        return false;
    }

    const auto parsed = parseOnOff(argv[index + 1]);
    if (!parsed.has_value()) {
        error = std::string(option) + " must be either 'on' or 'off'";
        return false;
    }

    assign(*parsed);
    ++index;
    return true;
}

} // namespace

std::string toString(TapeFormat format)
{
    switch (format) {
    case TapeFormat::Vhs:
        return "vhs";
    case TapeFormat::Betamax:
        return "betamax";
    case TapeFormat::Umatic:
        return "umatic";
    case TapeFormat::Broadcast:
        return "broadcast";
    case TapeFormat::Camcorder:
        return "camcorder";
    }

    return "unknown";
}

std::string usage()
{
    return R"(Usage:
  taperot <input> <output> [options]

Options:
  --format <vhs|betamax|umatic|broadcast|camcorder>
  --generation <number>
  --tracking <0.0-1.0>
  --tape-wear <0.0-1.0>
  --chroma-bleed <0.0-1.0>
  --timebase-jitter <0.0-1.0>
  --dropouts <0.0-1.0>
  --head-switch-noise <on|off>
  --interlace <on|off>
  --composite <on|off>
  --seed <integer>
  --help

Current scaffold:
  Validates arguments and prints the requested processing plan.
  The CLI does not decode, mutate, or encode video yet.
)";
}

ParseResult parseOptions(int argc, char* argv[])
{
    Options options;
    std::vector<std::string> positional;
    ParseResult result;

    for (int index = 1; index < argc; ++index) {
        const std::string_view arg(argv[index]);

        if (arg == "--help") {
            result.helpRequested = true;
            return result;
        }

        if (!isOption(arg)) {
            positional.emplace_back(arg);
            continue;
        }

        if (arg == "--format") {
            result.error = requireValue(argc, argv, index, arg);
            if (!result.error.empty()) {
                return result;
            }

            const auto parsed = parseFormat(argv[index + 1]);
            if (!parsed.has_value()) {
                result.error =
                    "--format must be one of: vhs, betamax, umatic, broadcast, camcorder";
                return result;
            }
            options.format = *parsed;
            ++index;
            continue;
        }

        if (arg == "--generation") {
            result.error = requireValue(argc, argv, index, arg);
            if (!result.error.empty()) {
                return result;
            }

            const auto parsed = parsePositiveInt(argv[index + 1]);
            if (!parsed.has_value()) {
                result.error = "--generation must be a positive integer";
                return result;
            }
            options.generation = *parsed;
            ++index;
            continue;
        }

        if (arg == "--tracking") {
            if (!parseRangedOption(argc, argv, index, arg, result.error,
                    [&](double value) { options.tracking = value; })) {
                return result;
            }
            continue;
        }

        if (arg == "--tape-wear") {
            if (!parseRangedOption(argc, argv, index, arg, result.error,
                    [&](double value) { options.tapeWear = value; })) {
                return result;
            }
            continue;
        }

        if (arg == "--chroma-bleed") {
            if (!parseRangedOption(argc, argv, index, arg, result.error,
                    [&](double value) { options.chromaBleed = value; })) {
                return result;
            }
            continue;
        }

        if (arg == "--timebase-jitter") {
            if (!parseRangedOption(argc, argv, index, arg, result.error,
                    [&](double value) { options.timebaseJitter = value; })) {
                return result;
            }
            continue;
        }

        if (arg == "--dropouts") {
            if (!parseRangedOption(argc, argv, index, arg, result.error,
                    [&](double value) { options.dropouts = value; })) {
                return result;
            }
            continue;
        }

        if (arg == "--head-switch-noise") {
            if (!parseOnOffOption(argc, argv, index, arg, result.error,
                    [&](bool value) { options.headSwitchNoise = value; })) {
                return result;
            }
            continue;
        }

        if (arg == "--interlace") {
            if (!parseOnOffOption(argc, argv, index, arg, result.error,
                    [&](bool value) { options.interlace = value; })) {
                return result;
            }
            continue;
        }

        if (arg == "--composite") {
            if (!parseOnOffOption(argc, argv, index, arg, result.error,
                    [&](bool value) { options.composite = value; })) {
                return result;
            }
            continue;
        }

        if (arg == "--seed") {
            result.error = requireValue(argc, argv, index, arg);
            if (!result.error.empty()) {
                return result;
            }

            const auto parsed = parseSeed(argv[index + 1]);
            if (!parsed.has_value()) {
                result.error = "--seed must be an integer from 0 to 4294967295";
                return result;
            }
            options.seed = *parsed;
            ++index;
            continue;
        }

        result.error = "unknown option: " + std::string(arg);
        return result;
    }

    if (positional.size() != 2) {
        result.error = "expected input and output paths";
        return result;
    }

    options.inputPath = positional[0];
    options.outputPath = positional[1];
    result.options = options;
    return result;
}

std::string describeProcessingPlan(const Options& options)
{
    std::ostringstream out;

    out << "Tape Rot processing plan\n";
    out << "  input: " << options.inputPath << '\n';
    out << "  output: " << options.outputPath << '\n';
    out << "  format: " << toString(options.format) << '\n';
    out << "  generation: " << options.generation << '\n';
    out << "  tracking: " << options.tracking << '\n';
    out << "  tape wear: " << options.tapeWear << '\n';
    out << "  chroma bleed: " << options.chromaBleed << '\n';
    out << "  timebase jitter: " << options.timebaseJitter << '\n';
    out << "  dropouts: " << options.dropouts << '\n';
    out << "  head-switch noise: " << (options.headSwitchNoise ? "on" : "off") << '\n';
    out << "  interlace: " << (options.interlace ? "on" : "off") << '\n';
    out << "  composite: " << (options.composite ? "on" : "off") << '\n';
    out << "  seed: " << options.seed << '\n';
    out << '\n';
    out << "Pipeline scaffold\n";
    out << "  decode video: not implemented\n";
    out << "  frame model: planar luma/chroma buffers\n";
    out << "  effect stages: chroma degradation, line jitter, dropouts, head-switch noise\n";
    out << "  encode video: not implemented\n";
    out << '\n';
    out << "No video was processed. This build only validates options and prints the plan.\n";

    return out.str();
}

} // namespace taperot::cli
