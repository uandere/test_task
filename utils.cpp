//
// Created by nazar on 12.07.23.
//

#include <string>
#include <gst/gst.h>
#include "utils.h"

[[nodiscard]] std::pair<std::string, std::string> parse_args(int argc, char** argv) {
    std::string inputFile;
    std::string outputFile;

    // Checking if the correct number of arguments is provided, exiting otherwise
    if (argc != 3) {
        g_printerr("Usage: %s <input_file> <output_file>\n", argv[0]);
        exit(static_cast<int>(error_codes::WrongNumberOfArguments));
    }

    // Reading the command line arguments
    inputFile = argv[1];
    outputFile = argv[2];

    //checking output file extension: must be .mkv
    {
        auto point_iter = outputFile.find('.');
        if (point_iter == std::string::npos || point_iter == outputFile.size() - 1) {
            g_printerr("No output file extension\n");
            exit(static_cast<int>(error_codes::WrongOutputFileExt));
        }
        auto extension = outputFile.substr(point_iter + 1);
        if (extension != "mkv") {
            g_printerr("Bad output file extension (must be .mkv)\n");
            exit(static_cast<int>(error_codes::WrongOutputFileExt));
        }
    }

    return std::pair(inputFile, outputFile);
}
