#include <filesystem>
#include <iostream>
#include <optional>
#include <string>

#include "satellite/exit_codes.hpp"
#include "satellite/json_io.hpp"

namespace {

/** @brief 打印用法到 stderr。 */
void usage() {
    std::cerr << "image-preprocess manifest|validate|run [--input file] "
                 "[--work-dir dir] [--dry-run]\n";
}

}  // namespace

int main(int argc, char** argv) {
    if (argc < 2) {
        usage();
        return satellite::EXIT_USAGE;
    }
    const std::string                    cmd = argv[1];
    std::optional<std::filesystem::path> input;
    std::optional<std::filesystem::path> work_dir;
    bool                                 dry_run = false;

    for (int i = 2; i < argc; ++i) {
        const std::string arg = argv[i];
        if (arg == "--input" && i + 1 < argc) { input = argv[++i]; }
        else if (arg == "--work-dir" && i + 1 < argc) { work_dir = argv[++i]; }
        else if (arg == "--dry-run") { dry_run = true; }
    }

    if (cmd == "manifest") {
        satellite::write_json_stdout({
            {"schema_version", "1.1"},
            {"name", "image.preprocess"},
            {"executable", "image-preprocess"},
            {"version", "0.1.0"},
            {"description",
             "Placeholder remote sensing image preprocessing plugin"},
            {"domain", "image"},
            {"safety_class", "planning_only"},
            {"commands",
             nlohmann::json::array({"manifest", "validate", "run"})},
            {"scenarios", nlohmann::json::array({"image_preprocess"})},
            {"capabilities",
             {{"kind", "actuate"},
              {"side_effect_class", "reversible"},
              {"relocatable", true},
              {"deterministic", false},
              {"idempotent", false},
              {"retryable", true},
              {"consumes", nlohmann::json::array({"DataProduct"})},
              {"produces", nlohmann::json::array({"DataProduct"})},
              {"hardware_tag", nullptr},
              {"timeout_sec", 300},
              {"compensation", nullptr},
              {"async", true},
              {"dry_run", true},
              {"cancel", "process_signal"},
              {"requires_gmat", false},
              {"batch", true}}},
        });
        return satellite::EXIT_OK;
    }

    if (cmd == "validate") {
        if (!input) {
            std::cerr << "validate requires --input\n";
            return satellite::EXIT_VALIDATION;
        }
        const auto req = satellite::read_json_file(*input);
        const bool ok  = req.contains("input_path") &&
                        req.at("input_path").is_string() &&
                        !req.at("input_path").get<std::string>().empty();
        satellite::write_json_stdout(
            {{"ok", ok},
             {"message", ok ? "ok" : "input_path required and non-empty"}});
        return ok ? satellite::EXIT_OK : satellite::EXIT_VALIDATION;
    }

    if (cmd == "run") {
        if (!input || !work_dir) {
            std::cerr << "run requires --input and --work-dir\n";
            return satellite::EXIT_VALIDATION;
        }
        const auto req = satellite::read_json_file(*input);
        if (!req.contains("input_path") || !req.at("input_path").is_string() ||
            req.at("input_path").get<std::string>().empty()) {
            std::cerr << "run requires non-empty input_path in request JSON\n";
            return satellite::EXIT_VALIDATION;
        }
        if (dry_run) {
            satellite::write_json_stdout(
                {{"status", "dry_run"}, {"tool", "image.preprocess"}});
            return satellite::EXIT_OK;
        }
        satellite::write_json_stdout({
            {"status", "succeeded"},
            {"tool", "image.preprocess"},
            {"input_path", req.at("input_path").get<std::string>()},
            {"output_path", (*work_dir / "preprocessed.tif").string()},
            {"warnings", nlohmann::json::array(
                             {"image.preprocess is a placeholder in v0.1.0"})},
        });
        return satellite::EXIT_OK;
    }

    usage();
    return satellite::EXIT_USAGE;
}
