#pragma once
#include <Texture.h>

class Text
{
public:
	Text(const std::string& fontPath, int size);
	~Text();

	void SetText(const std::string& text);
	void Draw(const Vector2f& position) const;

private:
	Texture* m_pTexture{};
	std::string m_FontPath;
	int m_FontSize;
};

