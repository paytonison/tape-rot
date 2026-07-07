#pragma once

#include <cstdint>
#include <optional>
#include <string>

namespace taperot::cli {

enum class TapeFormat {
    Vhs,
    Betamax,
    Umatic,
    Broadcast,
    Camcorder
};

struct Options {
    std::string inputPath;
    std::string outputPath;
    TapeFormat format = TapeFormat::Vhs;
    int generation = 1;
    double tracking = 0.0;
    double tapeWear = 0.0;
    double chromaBleed = 0.0;
    double timebaseJitter = 0.0;
    double dropouts = 0.0;
    bool headSwitchNoise = false;
    bool interlace = false;
    bool composite = false;
    std::uint32_t seed = 0;
};

struct ParseResult {
    std::optional<Options> options;
    bool helpRequested = false;
    std::string error;
};

ParseResult parseOptions(int argc, char* argv[]);

std::string usage();
std::string describeProcessingPlan(const Options& options);
std::string toString(TapeFormat format);

} // namespace taperot::cli
