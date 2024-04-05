#include "pch.h"
#include <drawing.hpp>

void RemoveCorlorFormatText(std::string& str) {
	for (size_t i = 0; i < str.size(); i++) {
		if (str[i] == '^') {
			str.erase(i, 2);
			i--;  // Decrement i to account for the erased character
		}
	}
}
void DrawOutlinedText(ImFont* font, const std::string& text, const ImVec2& pos, float size, ImU32 color, int Shadow)
{
	if (Shadow == 1 || Shadow == 3)//shadow top
		ImGui::GetBackgroundDrawList()->AddText(font, size, ImVec2(pos.x - 1, pos.y - 1), IM_COL32(0, 0, 0, 128), text.c_str());
	if (Shadow == 2 || Shadow == 3)//shadow bottom
		ImGui::GetBackgroundDrawList()->AddText(font, size, ImVec2(pos.x + 1, pos.y + 1), IM_COL32(0, 0, 0, 128), text.c_str());
	//real text
	ImGui::GetBackgroundDrawList()->AddText(font, size, pos, ImGui::GetColorU32(color), text.c_str());

}
inline ImU32 GetColorFromCode(char code, ImU32 default_) {
	switch (code) {
	case '1': return IM_COL32(255, 68, 51, 255); //^1 Red orange
	case '2': return IM_COL32(144, 238, 144, 255); //^2 Light Green
	case '3': return IM_COL32(255, 192, 0, 255); // ^3  Light Yellow
	case '4': return IM_COL32(0, 0, 255, 255); //^4  Dark Blue
	case '5': return IM_COL32(0, 150, 255, 255); //^5 Light Blue
	case '6': return IM_COL32(127, 0, 255, 255); // ^6 Violet
	case '7': return IM_COL32(255, 255, 255, 255); // ^7 // White
	case '8': return IM_COL32(136, 8, 8, 255); // ^8 // Blood Red
	case '9': return IM_COL32(255, 0, 255, 255); // ^9 // Fuchsia
	default: return default_; // Default to original setting
	}
}
auto DrawTextWithColorsCode(ImFont* pFont, const std::string& text, const ImVec2& pos, float size, ImU32 color, bool center, int Shadow) -> VOID
{
	std::stringstream stream(text);
	std::string line;

	float y = 0.0f;
	int index = 0;

	while (std::getline(stream, line))
	{
		std::string copiedText(line);
		auto OrgTextFormat = copiedText.c_str();
		RemoveCorlorFormatText(line);

		ImVec2 textSize = pFont->CalcTextSizeA(size, 300, 0.0f, line.c_str());
		ImVec2 drawPos;
		if (center) drawPos = ImVec2(pos.x - textSize.x / 2.0f, pos.y + textSize.y * index);
		else drawPos = ImVec2(pos.x, pos.y + textSize.y * index);

		ImGuiTextBuffer buffer;
		ImU32 textColor = color;

		while (*OrgTextFormat) {
			if (*OrgTextFormat == '^' && *(OrgTextFormat + 1) >= '0' && *(OrgTextFormat + 1) <= '9')
			{
				if (!buffer.empty())
				{
					DrawOutlinedText(pFont, buffer.c_str(), drawPos, size, textColor, Shadow);//ImGui::GetColorU32(color)
					ImVec2 thisSize = pFont->CalcTextSizeA(size, 300, 0.0f, buffer.c_str());
					drawPos.x += thisSize.x;//next draw pos after this text
					buffer.clear();//done draw clear for retrieve next text
				}
				textColor = GetColorFromCode(*(OrgTextFormat + 1), color);//convert this color 
				OrgTextFormat += 2; // Skip the '^' and the color code
			}
			else {
				buffer.appendf("%c", *OrgTextFormat);
				OrgTextFormat++;
			}
		}
		DrawOutlinedText(pFont, buffer.c_str(), drawPos, size, textColor, Shadow);

		y = pos.y + textSize.y * (index + 1);
		index++;
	}
}