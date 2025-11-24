# Markdown to PDF Converter with Atom One Dark & Emoji Support

This project provides two C++ tools to convert Markdown files to PDF, each with a different approach to styling and emoji support.

## Tools

### 1. HTML-based Converter (`md_to_pdf_html.cpp`)

Converts Markdown to PDF via HTML using Chromium.

- **Features**:
  - **Atom One Dark** color scheme.
  - **MathJax** support for LaTeX equations.
  - **System Fonts** (Arial/Roboto/Segoe UI) for clean rendering.
  - **Emoji Support** via system fonts.
- **Process**: Markdown -> Pandoc (HTML) -> Chromium (PDF).

### 2. LaTeX-based Converter (`md_to_pdf_latex.cpp`)

Converts Markdown to PDF via LuaLaTeX.

- **Features**:
  - **Auto-Emoji Fixer**: Automatically detects missing emoji warnings from Pandoc/LaTeX and updates `emoji.lua` to include them.
  - **Lua Filter**: Wraps emojis in a custom command to ensure they use the correct font.
  - **High-quality Typesetting** via LaTeX.
- **Process**: Markdown -> Pandoc (LuaLaTeX) -> PDF.

## Prerequisites

Ensure you have the following installed and in your system PATH:

- **C++ Compiler** (g++)
- **Pandoc**
- **Chromium** (for HTML converter)
- **LuaLaTeX** (usually part of TeX Live or MiKTeX)
- **Fonts**: `Noto Color Emoji` or `Segoe UI Emoji` (for emojis).

## Compilation

Compile the programs using `g++`:

```bash
g++ md_to_pdf_html.cpp -o md2pdf_html.exe
g++ md_to_pdf_latex.cpp -o md2pdf_latex.exe
```

## Usage

### HTML Method

```bash
./md2pdf_html.exe <input_file.md>
```

Output: `<filename>_html.pdf`

### LaTeX Method

```bash
./md2pdf_latex.exe <input_file.md>
```

Output: `<filename>_latex.pdf`

_Note: If the LaTeX converter encounters missing emojis, it will automatically update `emoji.lua` and retry the conversion._

## Files

- `md_to_pdf_html.cpp`: Source for HTML converter.
- `md_to_pdf_latex.cpp`: Source for LaTeX converter.
- `emoji.lua`: Pandoc Lua filter for handling emojis.
- `emoji_header.tex`: LaTeX header definitions.
