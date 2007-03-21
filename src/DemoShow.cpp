///////////////////////////////////////////////////////////////////////
// Wendy demo system
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
#include <wendy/GLTexture.h>
#include <wendy/GLLight.h>
#include <wendy/GLShader.h>
#include <wendy/GLCanvas.h>
#include <wendy/GLPass.h>

#include <wendy/RenderFont.h>
#include <wendy/RenderStyle.h>

#include <wendy/UIRender.h>
#include <wendy/UIWidget.h>

#include <wendy/DemoParameter.h>
#include <wendy/DemoEffect.h>
#include <wendy/DemoShow.h>

///////////////////////////////////////////////////////////////////////

namespace wendy
{
  namespace demo
  {
  
///////////////////////////////////////////////////////////////////////

using namespace moira;

///////////////////////////////////////////////////////////////////////

void Show::addEffect(Effect& effect)
{
  if (std::find(effects.begin(), effects.end(), &effect) == effects.end())
    effects.push_back(&effect);
}

void Show::removeEffect(Effect& effect)
{
  EffectList::iterator i = std::find(effects.begin(), effects.end(), &effect);
  if (i != effects.end())
    effects.erase(i);
}

void Show::prepare(void) const
{
  for (EffectList::const_iterator i = effects.begin();  i != effects.end();  i++)
    (*i)->prepare();
}

void Show::render(void) const
{
  for (EffectList::const_iterator i = effects.begin();  i != effects.end();  i++)
    (*i)->render();
}

const String& Show::getTitle(void) const
{
  return title;
}

void Show::setTitle(const String& newTitle)
{
  title = newTitle;
}

Time Show::getDuration(void) const
{
  Time duration = 0.0;

  for (EffectList::const_iterator i = effects.begin();  i != effects.end();  i++)
    duration = std::max(duration, (*i)->getDuration());

  return duration;
}

Time Show::getTimeElapsed(void) const
{
  return elapsed;
}

void Show::setTimeElapsed(Time newTime)
{
  elapsed = std::min(newTime, getDuration());

  for (EffectList::const_iterator i = effects.begin();  i != effects.end();  i++)
    updateEffect(**i, elapsed);
}

const Show::EffectList& Show::getEffects(void) const
{
  return effects;
}

Show* Show::createInstance(const String& name)
{
  Ptr<Show> show = new Show(name);
  if (!show->init())
    return NULL;

  return show.detachObject();
}

Show::Show(const String& name):
  Resource<Show>(name),
  elapsed(0.0)
{
}

bool Show::init(void)
{
  // Create built-in effect types
  {
    String name;

    name = "Null effect";

    if (!EffectType::findInstance(name))
      new EffectTemplate<NullEffect>(name);

    name = "Clear screen";

    if (!EffectType::findInstance(name))
      new EffectTemplate<ClearEffect>(name);
  }

  return true;
}

void Show::updateEffect(Effect& effect, Time newTime)
{
  Time currentTime = effect.start + effect.elapsed;
  Time deltaTime = newTime - currentTime;

  if ((currentTime == effect.start) || (newTime < currentTime))
  {
    effect.restart();
    effect.active = false;
    effect.elapsed = 0.0;
    currentTime = 0.0;
  }

  if (effect.active)
  {
    if (newTime > effect.start + effect.duration)
    {
      effect.active = false;
      effect.elapsed = effect.duration;
    }
  }
  else
  {
    if ((currentTime == 0.0 && effect.start == 0.0) ||
	(currentTime < effect.start &&
	 newTime >= effect.start &&
	 newTime <= effect.start + effect.duration))
    {
      effect.active = true;
    }
  }

  if (effect.active)
  {
    effect.elapsed = newTime - effect.start;

    effect.update(deltaTime);

    const Effect::List& children = effect.getChildren();

    for (Effect::List::const_iterator i = children.begin();  i != children.end();  i++)
      updateEffect(**i, newTime - effect.start);
  }
}

///////////////////////////////////////////////////////////////////////

ShowCodec::ShowCodec(void):
  ResourceCodec<Show>("Demo show XML codec")
{
  addSuffix("show");
}

Show* ShowCodec::read(const Path& path, const String& name)
{
  return ResourceCodec<Show>::read(path, name);
}

Show* ShowCodec::read(Stream& stream, const String& name)
{
  while (!effectStack.empty())
    effectStack.pop();

  if (!Reader::read(stream))
    return NULL;

  return show.detachObject();
}

bool ShowCodec::write(const Path& path, const Show& show)
{
  return ResourceCodec<Show>::write(path, show);
}

bool ShowCodec::write(Stream& stream, const Show& show)
{
  try
  {
    setStream(&stream);

    beginElement("show");
    addAttribute("title", show.getTitle());

    // TODO: The code.

    endElement();

    setStream(NULL);
  }
  catch (Exception& exception)
  {
    Log::writeError("Failed to write demo show %s: %s",
                    show.getName().c_str(),
		    exception.what());
    return false;
  }

  return true;
}

bool ShowCodec::onBeginElement(const String& name)
{
  if (name == "show")
  {
    show = Show::createInstance();
    if (!show)
      return false;

    show->setTitle(readString("title"));
    return true;
  }

  if (show)
  {
    if (name == "effect")
    {
      String typeName = readString("type");

      EffectType* type = EffectType::findInstance(typeName);
      if (!type)
      {
	Log::writeError("Effect type %s does not exist", typeName.c_str());
	return false;
      }

      Effect* effect = type->createEffect(readString("name"));
      if (!effect)
	return false;
      
      effect->setStartTime(readFloat("start"));
      effect->setDuration(readFloat("duration"));

      if (effectStack.empty())
	show->addEffect(*effect);
      else
	effectStack.top()->addChild(*effect);

      effectStack.push(effect);
      return true;
    }

    if (!effectStack.empty())
    {
      Effect* effect = effectStack.top();

      if (name == "parameter")
      {
	String parameterName = readString("name");

	currentParameter = effect->findParameter(parameterName);
	if (!currentParameter)
	{
	  Log::writeError("Demo effect parameter %s does not exist",
	                  parameterName.c_str());
	  return false;
	}

        return true;
      }

      if (currentParameter)
      {
	if (name == "key")
	{
	  currentParameter->createKey(readFloat("moment"), readString("value"));
	  return true;
	}
      }
    }
  }
  
  return true;
}

bool ShowCodec::onEndElement(const String& name)
{
  if (show)
  {
    if (name == "effect")
      effectStack.pop();

    if (name == "parameter")
      currentParameter = NULL;
  }
  
  return true;
}

///////////////////////////////////////////////////////////////////////

  } /*namespace demo*/
} /*namespace wendy*/

///////////////////////////////////////////////////////////////////////