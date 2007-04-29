//////////////////////////////////////////////////////////////////////
// Wendy user interface library
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

#include <moira/Moira.h>

#include <wendy/Config.h>

#include <wendy/OpenGL.h>
#include <wendy/GLContext.h>
#include <wendy/GLCanvas.h>
#include <wendy/GLTexture.h>
#include <wendy/GLVertex.h>
#include <wendy/GLBuffer.h>
#include <wendy/GLLight.h>
#include <wendy/GLShader.h>
#include <wendy/GLPass.h>
#include <wendy/GLRender.h>

#include <wendy/RenderFont.h>

#include <wendy/UIRender.h>
#include <wendy/UIWidget.h>
#include <wendy/UIBook.h>

///////////////////////////////////////////////////////////////////////

namespace wendy
{
  namespace UI
  {
  
///////////////////////////////////////////////////////////////////////

using namespace moira;

///////////////////////////////////////////////////////////////////////

Page::Page(const String& initText):
  text(initText)
{
}

const String& Page::getText(void) const
{
  return text;
}

void Page::setText(const String& newText)
{
  text = newText;
}

void Page::draw(void) const
{
  const Rectangle& area = getGlobalArea();

  Renderer* renderer = Renderer::get();
  if (renderer->pushClipArea(area))
  {
    WidgetState state;
    if (isEnabled())
      state = STATE_NORMAL;
    else
      state = STATE_DISABLED;

    renderer->drawFrame(area, state);

    Widget::draw();

    renderer->popClipArea();
  }
}

///////////////////////////////////////////////////////////////////////

Book::Book(void):
  activePage(NULL)
{
  getButtonClickedSignal().connect(*this, &Book::onButtonClicked);
  getAreaChangedSignal().connect(*this, &Book::onAreaChanged);
}

Page* Book::getActivePage(void) const
{
  return activePage;
}

void Book::setActivePage(Page* newPage)
{
  setActivePage(newPage, false);
}

SignalProxy1<void, Book&> Book::getPageChangedSignal(void)
{
  return pageChangedSignal;
}

void Book::draw(void) const
{
  const Rectangle& area = getGlobalArea();

  Renderer* renderer = Renderer::get();
  if (renderer->pushClipArea(area))
  {
    PageList pages;
    getPages(pages);

    if (!pages.empty())
    {
      const float em = Renderer::get()->getDefaultEM();

      const Vector2& size = getArea().size;

      const float width = size.x / pages.size();
      const float height = em * 2.f;

      for (unsigned int i = 0;  i < pages.size();  i++)
      {
	Rectangle buttonArea;
	buttonArea.set(width * i, size.y - height, width, height);

	WidgetState state;
	if (isEnabled())
	{
	  if (activePage == pages[i])
	    state = STATE_ACTIVE;
	  else
	    state = STATE_NORMAL;
	}
	else
	  state = STATE_DISABLED;

	renderer->drawButton(buttonArea, state, pages[i]->getText());
      }
    }

    Widget::draw();

    renderer->popClipArea();
  }
}

void Book::addedChild(Widget& child)
{
  if (Page* page = dynamic_cast<Page*>(&child))
  {
    const float em = Renderer::get()->getDefaultEM();

    const Vector2& size = getArea().size;

    page->setArea(Rectangle(0.f, 0.f, size.x, size.y - em * 2.f));

    if (activePage)
      page->setVisible(false);
    else
      setActivePage(page, false);
  }
}

void Book::removedChild(Widget& child)
{
  if (Page* page = dynamic_cast<Page*>(&child))
  {
    if (page == activePage)
    {
      PageList pages;
      getPages(pages);

      if (pages.empty())
	setActivePage(NULL, false);
      else
	setActivePage(pages.front(), false);
    }
  }
}

void Book::getPages(PageList& pages) const
{
  const List& children = getChildren();

  pages.reserve(children.size());

  for (List::const_iterator i = children.begin();  i != children.end();  i++)
  {
    if (Page* page = dynamic_cast<Page*>(*i))
      pages.push_back(page);
  }
}

void Book::setActivePage(Page* newPage, bool notify)
{
  if (newPage == activePage)
    return;

  const List& children = getChildren();

  if (std::find(children.begin(), children.end(), newPage) == children.end())
    return;

  if (activePage)
    activePage->setVisible(false);

  activePage = newPage;
  activePage->setVisible(true);
  activePage->activate();

  if (notify)
    pageChangedSignal.emit(*this);
}

void Book::onAreaChanged(Widget& widget)
{
  PageList pages;
  getPages(pages);

  const float em = Renderer::get()->getDefaultEM();

  const Vector2& size = getArea().size;

  for (PageList::const_iterator i = pages.begin();  i != pages.end();  i++)
    (*i)->setArea(Rectangle(0.f, 0.f, size.x, size.y - em * 2.f));
}

void Book::onButtonClicked(Widget& widget,
		           const Vector2& point,
		           unsigned int button,
		           bool clicked)
{
  PageList pages;
  getPages(pages);

  const float position = transformToLocal(point).x;
  const float width = getArea().size.x / pages.size();

  const unsigned int index = (unsigned int) (position / width);

  setActivePage(pages[index], true);
}

///////////////////////////////////////////////////////////////////////

  } /*namespace UI*/
} /*namespace wendy*/

///////////////////////////////////////////////////////////////////////