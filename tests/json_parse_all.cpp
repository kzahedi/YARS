#include <gtest/gtest.h>

#include <array>
#include <cstdio>
#include <cstdlib>
#include <filesystem>
#include <fstream>
#include <sstream>
#include <string>
#include <vector>

// Parse-all test: invokes the yars binary on every JSON config listed in
// tests/json_corpus.txt and asserts that the parser+initial-setup pipeline
// completes for at least one simulation step. Configs that require an
// external controller (TCPIP, named pipes, language bindings) are skipped:
// they cannot run unattended without the controller running on the other
// side of the IPC channel.
//
// Source-of-truth corpus: tests/json_corpus.txt (relative to the repo root).
//
// Required environment / build state:
//   * YARS_BIN env var, or fallback to ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/yars
//   * Tests are invoked from build/ so xml/ resolves at ../xml/

namespace {

const std::vector<std::string> kSkipPatterns = {
    "_tcpip", "_named_pipe", "_go", "_julia", "_matlab", "_python",
    "muscle_tcpip", "_mpi",
    // Hexapod configs that load a controller library not built by default:
    // hexapod.json + hexapod_low.json reference the Python controller,
    // hexapod_crbm.json references the CRBM C++ controller. These can be
    // re-enabled once controller-build-verification declares them present.
    "hexapod.json", "hexapod_low.json", "hexapod_crbm.json"};

bool ShouldSkip(const std::string& path) {
  for (const auto& pat : kSkipPatterns) {
    if (path.find(pat) != std::string::npos) return true;
  }
  return false;
}

std::filesystem::path RepoRoot() {
  const char* env = std::getenv("YARS_REPO_ROOT");
  if (env && *env) return std::filesystem::path(env);
  // Tests are typically run from <build>/, so the repo root is one up.
  return std::filesystem::current_path().parent_path();
}

std::filesystem::path YarsBinary() {
  const char* env = std::getenv("YARS_BIN");
  if (env && *env) return std::filesystem::path(env);
  return std::filesystem::current_path() / "bin" / "yars";
}

std::vector<std::string> LoadCorpus() {
  std::vector<std::string> configs;
  const auto corpus_file = RepoRoot() / "tests" / "json_corpus.txt";
  std::ifstream in(corpus_file);
  if (!in) return configs;
  std::string line;
  while (std::getline(in, line)) {
    if (line.empty() || line[0] == '#') continue;
    configs.push_back(line);
  }
  return configs;
}

class XmlParseAllTest : public ::testing::TestWithParam<std::string> {};

TEST_P(XmlParseAllTest, ParsesAndInitializes) {
  const std::string rel_xml = GetParam();
  const auto repo = RepoRoot();
  const auto xml_abs = repo / rel_xml;
  const auto yars = YarsBinary();

  ASSERT_TRUE(std::filesystem::exists(xml_abs)) << "missing xml: " << xml_abs;
  ASSERT_TRUE(std::filesystem::exists(yars)) << "missing yars binary: " << yars;

  if (ShouldSkip(rel_xml)) {
    GTEST_SKIP() << "controller-dependent config; cannot parse without runtime peer: "
                 << rel_xml;
  }

  // 1 iteration is enough to validate parse + scene-graph construction.
  std::ostringstream cmd;
  cmd << '"' << yars.string() << '"'
      << " --nogui --iterations 1"
      << " --xml " << '"' << xml_abs.string() << '"'
      << " 2>&1";

  // Capture output in case of failure.
  std::array<char, 4096> buf{};
  std::string output;
  FILE* pipe = popen(cmd.str().c_str(), "r");
  ASSERT_NE(pipe, nullptr) << "popen failed";
  while (fgets(buf.data(), buf.size(), pipe)) output += buf.data();
  const int raw = pclose(pipe);
  const int exit_code = WEXITSTATUS(raw);

  EXPECT_EQ(exit_code, 0)
      << "yars exited non-zero parsing " << rel_xml
      << "\n---output---\n" << output;
}

INSTANTIATE_TEST_SUITE_P(
    Corpus, XmlParseAllTest, ::testing::ValuesIn(LoadCorpus()),
    [](const ::testing::TestParamInfo<std::string>& info) {
      std::string name = info.param;
      for (auto& c : name) {
        if (!std::isalnum(static_cast<unsigned char>(c))) c = '_';
      }
      return name;
    });

}  // namespace
