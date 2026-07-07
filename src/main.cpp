#include "cli/Options.hpp"

#include <iostream>

int main(int argc, char* argv[])
{
    const auto result = taperot::cli::parseOptions(argc, argv);

    if (result.helpRequested) {
        std::cout << taperot::cli::usage();
        return 0;
    }

    if (!result.error.empty()) {
        std::cerr << "error: " << result.error << "\n\n";
        std::cerr << taperot::cli::usage();
        return 2;
    }

    if (!result.options.has_value()) {
        std::cerr << "error: no processing options were produced\n";
        return 2;
    }

    std::cout << taperot::cli::describeProcessingPlan(*result.options);
    return 0;
}
