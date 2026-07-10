#include <iostream>
#include <optional>
#include <string>

#include "satellite/exit_codes.hpp"
#include "satellite/json_io.hpp"

namespace {

void usage() {
    std::cerr << "image-preprocess manifest|validate|run [--input file] [--work-dir dir] [--dry-run]\n";
}

}  // namespace

int main(int argc, char** argv) {
    if (argc < 2) {
        usage();
        return satellite::EXIT_USAGE;
    }
    const std::string cmd = argv[1];
    std::optional<std::filesystem::path> input;
    std::optional<std::filesystem::path> work_dir;
    bool dry_run = false;

    for (int i = 2; i < argc; ++i) {
        const std::string arg = argv[i];
        if (arg == "--input" && i + 1 < argc) {
            input = argv[++i];
        } else if (arg == "--work-dir" && i + 1 < argc) {
            work_dir = argv[++i];
        } else if (arg == "--dry-run") {
            dry_run = true;
        }
    }

    if (cmd == "manifest") {
        mp::write_json_stdout({
            {"schema_version", "1.0"},
            {"name", "image.preprocess"},
            {"executable", "image-preprocess"},
            {"version", "0.1.0"},
            {"commands", nlohmann::json::array({"manifest", "validate", "run"})},
            {"capabilities", {{"requires_gmat", false}, {"dry_run", true}}},
        });
        return satellite::EXIT_OK;
    }

    if (cmd == "validate") {
        if (!input) {
            std::cerr << "validate requires --input\n";
            return satellite::EXIT_VALIDATION;
        }
        const auto req = mp::read_json_file(*input);
        const bool ok = req.contains("input_path");
        mp::write_json_stdout({{"ok", ok}, {"message", ok ? "ok" : "input_path required"}});
        return ok ? satellite::EXIT_OK : satellite::EXIT_VALIDATION;
    }

    if (cmd == "run") {
        if (!input || !work_dir) {
            std::cerr << "run requires --input and --work-dir\n";
            return satellite::EXIT_VALIDATION;
        }
        const auto req = mp::read_json_file(*input);
        if (dry_run) {
            mp::write_json_stdout({{"status", "dry_run"}, {"tool", "image.preprocess"}});
            return satellite::EXIT_OK;
        }
        mp::write_json_stdout({
            {"status", "succeeded"},
            {"tool", "image.preprocess"},
            {"input_path", req.value("input_path", "")},
            {"output_path", (*work_dir / "preprocessed.tif").string()},
            {"warnings", nlohmann::json::array({"image.preprocess is a placeholder in v0.1.0"})},
        });
        return satellite::EXIT_OK;
    }

    usage();
    return satellite::EXIT_USAGE;
}
