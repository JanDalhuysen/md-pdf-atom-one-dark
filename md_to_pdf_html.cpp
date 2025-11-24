#include <cstdlib>
#include <filesystem>
#include <fstream>
#include <iostream>
#include <string>
#include <vector>

namespace fs = std::filesystem;

// Atom One Dark Colors
const std::string CSS_STYLE = R"(
<style>
    @import url('https://fonts.googleapis.com/css2?family=JetBrains+Mono:wght@400;700&family=Nunito:wght@400;700&display=swap');

    :root {
        --black: #181a1f;
        --bg0: #282c34;
        --bg1: #31353f;
        --bg2: #393f4a;
        --bg3: #3b3f4c;
        --bg-d: #21252b;
        --bg-blue: #73b8f1;
        --bg-yellow: #ebd09c;
        --fg: #abb2bf;
        --purple: #c678dd;
        --green: #98c379;
        --orange: #d19a66;
        --blue: #61afef;
        --yellow: #e5c07b;
        --cyan: #56b6c2;
        --red: #e86671;
        --grey: #5c6370;
        --light-grey: #848b98;
        --dark-cyan: #2b6f77;
        --dark-red: #993939;
        --dark-yellow: #93691d;
        --dark-purple: #8a3fa0;
    }

    @page { margin: 0; }

    body {
        background-color: var(--bg0);
        color: var(--fg);
        /* font-family: 'Nunito', 'Segoe UI Emoji', 'Noto Color Emoji', sans-serif; */
        /* font-family: Arial, sans-serif; */
        font-family: 'Roboto', sans-serif;
        line-height: 1.6;
        padding: 2rem;
        max-width: 800px;
        margin: 0 auto;
    }

    h1, h2, h3, h4, h5, h6 {
        color: var(--blue);
        margin-top: 1.5em;
    }

    h1 { border-bottom: 1px solid var(--bg2); padding-bottom: 0.3em; }
    
    code {
        font-family: 'JetBrains Mono', monospace;
        background-color: var(--bg1);
        padding: 0.2em 0.4em;
        border-radius: 3px;
        color: var(--green);
    }

    pre {
        background-color: var(--bg1);
        padding: 1em;
        border-radius: 5px;
        overflow-x: auto;
    }

    pre code {
        background-color: transparent;
        padding: 0;
        color: var(--fg);
    }

    a { color: var(--cyan); text-decoration: none; }
    a:hover { text-decoration: underline; }

    blockquote {
        border-left: 4px solid var(--purple);
        padding-left: 1em;
        color: var(--grey);
        margin-left: 0;
    }

    hr {
        border: none;
        border-top: 1px solid var(--bg2);
        margin: 2em 0;
    }

    strong { color: var(--orange); }
    em { color: var(--yellow); }
    
    /* Scrollbar for web viewing (optional) */
    ::-webkit-scrollbar { width: 10px; }
    ::-webkit-scrollbar-track { background: var(--bg0); }
    ::-webkit-scrollbar-thumb { background: var(--bg3); border-radius: 5px; }
</style>
<script id="MathJax-script" async src="https://cdn.jsdelivr.net/npm/mathjax@4/tex-mml-chtml.js"></script>

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
    std::string temp_body = stem + "_body.html";
    std::string temp_full = stem + "_full.html";
    std::string output_pdf = stem + "_html.pdf";

    // 1. Convert Markdown to HTML body using Pandoc
    // Using --no-highlight to handle styling via CSS manually or let pandoc generate classes
    // Adding --mathjax for math support if needed, and smart typography
    std::string pandoc_cmd = "pandoc \"" + input_file + "\" -o \"" + temp_body + "\" --from markdown+emoji --mathjax";
    run_command(pandoc_cmd);

    // 2. Read the body content
    std::ifstream body_in(temp_body);
    if (!body_in)
    {
        std::cerr << "Failed to read generated HTML body." << std::endl;
        return 1;
    }
    std::string body_content((std::istreambuf_iterator<char>(body_in)), std::istreambuf_iterator<char>());
    body_in.close();

    // 3. Construct full HTML
    std::ofstream full_out(temp_full);
    full_out << "<!DOCTYPE html>\n<html>\n<head>\n<meta charset=\"UTF-8\">\n";
    full_out << CSS_STYLE;
    full_out << "\n</head>\n<body>\n";
    full_out << body_content;
    full_out << "\n</body>\n</html>";
    full_out.close();

    // 4. Convert HTML to PDF using Chromium
    // Note: --no-pdf-header-footer removes default headers
    // --print-to-pdf requires absolute path or filename in current dir
    std::string chromium_cmd =
        "chromium --headless --disable-gpu --print-to-pdf=\"" + output_pdf + "\" \"" + temp_full +
        "\" --no-pdf-header-footer --run-all-compositor-stages-before-draw --virtual-time-budget=10000";

    // Attempt to find chromium or chrome if simple 'chromium' fails?
    // For now, assuming 'chromium' is in PATH as per user attempts.
    // Windows users might have 'chrome' instead. Let's try a fallback or just use what user had.
    // User had: chromium --headless ...
    run_command(chromium_cmd);

    // 5. Cleanup
    // fs::remove(temp_body);
    // fs::remove(temp_full);

    std::cout << "Successfully created " << output_pdf << std::endl;

    return 0;
}
