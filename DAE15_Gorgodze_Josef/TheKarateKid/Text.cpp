#include "pch.h"
#include "Text.h"

Text::Text(const std::string& fontPath, int size)
	: m_FontPath(fontPath)
	, m_FontSize(size)
{
}

Text::~Text()
{
	delete m_pTexture;
}

void Text::SetText(const std::string& text)
{
	delete m_pTexture;
	m_pTexture = new Texture(text, m_FontPath, m_FontSize, Color4f{ 1.f, 1.f, 1.f, 1.f });
}

void Text::Draw(const Vector2f& position) const
{
	if (m_pTexture)
	{
		m_pTexture->Draw(position);
	}
}
