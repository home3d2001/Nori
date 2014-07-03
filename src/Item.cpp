//////////////////////////////////////////////////////////////////////
// Wendy - a simple game engine
// Copyright (c) 2007 Camilla Berglund <elmindreda@elmindreda.org>
//
// This software is provided 'as-is', without any express or implied
// warranty. In no event will the authors be held liable for any
// damages arising from the use of this software.
//
// Permission is granted to anyone to use this software for any
// purpose, including commercial applications, and to alter it and
// redistribute it freely, subject to the following restrictions:
//
//  1. The origin of this software must not be misrepresented; you
//     must not claim that you wrote the original software. If you use
//     this software in a product, an acknowledgment in the product
//     documentation would be appreciated but is not required.
//
//  2. Altered source versions must be plainly marked as such, and
//     must not be misrepresented as being the original software.
//
//  3. This notice may not be removed or altered from any source
//     distribution.
//
///////////////////////////////////////////////////////////////////////

#include <wendy/Config.hpp>

#include <wendy/Drawer.hpp>
#include <wendy/Layer.hpp>
#include <wendy/Widget.hpp>
#include <wendy/Item.hpp>

///////////////////////////////////////////////////////////////////////

namespace wendy
{

///////////////////////////////////////////////////////////////////////

Item::Item(Layer& layer, const char* value, ItemID id):
  m_layer(layer),
  m_value(value),
  m_id(id)
{
}

Item::~Item()
{
}

bool Item::operator < (const Item& other) const
{
  return m_value < other.m_value;
}

float Item::width() const
{
  Font& font = m_layer.drawer().theme().font();
  return font.height() * 2.f + font.boundsOf(m_value.c_str()).size.x;
}

float Item::height() const
{
  return m_layer.drawer().theme().em() * 1.5f;
}

ItemID Item::id() const
{
  return m_id;
}

const String& Item::value() const
{
  return m_value;
}

void Item::setValue(const char* newValue)
{
  m_value = newValue;
  m_layer.invalidate();
}

void Item::draw(const Rect& area, WidgetState state) const
{
  Drawer& drawer = m_layer.drawer();
  Font& font = drawer.theme().font();
  const float em = font.height();
  const Rect textArea(area.position + vec2(em / 2.f, 0.f),
                      area.size - vec2(em, 0.f));

  if (state == STATE_SELECTED)
  {
    const vec3 color = drawer.theme().backgroundColor(STATE_SELECTED);
    drawer.fillRectangle(area, vec4(color, 1.f));
  }

  drawer.setCurrentFont(nullptr);
  drawer.drawText(textArea, m_value.c_str(), LEFT_ALIGNED, state);
}

///////////////////////////////////////////////////////////////////////

SeparatorItem::SeparatorItem(Layer& layer):
  Item(layer)
{
}

float SeparatorItem::width() const
{
  return m_layer.drawer().theme().em() * 2.f;
}

float SeparatorItem::height() const
{
  return m_layer.drawer().theme().em() / 2.f;
}

void SeparatorItem::draw(const Rect& area, WidgetState state) const
{
  Drawer& drawer = m_layer.drawer();

  const vec2 start(area.position.x, area.position.y + area.size.y / 2.f);
  const vec2 end(area.position.x + area.size.x,
                 area.position.y + area.size.y / 2.f);

  drawer.drawLine(start, end, vec4(vec3(0.f), 1.f));
}

///////////////////////////////////////////////////////////////////////

TextureItem::TextureItem(Layer& layer,
                         Texture& texture,
                         const char* name,
                         ItemID ID):
  Item(layer, name, ID),
  m_texture(&texture)
{
}

float TextureItem::width() const
{
  return Item::width() + m_layer.drawer().theme().em() * 3.f;
}

float TextureItem::height() const
{
  return m_layer.drawer().theme().em() * 3.f;
}

Texture& TextureItem::texture() const
{
  return *m_texture;
}

void TextureItem::draw(const Rect& area, WidgetState state) const
{
  Drawer& drawer = m_layer.drawer();
  const float em = drawer.theme().em();
  const Rect textArea(area.position + vec2(em * 3.5f, 0.f),
                      area.size - vec2(em, 0.f));

  if (state == STATE_SELECTED)
  {
    const vec3 color = drawer.theme().backgroundColor(STATE_SELECTED);
    drawer.fillRectangle(area, vec4(color, 1.f));
  }

  drawer.setCurrentFont(nullptr);
  drawer.drawText(textArea, value().c_str(), LEFT_ALIGNED, state);

  const Rect textureArea(area.position, vec2(em * 3.f));
  drawer.blitTexture(textureArea, *m_texture);
}

///////////////////////////////////////////////////////////////////////

} /*namespace wendy*/

///////////////////////////////////////////////////////////////////////
