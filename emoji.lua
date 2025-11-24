-- emoji.lua
-- Pandoc Lua filter to wrap emoji characters in \emojiChar{} without escaping

function is_emoji(char)
	local code = utf8.codepoint(char)
	return (code >= 0x1F600 and code <= 0x1F64F) -- Emoticons
		or (code >= 0x1F300 and code <= 0x1F5FF) -- Misc Symbols & Pictographs
		-- or (code >= 0x1F680 and code <= 0x1F6FF) -- Transport & Map
		-- or (code >= 0x2600 and code <= 0x26FF)   -- Misc symbols
		-- or (code >= 0x2700 and code <= 0x27BF)   -- Dingbats
		-- or (code >= 0xFE00 and code <= 0xFE0F)   -- Variation Selectors
		-- or (code >= 0x1F900 and code <= 0x1F9FF) -- Supplemental Symbols and Pictographs
		-- or (code >= 0x1FA70 and code <= 0x1FAFF) -- Symbols and Pictographs Extended-A
		-- or (code >= 0x200D and code <= 0x200D)   -- Zero Width Joiner (ZWJ)
		-- or (code >= 0x2300 and code <= 0x23FF)   -- Misc Technical (Hourglass, Watch, Keyboards, etc.)
		-- or (code >= 0x2B50 and code <= 0x2B50)   -- Star
		-- or (code >= 0x2194 and code <= 0x2199)   -- Arrows (some)
		-- or (code >= 0x2934 and code <= 0x2935)   -- Arrows (curved)
		or (code == 0x1F9E9)
		or (code == 0x2696)
		or (code == 0x1F9E0)
		or (code == 0x1F9EE)
end

function Str(el)
	local out = {}
	for _, c in utf8.codes(el.text) do
		local char = utf8.char(c)
		if is_emoji(char) then
			table.insert(out, pandoc.RawInline("latex", "\\emojiChar{" .. char .. "}"))
		else
			table.insert(out, pandoc.Str(char))
		end
	end
	return pandoc.Inlines(out)
end
