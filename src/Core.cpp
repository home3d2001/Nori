///////////////////////////////////////////////////////////////////////
// Nori - a simple game engine
// Copyright (c) 2005 Camilla Berglund <elmindreda@elmindreda.org>
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

#include <nori/Config.hpp>

#include <nori/Core.hpp>

#include <algorithm>
#include <exception>
#include <sstream>
#include <iostream>

#include <cstdlib>
#include <cstring>
#include <cstdio>

namespace nori
{

namespace
{

std::vector<LogConsumer*> consumers;

} /*namespace*/

std::string stringCast(const vec2& v)
{
  std::ostringstream stream;

  stream << v.x << ' ' << v.y;
  return stream.str();
}

std::string stringCast(const vec3& v)
{
  std::ostringstream stream;

  stream << v.x << ' ' << v.y << ' ' << v.z;
  return stream.str();
}

std::string stringCast(const vec4& v)
{
  std::ostringstream stream;

  stream << v.x << ' ' << v.y << ' ' << v.z << ' ' << v.w;
  return stream.str();
}

std::string stringCast(const mat2& v)
{
  std::ostringstream stream;

  stream << v[0][0] << ' ' << v[0][1] << ' '
         << v[1][0] << ' ' << v[1][1];

  return stream.str();
}

std::string stringCast(const mat3& v)
{
  std::ostringstream stream;

  stream << v[0][0] << ' ' << v[0][1] << ' ' << v[0][2] << ' '
         << v[1][0] << ' ' << v[1][1] << ' ' << v[1][2] << ' '
         << v[2][0] << ' ' << v[2][1] << ' ' << v[2][2];

  return stream.str();
}

std::string stringCast(const mat4& v)
{
  std::ostringstream stream;

  stream << v[0][0] << ' ' << v[0][1] << ' ' << v[0][2] << ' ' << v[0][3] << ' '
         << v[1][0] << ' ' << v[1][1] << ' ' << v[1][2] << ' ' << v[1][3] << ' '
         << v[2][0] << ' ' << v[2][1] << ' ' << v[2][2] << ' ' << v[2][3] << ' '
         << v[3][0] << ' ' << v[3][1] << ' ' << v[3][2] << ' ' << v[3][3];

  return stream.str();
}

std::string stringCast(const quat& v)
{
  std::ostringstream stream;

  stream << v.x << ' ' << v.y << ' ' << v.z << ' ' << v.w;
  return stream.str();
}

vec2 vec2Cast(const std::string& string)
{
  std::istringstream stream(string, std::ios::in);

  vec2 result;
  stream >> result.x >> result.y;
  return result;
}

vec3 vec3Cast(const std::string& string)
{
  std::istringstream stream(string, std::ios::in);

  vec3 result;
  stream >> result.x >> result.y >> result.z;
  return result;
}

vec4 vec4Cast(const std::string& string)
{
  std::istringstream stream(string, std::ios::in);

  vec4 result;
  stream >> result.x >> result.y >> result.z >> result.w;
  return result;
}

mat2 mat2Cast(const std::string& string)
{
  std::istringstream stream(string, std::ios::in);

  mat2 result;
  stream >> result[0][0] >> result[0][1];
  stream >> result[1][0] >> result[1][1];
  return result;
}

mat3 mat3Cast(const std::string& string)
{
  std::istringstream stream(string, std::ios::in);

  mat3 result;
  stream >> result[0][0] >> result[0][1] >> result[0][2];
  stream >> result[1][0] >> result[1][1] >> result[1][2];
  stream >> result[2][0] >> result[2][1] >> result[2][2];
  return result;
}

mat4 mat4Cast(const std::string& string)
{
  std::istringstream stream(string, std::ios::in);

  mat4 result;
  stream >> result[0][0] >> result[0][1] >> result[0][2] >> result[0][3];
  stream >> result[1][0] >> result[1][1] >> result[1][2] >> result[1][3];
  stream >> result[2][0] >> result[2][1] >> result[2][2] >> result[2][3];
  stream >> result[3][0] >> result[3][1] >> result[3][2] >> result[3][3];
  return result;
}

quat quatCast(const std::string& string)
{
  std::istringstream stream(string, std::ios::in);

  quat result;
  stream >> result.x >> result.y >> result.z >> result.w;
  return result;
}

std::string format(const char* format, ...)
{
  va_list vl;

  va_start(vl, format);
  std::string result = vlformat(format, vl);
  va_end(vl);

  return result;
}

std::string vlformat(const char* format, va_list vl)
{
  char buffer[65536];

  if (vsnprintf(buffer, sizeof(buffer), format, vl) < 0)
    buffer[sizeof(buffer) - 1] = '\0';

  return std::string(buffer);
}

void logError(const char* format, ...)
{
  va_list vl;

  va_start(vl, format);
  std::string message = vlformat(format, vl);
  va_end(vl);

  if (consumers.empty())
    std::cerr << "Error: " << message << std::endl;
  else
  {
    for (LogConsumer* c : consumers)
      c->onLogEntry(ERROR_LOG_ENTRY, message.c_str());
  }
}

void logWarning(const char* format, ...)
{
  va_list vl;

  va_start(vl, format);
  std::string message = vlformat(format, vl);
  va_end(vl);

  if (consumers.empty())
    std::cerr << "Warning: " << message << std::endl;
  else
  {
    for (LogConsumer* c : consumers)
      c->onLogEntry(WARNING_LOG_ENTRY, message.c_str());
  }
}

void log(const char* format, ...)
{
  va_list vl;

  va_start(vl, format);
  std::string message = vlformat(format, vl);
  va_end(vl);

  if (consumers.empty())
    std::cerr << message << std::endl;
  else
  {
    for (LogConsumer* c : consumers)
      c->onLogEntry(INFO_LOG_ENTRY, message.c_str());
  }
}

void panic(const char* format, ...)
{
  va_list vl;

  va_start(vl, format);
  std::string message = vlformat(format, vl);
  va_end(vl);

  std::cerr << message << std::endl;
  std::terminate();
}

Exception::Exception(const char* message):
  message(message)
{
}

const char* Exception::what() const throw()
{
  return message;
}

bool RefBase::unreferenced(RefObject* object)
{
  return object->count == 0;
}

void RefBase::increment(RefObject* object)
{
  object->count++;
}

void RefBase::decrement(RefObject* object)
{
  object->count--;
}

RefObject::RefObject():
  count(0)
{
}

RefObject::RefObject(const RefObject& source):
  count(0)
{
}

RefObject::~RefObject()
{
}

RefObject& RefObject::operator = (const RefObject& source)
{
  return *this;
}

LogConsumer::LogConsumer()
{
  consumers.push_back(this);
}

LogConsumer::~LogConsumer()
{
  consumers.erase(std::find(consumers.begin(), consumers.end(), this));
}

} /*namespace nori*/

