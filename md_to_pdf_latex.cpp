#include <cstdlib>
#include <filesystem>
#include <fstream>
#include <iostream>
#include <regex>
#include <set>
#include <sstream>
#include <string>
#include <vector>

namespace fs = std::filesystem;

// \usepackage{fontspec}
// \directlua{
//   luaotfload.add_fallback("emojifallback", {
//     "Segoe UI Emoji:mode=harf;"
//   })
// }
// \setmainfont{Segoe UI}[RawFeature={fallback=emojifallback}]

// LaTeX header content for emoji support via font fallback
const std::string LATEX_HEADER = R"(
\hyphenpenalty=10000
\exhyphenpenalty=10000

\pagenumbering{gobble} % This line disables page numbers for the entire document

\usepackage{fontspec}
\newfontfamily\emoji{Noto Color Emoji}[Renderer=Harfbuzz]
\newcommand{\emojiChar}[1]{{\emoji #1}}
)";

void run_command(const std::string &cmd)
{
    std::cout << "Running: " << cmd << std::endl;
    int ret = std::system(cmd.c_str());
    if (ret != 0)
    {
        std::cerr << "Command failed with exit code: " << ret << std::endl;
        exit(1);
    }
}

void update_emoji_lua(const std::set<std::string> &missing_codes)
{
    std::string lua_file = "emoji.lua";
    std::ifstream in(lua_file);
    if (!in)
    {
        std::cerr << "Could not open " << lua_file << " for reading." << std::endl;
        return;
    }
    std::stringstream buffer;
    buffer << in.rdbuf();
    std::string content = buffer.str();
    in.close();

    // Find the end of the is_emoji function
    size_t func_pos = content.find("function is_emoji");
    if (func_pos == std::string::npos)
        return;

    // Search for "\nend" to avoid matching "end" inside comments (e.g. "Extended-A")
    size_t end_pos = content.find("\nend", func_pos);
    if (end_pos == std::string::npos)
        return;

    std::string insertions = "";
    for (const auto &code : missing_codes)
    {
        insertions += "\n\t\tor (code == 0x" + code + ")";
    }

    content.insert(end_pos, insertions);

    std::ofstream out(lua_file);
    out << content;
    out.close();
    std::cout << "Updated " << lua_file << " with " << missing_codes.size() << " new missing emoji codes." << std::endl;
}

int main(int argc, char *argv[])
{
    if (argc < 2)
    {
        std::cerr << "Usage: " << argv[0] << " <input_markdown_file>" << std::endl;
        return 1;
    }

    std::string input_file = argv[1];
    fs::path input_path(input_file);
    std::string stem = input_path.stem().string();
    std::string output_pdf = stem + "_latex.pdf";
    std::string header_file = "emoji_header.tex";
    std::string log_file = "pandoc_log.txt";

    // 1. Create temporary header file
    std::ofstream header_out(header_file);
    header_out << LATEX_HEADER;
    header_out.close();

    // 2. Run Pandoc with LuaLaTeX and the header, capturing stderr
    // Using --listings for code blocks as per user attempts
    // Using eisvogel template if available, but user had issues. Let's try default first or user's template if they
    // want. The user's command: pandoc raz.md -o raz.tex --from markdown --template eisvogel --listings
    // --lua-filter=emoji2.lua --pdf-engine=lualatex We will omit the lua filter and use our header instead. We'll stick
    // to default template for now to ensure it works, unless we want to try 'eisvogel'. Let's try to use 'eisvogel' if
    // it's installed, but fallback to default might be safer. Given the user specifically mentioned "eisvogel", let's
    // try to include it but maybe make it optional or just use default for reliability first. Actually, the user's
    // issue was emojis. Let's solve that. The template is secondary but nice. I'll use the default template to minimize
    // variables, but inject the header.

    std::string pandoc_cmd =
        "pandoc \"" + input_file + "\" -o \"" + output_pdf +
        "\" --from markdown -V geometry:margin=1in --lua-filter=emoji.lua --pdf-engine=lualatex -H " + header_file +
        " --listings";

    // Run first time
    run_command(pandoc_cmd + " 2> " + log_file);

    // 3. Check for missing characters
    std::ifstream log_in(log_file);
    std::string line;
    std::regex re(R"(U\+([0-9A-Fa-f]+))");
    std::smatch match;
    std::set<std::string> missing_codes;

    while (std::getline(log_in, line))
    {
        if (line.find("[WARNING] Missing character:") != std::string::npos)
        {
            if (std::regex_search(line, match, re))
            {
                if (match.size() > 1)
                {
                    missing_codes.insert(match.str(1));
                }
            }
        }
    }
    log_in.close();

    // 4. If missing codes found, update lua and retry
    if (!missing_codes.empty())
    {
        std::cout << "Found missing characters. Updating emoji.lua..." << std::endl;
        update_emoji_lua(missing_codes);
        std::cout << "Re-running pandoc..." << std::endl;
        run_command(pandoc_cmd); // No need to capture log second time unless we want to loop, but once should be enough
                                 // for now.
    }

    // 5. Cleanup
    // fs::remove(header_file);
    // fs::remove(log_file);

    std::cout << "Successfully created " << output_pdf << std::endl;

    return 0;
}
