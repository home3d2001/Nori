///////////////////////////////////////////////////////////////////////
// Wendy OpenGL library
// Copyright (c) 2006 Camilla Berglund <elmindreda@elmindreda.org>
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

#include <wendy/Config.h>

#include <wendy/Core.h>
#include <wendy/Block.h>

#include <wendy/GLBuffer.h>
#include <wendy/GLTexture.h>
#include <wendy/GLProgram.h>
#include <wendy/GLContext.h>
#include <wendy/GLState.h>

#define GLEW_STATIC
#include <GL/glew.h>

#include <internal/GLHelper.h>

#include <algorithm>

///////////////////////////////////////////////////////////////////////

namespace wendy
{
  namespace GL
  {

///////////////////////////////////////////////////////////////////////

namespace
{

GLenum convertToGL(CullMode mode)
{
  switch (mode)
  {
    case CULL_NONE:
      break;
    case CULL_FRONT:
      return GL_FRONT;
    case CULL_BACK:
      return GL_BACK;
    case CULL_BOTH:
      return GL_FRONT_AND_BACK;
  }

  panic("Invalid cull mode %u", mode);
}

CullMode invertCullMode(CullMode mode)
{
  switch (mode)
  {
    case CULL_NONE:
      return CULL_BOTH;
    case CULL_FRONT:
      return CULL_BACK;
    case CULL_BACK:
      return CULL_FRONT;
    case CULL_BOTH:
      return CULL_NONE;
  }

  panic("Invalid cull mode %u", mode);
}

GLenum convertToGL(BlendFactor factor)
{
  switch (factor)
  {
    case BLEND_ZERO:
      return GL_ZERO;
    case BLEND_ONE:
      return GL_ONE;
    case BLEND_SRC_COLOR:
      return GL_SRC_COLOR;
    case BLEND_DST_COLOR:
      return GL_DST_COLOR;
    case BLEND_SRC_ALPHA:
      return GL_SRC_ALPHA;
    case BLEND_DST_ALPHA:
      return GL_DST_ALPHA;
    case BLEND_ONE_MINUS_SRC_COLOR:
      return GL_ONE_MINUS_SRC_COLOR;
    case BLEND_ONE_MINUS_DST_COLOR:
      return GL_ONE_MINUS_DST_COLOR;
    case BLEND_ONE_MINUS_SRC_ALPHA:
      return GL_ONE_MINUS_SRC_ALPHA;
    case BLEND_ONE_MINUS_DST_ALPHA:
      return GL_ONE_MINUS_DST_ALPHA;
  }

  panic("Invalid blend factor %u", factor);
}

GLenum convertToGL(Function function)
{
  switch (function)
  {
    case ALLOW_NEVER:
      return GL_NEVER;
    case ALLOW_ALWAYS:
      return GL_ALWAYS;
    case ALLOW_EQUAL:
      return GL_EQUAL;
    case ALLOW_NOT_EQUAL:
      return GL_NOTEQUAL;
    case ALLOW_LESSER:
      return GL_LESS;
    case ALLOW_LESSER_EQUAL:
      return GL_LEQUAL;
    case ALLOW_GREATER:
      return GL_GREATER;
    case ALLOW_GREATER_EQUAL:
      return GL_GEQUAL;
  }

  panic("Invalid comparison function %u", function);
}

GLenum convertToGL(Operation operation)
{
  switch (operation)
  {
    case OP_KEEP:
      return GL_KEEP;
    case OP_ZERO:
      return GL_ZERO;
    case OP_REPLACE:
      return GL_REPLACE;
    case OP_INCREASE:
      return GL_INCR;
    case OP_DECREASE:
      return GL_DECR;
    case OP_INVERT:
      return GL_INVERT;
    case OP_INCREASE_WRAP:
      return GL_INCR_WRAP;
    case OP_DECREASE_WRAP:
      return GL_DECR_WRAP;
  }

  panic("Invalid stencil operation %u", operation);
}

bool samplerTypeMatchesTextureType(SamplerType samplerType, TextureType textureType)
{
  return (int) samplerType == (int) textureType;
}

} /*namespace*/

///////////////////////////////////////////////////////////////////////

UniformStateIndex::UniformStateIndex():
  index(0xffff),
  offset(0xffff)
{
}

UniformStateIndex::UniformStateIndex(uint16 initIndex, uint16 initOffset):
  index(initIndex),
  offset(initOffset)
{
}

///////////////////////////////////////////////////////////////////////

SamplerStateIndex::SamplerStateIndex():
  index(0xffff),
  unit(0xffff)
{
}

SamplerStateIndex::SamplerStateIndex(uint16 initIndex, uint16 initUnit):
  index(initIndex),
  unit(initUnit)
{
}

///////////////////////////////////////////////////////////////////////

ProgramState::ProgramState():
  ID(allocateID())
{
}

ProgramState::ProgramState(const ProgramState& source):
  ID(allocateID()),
  program(source.program),
  floats(source.floats),
  textures(source.textures)
{
}

ProgramState::~ProgramState()
{
  releaseID(ID);
}

void ProgramState::apply() const
{
  if (!program)
  {
    logError("Applying program state with no program set");
    return;
  }

  Context& context = program->getContext();
  context.setCurrentProgram(program);

  SharedProgramState* state = context.getCurrentSharedProgramState();

  unsigned int textureIndex = 0, textureUnit = 0;

  for (unsigned int i = 0;  i < program->getSamplerCount();  i++)
  {
    context.setActiveTextureUnit(textureUnit);

    Sampler& sampler = program->getSampler(i);
    if (sampler.isShared())
    {
      if (state)
        state->updateTo(sampler);
      else
        logError("Program \'%s\' uses shared sampler \'%s\' without a current shared program state",
                 program->getName().c_str(),
                 sampler.getName().c_str());
    }
    else
    {
      context.setCurrentTexture(textures[textureIndex]);
      textureIndex++;
    }

    sampler.bind(textureUnit);
    textureUnit++;
  }

  size_t offset = 0;

  for (unsigned int i = 0;  i < program->getUniformCount();  i++)
  {
    Uniform& uniform = program->getUniform(i);
    if (uniform.isShared())
    {
      if (state)
        state->updateTo(uniform);
      else
        logError("Program \'%s\' uses shared uniform \'%s\' without a current shared program state",
                 program->getName().c_str(),
                 uniform.getName().c_str());
    }
    else
    {
      uniform.copyFrom(&floats[0] + offset);
      offset += uniform.getElementCount();
    }
  }
}

bool ProgramState::hasUniformState(const char* name) const
{
  if (!program)
    return false;

  Uniform* uniform = program->findUniform(name);
  if (!uniform)
    return false;

  return !uniform->isShared();
}

bool ProgramState::hasSamplerState(const char* name) const
{
  if (!program)
    return false;

  Sampler* sampler = program->findSampler(name);
  if (!sampler)
    return false;

  return !sampler->isShared();
}

Texture* ProgramState::getSamplerState(const char* name) const
{
  if (!program)
  {
    logError("Cannot retrieve sampler state on program state with no program");
    return NULL;
  }

  unsigned int textureIndex = 0;

  for (unsigned int i = 0;  i < program->getSamplerCount();  i++)
  {
    const Sampler& sampler = program->getSampler(i);
    if (sampler.isShared())
      continue;

    if (sampler.getName() == name)
      return textures[textureIndex];

    textureIndex++;
  }

  logError("Program \'%s\' has no sampler named \'%s\'",
           program->getName().c_str(),
           name);
  return NULL;
}

Texture* ProgramState::getSamplerState(SamplerStateIndex index) const
{
  if (!program)
  {
    logError("Cannot retrieve sampler state on program state with no program");
    return NULL;
  }

  return textures[index.unit];
}

void ProgramState::setSamplerState(const char* name, Texture* newTexture)
{
  if (!program)
  {
    logError("Cannot set sampler state on program state with no program");
    return;
  }

  unsigned int textureIndex = 0;

  for (unsigned int i = 0;  i < program->getSamplerCount();  i++)
  {
    Sampler& sampler = program->getSampler(i);
    if (sampler.isShared())
      continue;

    if (sampler.getName() == name)
    {
      if (newTexture)
      {
        if (samplerTypeMatchesTextureType(sampler.getType(), newTexture->getType()))
          textures[textureIndex] = newTexture;
        else
          logError("Type mismatch between sampler \'%s\' and texture \'%s\'",
                   sampler.getName().c_str(),
                   newTexture->getName().c_str());
      }
      else
        textures[textureIndex] = NULL;

      return;
    }

    textureIndex++;
  }
}

void ProgramState::setSamplerState(SamplerStateIndex index, Texture* newTexture)
{
  if (!program)
  {
    logError("Cannot set sampler state on program state with no program");
    return;
  }

  Sampler& sampler = program->getSampler(index.index);

  if (newTexture)
  {
    if (samplerTypeMatchesTextureType(sampler.getType(), newTexture->getType()))
      textures[index.unit] = newTexture;
    else
      logError("Type mismatch between sampler \'%s\' and texture \'%s\'",
                sampler.getName().c_str(),
                newTexture->getName().c_str());
  }
  else
    textures[index.unit] = NULL;
}

UniformStateIndex ProgramState::getUniformStateIndex(const char* name) const
{
  if (!program)
  {
    logError("Cannot retrieve uniform state indices with no program");
    return UniformStateIndex();
  }

  unsigned int offset = 0;

  for (unsigned int i = 0;  i < program->getUniformCount();  i++)
  {
    Uniform& uniform = program->getUniform(i);
    if (uniform.isShared())
      continue;

    if (uniform.getName() == name)
      return UniformStateIndex(i, offset);

    offset += uniform.getElementCount();
  }

  return UniformStateIndex();
}

SamplerStateIndex ProgramState::getSamplerStateIndex(const char* name) const
{
  if (!program)
  {
    logError("Cannot retrieve sampler state indices with no program");
    return SamplerStateIndex();
  }

  unsigned int textureIndex = 0;

  for (unsigned int i = 0;  i < program->getSamplerCount();  i++)
  {
    Sampler& sampler = program->getSampler(i);
    if (sampler.isShared())
      continue;

    if (sampler.getName() == name)
      return SamplerStateIndex(i, textureIndex);

    textureIndex++;
  }

  return SamplerStateIndex();
}

Program* ProgramState::getProgram() const
{
  return program;
}

void ProgramState::setProgram(Program* newProgram)
{
  floats.clear();
  textures.clear();

  program = newProgram;
  if (!program)
    return;

  unsigned int floatCount = 0;
  unsigned int textureCount = 0;

  for (unsigned int i = 0;  i < program->getUniformCount();  i++)
  {
    Uniform& uniform = program->getUniform(i);
    if (!uniform.isShared())
      floatCount += uniform.getElementCount();
  }

  for (unsigned int i = 0;  i < program->getSamplerCount();  i++)
  {
    Sampler& sampler = program->getSampler(i);
    if (!sampler.isShared())
      textureCount++;
  }

  floats.insert(floats.end(), floatCount, 0.f);
  textures.resize(textureCount);
}

StateID ProgramState::getID() const
{
  return ID;
}

StateID ProgramState::allocateID()
{
  if (usedIDs.empty())
    return nextID++;

  const StateID ID = usedIDs.back();
  usedIDs.pop_back();
  return ID;
}

void ProgramState::releaseID(StateID ID)
{
  usedIDs.push_front(ID);
}

void* ProgramState::getData(const char* name, UniformType type)
{
  if (!program)
  {
    logError("Cannot set uniform state on program state with no program");
    return NULL;
  }

  unsigned int offset = 0;

  for (unsigned int i = 0;  i < program->getUniformCount();  i++)
  {
    Uniform& uniform = program->getUniform(i);
    if (uniform.isShared())
      continue;

    if (uniform.getName() == name)
    {
      if (uniform.getType() == type)
        return &floats[0] + offset;

      logError("Uniform \'%s\' of program \'%s\' is not of type \'%s\'",
               uniform.getName().c_str(),
               program->getName().c_str(),
               Uniform::getTypeName(type));
      return NULL;
    }

    offset += uniform.getElementCount();
  }

  logError("Program \'%s\' has no uniform named \'%s\'",
           program->getName().c_str(),
           name);
  return NULL;
}

const void* ProgramState::getData(const char* name, UniformType type) const
{
  if (!program)
  {
    logError("Cannot set uniform state on program state with no program");
    return NULL;
  }

  unsigned int offset = 0;

  for (unsigned int i = 0;  i < program->getUniformCount();  i++)
  {
    Uniform& uniform = program->getUniform(i);
    if (uniform.isShared())
      continue;

    if (uniform.getName() == name)
    {
      if (uniform.getType() == type)
        return &floats[0] + offset;

      logError("Uniform \'%s\' of program \'%s\' is not of type \'%s\'",
               uniform.getName().c_str(),
               program->getName().c_str(),
               Uniform::getTypeName(type));
      return NULL;
    }

    offset += uniform.getElementCount();
  }

  logError("Program \'%s\' has no uniform named \'%s\'",
           program->getName().c_str(),
           name);
  return NULL;
}

void* ProgramState::getData(UniformStateIndex index, UniformType type)
{
  if (!program)
  {
    logError("Cannot set uniform state on program state with no program");
    return NULL;
  }

  Uniform& uniform = program->getUniform(index.index);

  if (uniform.getType() != type)
  {
    logError("Uniform %u of program \'%s\' is not of type \'%s\'",
             index.index,
             program->getName().c_str(),
             Uniform::getTypeName(type));
    return NULL;
  }

  return &floats[0] + index.offset;
}

const void* ProgramState::getData(UniformStateIndex index, UniformType type) const
{
  if (!program)
  {
    logError("Cannot set uniform state on program state with no program");
    return NULL;
  }

  Uniform& uniform = program->getUniform(index.index);

  if (uniform.getType() != type)
  {
    logError("Uniform %u of program \'%s\' is not of type \'%s\'",
             index.index,
             program->getName().c_str(),
             Uniform::getTypeName(type));
    return NULL;
  }

  return &floats[0] + index.offset;
}

template <>
UniformType ProgramState::getUniformType<float>()
{
  return UNIFORM_FLOAT;
}

template <>
UniformType ProgramState::getUniformType<vec2>()
{
  return UNIFORM_VEC2;
}

template <>
UniformType ProgramState::getUniformType<vec3>()
{
  return UNIFORM_VEC3;
}

template <>
UniformType ProgramState::getUniformType<vec4>()
{
  return UNIFORM_VEC4;
}

template <>
UniformType ProgramState::getUniformType<mat2>()
{
  return UNIFORM_MAT2;
}

template <>
UniformType ProgramState::getUniformType<mat3>()
{
  return UNIFORM_MAT3;
}

template <>
UniformType ProgramState::getUniformType<mat4>()
{
  return UNIFORM_MAT4;
}

ProgramState::IDQueue ProgramState::usedIDs;

StateID ProgramState::nextID = 0;

///////////////////////////////////////////////////////////////////////

void RenderState::apply() const
{
  if (Context* context = Context::getSingleton())
  {
    if (Stats* stats = context->getStats())
      stats->addStateChange();
  }

  if (dirty)
  {
    force();
    return;
  }

  CullMode cullMode = data.cullMode;
  if (cullingInverted)
    cullMode = invertCullMode(cullMode);

  if (cullMode != cache.cullMode)
  {
    if ((cullMode == CULL_NONE) != (cache.cullMode == CULL_NONE))
      setBooleanState(GL_CULL_FACE, cullMode != CULL_NONE);

    if (cullMode != CULL_NONE)
      glCullFace(convertToGL(cullMode));

    cache.cullMode = cullMode;
  }

  if (data.srcFactor != cache.srcFactor || data.dstFactor != cache.dstFactor)
  {
    setBooleanState(GL_BLEND, data.srcFactor != BLEND_ONE || data.dstFactor != BLEND_ZERO);

    if (data.srcFactor != BLEND_ONE || data.dstFactor != BLEND_ZERO)
      glBlendFunc(convertToGL(data.srcFactor), convertToGL(data.dstFactor));

    cache.srcFactor = data.srcFactor;
    cache.dstFactor = data.dstFactor;
  }

  if (data.depthTesting || data.depthWriting)
  {
    // Set depth buffer writing.
    if (data.depthWriting != cache.depthWriting)
      glDepthMask(data.depthWriting ? GL_TRUE : GL_FALSE);

    if (data.depthTesting)
    {
      // Set depth buffer function.
      if (data.depthFunction != cache.depthFunction)
      {
        glDepthFunc(convertToGL(data.depthFunction));
        cache.depthFunction = data.depthFunction;
      }
    }
    else if (data.depthWriting)
    {
      // NOTE: Special case; depth buffer filling.
      //       Set specific depth buffer function.
      const Function depthFunction = ALLOW_ALWAYS;

      if (cache.depthFunction != depthFunction)
      {
        glDepthFunc(convertToGL(depthFunction));
        cache.depthFunction = depthFunction;
      }
    }

    if (!(cache.depthTesting || cache.depthWriting))
      glEnable(GL_DEPTH_TEST);
  }
  else
  {
    if (cache.depthTesting || cache.depthWriting)
      glDisable(GL_DEPTH_TEST);
  }

  cache.depthTesting = data.depthTesting;
  cache.depthWriting = data.depthWriting;

  if (data.colorWriting != cache.colorWriting)
  {
    const GLboolean state = data.colorWriting ? GL_TRUE : GL_FALSE;
    glColorMask(state, state, state, state);
    cache.colorWriting = data.colorWriting;
  }

  if (data.stencilTesting != cache.stencilTesting)
  {
    setBooleanState(GL_STENCIL_TEST, data.stencilTesting);
    cache.stencilTesting = data.stencilTesting;
  }

  if (data.stencilTesting)
  {
    if (data.stencilFunction != cache.stencilFunction ||
        data.stencilRef != cache.stencilRef ||
        data.stencilMask != cache.stencilMask)
    {
      glStencilFunc(convertToGL(data.stencilFunction),
                    data.stencilRef, data.stencilMask);

      cache.stencilFunction = data.stencilFunction;
      cache.stencilRef = data.stencilRef;
      cache.stencilMask = data.stencilMask;
    }

    if (data.stencilFailOp != cache.stencilFailOp ||
        data.depthFailOp != cache.depthFailOp ||
        data.depthPassOp != cache.depthPassOp)
    {
      glStencilOp(convertToGL(data.stencilFailOp),
                  convertToGL(data.depthFailOp),
                  convertToGL(data.depthPassOp));

      cache.stencilFailOp = data.stencilFailOp;
      cache.depthFailOp = data.depthFailOp;
      cache.depthPassOp = data.depthPassOp;
    }
  }

  if (data.wireframe != cache.wireframe)
  {
    const GLenum state = data.wireframe ? GL_LINE : GL_FILL;
    glPolygonMode(GL_FRONT_AND_BACK, state);
    cache.wireframe = data.wireframe;
  }

  if (data.lineSmoothing != cache.lineSmoothing)
  {
    setBooleanState(GL_LINE_SMOOTH, data.lineSmoothing);
    cache.lineSmoothing = data.lineSmoothing;
  }

  if (data.multisampling != cache.multisampling)
  {
    setBooleanState(GL_MULTISAMPLE, data.multisampling);
    cache.multisampling = data.multisampling;
  }

  if (data.lineWidth != cache.lineWidth)
  {
    glLineWidth(data.lineWidth);
    cache.lineWidth = data.lineWidth;
  }

#if WENDY_DEBUG
  checkGL("Error when applying render state");
#endif

  ProgramState::apply();
}

bool RenderState::isCulling() const
{
  return data.cullMode != CULL_NONE;
}

bool RenderState::isBlending() const
{
  return data.srcFactor != BLEND_ONE || data.dstFactor != BLEND_ZERO;
}

bool RenderState::isDepthTesting() const
{
  return data.depthTesting;
}

bool RenderState::isDepthWriting() const
{
  return data.depthWriting;
}

bool RenderState::isColorWriting() const
{
  return data.colorWriting;
}

bool RenderState::isStencilTesting() const
{
  return data.stencilTesting;
}

bool RenderState::isWireframe() const
{
  return data.wireframe;
}

bool RenderState::isLineSmoothing() const
{
  return data.lineSmoothing;
}

bool RenderState::isMultisampling() const
{
  return data.multisampling;
}

float RenderState::getLineWidth() const
{
  return data.lineWidth;
}

CullMode RenderState::getCullMode() const
{
  return data.cullMode;
}

BlendFactor RenderState::getSrcFactor() const
{
  return data.srcFactor;
}

BlendFactor RenderState::getDstFactor() const
{
  return data.dstFactor;
}

Function RenderState::getDepthFunction() const
{
  return data.depthFunction;
}

Function RenderState::getStencilFunction() const
{
  return data.stencilFunction;
}

Operation RenderState::getStencilFailOperation() const
{
  return data.stencilFailOp;
}

Operation RenderState::getDepthFailOperation() const
{
  return data.depthFailOp;
}

Operation RenderState::getDepthPassOperation() const
{
  return data.depthPassOp;
}

unsigned int RenderState::getStencilReference() const
{
  return data.stencilRef;
}

unsigned int RenderState::getStencilWriteMask() const
{
  return data.stencilMask;
}

void RenderState::setDepthTesting(bool enable)
{
  data.depthTesting = enable;
}

void RenderState::setDepthWriting(bool enable)
{
  data.depthWriting = enable;
}

void RenderState::setStencilTesting(bool enable)
{
  data.stencilTesting = enable;
}

void RenderState::setCullMode(CullMode mode)
{
  data.cullMode = mode;
}

void RenderState::setBlendFactors(BlendFactor src, BlendFactor dst)
{
  data.srcFactor = src;
  data.dstFactor = dst;
}

void RenderState::setDepthFunction(Function function)
{
  data.depthFunction = function;
}

void RenderState::setStencilFunction(Function newFunction)
{
  data.stencilFunction = newFunction;
}

void RenderState::setStencilReference(unsigned int newReference)
{
  data.stencilRef = newReference;
}

void RenderState::setStencilWriteMask(unsigned int newMask)
{
  data.stencilMask = newMask;
}

void RenderState::setStencilFailOperation(Operation newOperation)
{
  data.stencilFailOp = newOperation;
}

void RenderState::setDepthFailOperation(Operation newOperation)
{
  data.depthFailOp = newOperation;
}

void RenderState::setDepthPassOperation(Operation newOperation)
{
  data.depthPassOp = newOperation;
}

void RenderState::setColorWriting(bool enabled)
{
  data.colorWriting = enabled;
}

void RenderState::setWireframe(bool enabled)
{
  data.wireframe = enabled;
}

void RenderState::setLineSmoothing(bool enabled)
{
  data.lineSmoothing = enabled;
}

void RenderState::setMultisampling(bool enabled)
{
  data.multisampling = enabled;
}

void RenderState::setLineWidth(float newWidth)
{
  data.lineWidth = newWidth;
}

bool RenderState::isCullingInverted()
{
  return cullingInverted;
}

void RenderState::setCullingInversion(bool newState)
{
  cullingInverted = newState;
}

void RenderState::force() const
{
  cache = data;

  CullMode cullMode = data.cullMode;
  if (cullingInverted)
    cullMode = invertCullMode(cullMode);

  setBooleanState(GL_CULL_FACE, cullMode != CULL_NONE);
  if (cullMode != CULL_NONE)
    glCullFace(convertToGL(cullMode));

  setBooleanState(GL_BLEND, data.srcFactor != BLEND_ONE || data.dstFactor != BLEND_ZERO);
  glBlendFunc(convertToGL(data.srcFactor), convertToGL(data.dstFactor));

  glDepthMask(data.depthWriting ? GL_TRUE : GL_FALSE);
  setBooleanState(GL_DEPTH_TEST, data.depthTesting || data.depthWriting);

  if (data.depthWriting && !data.depthTesting)
  {
    const Function depthFunction = ALLOW_ALWAYS;
    glDepthFunc(convertToGL(depthFunction));
    cache.depthFunction = depthFunction;
  }
  else
    glDepthFunc(convertToGL(data.depthFunction));

  const GLboolean state = data.colorWriting ? GL_TRUE : GL_FALSE;
  glColorMask(state, state, state, state);

  const GLenum polygonMode = data.wireframe ? GL_LINE : GL_FILL;
  glPolygonMode(GL_FRONT_AND_BACK, polygonMode);

  setBooleanState(GL_LINE_SMOOTH, data.lineSmoothing);
  glLineWidth(data.lineWidth);

  setBooleanState(GL_MULTISAMPLE, data.multisampling);

  setBooleanState(GL_STENCIL_TEST, data.stencilTesting);
  glStencilFunc(convertToGL(data.stencilFunction),
                data.stencilRef, data.stencilMask);
  glStencilOp(convertToGL(data.stencilFailOp),
              convertToGL(data.depthFailOp),
              convertToGL(data.depthPassOp));

#if WENDY_DEBUG
  checkGL("Error when forcing render state");
#endif

  ProgramState::apply();

  dirty = false;
}

void RenderState::setBooleanState(unsigned int state, bool value) const
{
  if (value)
    glEnable(state);
  else
    glDisable(state);
}

RenderState::Data RenderState::cache;

bool RenderState::dirty = true;

bool RenderState::cullingInverted = false;

///////////////////////////////////////////////////////////////////////

RenderState::Data::Data():
  depthTesting(true),
  depthWriting(true),
  colorWriting(true),
  stencilTesting(false),
  wireframe(false),
  lineSmoothing(false),
  multisampling(true),
  lineWidth(1.f),
  cullMode(CULL_BACK),
  srcFactor(BLEND_ONE),
  dstFactor(BLEND_ZERO),
  depthFunction(ALLOW_LESSER),
  stencilFunction(ALLOW_ALWAYS),
  stencilRef(0),
  stencilMask(~0),
  stencilFailOp(OP_KEEP),
  depthFailOp(OP_KEEP),
  depthPassOp(OP_KEEP)
{
}

///////////////////////////////////////////////////////////////////////

  } /*namespace GL*/
} /*namespace wendy*/

///////////////////////////////////////////////////////////////////////
