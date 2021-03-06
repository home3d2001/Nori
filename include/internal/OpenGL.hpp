///////////////////////////////////////////////////////////////////////
// Nori - a simple game engine
// Copyright (c) 2012 Camilla Berglund <elmindreda@elmindreda.org>
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

#pragma once

namespace nori
{

NORI_CHECKFORMAT(1, bool checkGL(const char* format, ...));

GLenum convertToGL(IndexType type);
GLenum convertToGL(PixelFormat::Type type);
GLenum convertToGL(const PixelFormat& format, bool sRGB);
GLenum convertToGL(PixelFormat::Type type);
GLenum convertToGL(PixelFormat::Semantic semantic);
GLenum convertToGL(TextureType type);

GLboolean getBoolean(GLenum token);
GLint getInteger(GLenum token);
GLfloat getFloat(GLenum token);

class Preprocessor
{
public:
  Preprocessor(ResourceCache& cache);
  void parse(const char* name);
  void parse(const char* name, const char* text);
  bool hasVersion() const;
  const std::string& output() const { return m_output; }
  const std::string& version() const { return m_version; }
  const std::string& names() const { return m_list; }
  const std::vector<Path>& paths() const { return m_paths; }
private:
  void addLine();
  void advance(size_t offset);
  void discard();
  void appendToOutput();
  void appendToOutput(const char* text);
  char c(ptrdiff_t offset) const;
  void passWhitespace();
  void parseWhitespace();
  void parseNewLine();
  void parseSingleLineComment();
  void parseMultiLineComment();
  std::string passNumber();
  std::string passIdentifier();
  std::string passShaderName();
  void parseCommand();
  bool hasMore() const;
  bool isNewLine() const;
  bool isMultiLineComment() const;
  bool isSingleLineComment() const;
  bool isWhitespace() const;
  bool isCommand() const;
  bool isAlpha() const;
  bool isNumeric() const;
  bool isAlphaNumeric() const;
  bool isFirstOnLine() const;
  void setFirstOnLine(bool newState);
  class File;
  ResourceCache& m_cache;
  std::vector<File> m_files;
  std::vector<std::string> m_names;
  std::vector<Path> m_paths;
  std::string m_output;
  std::string m_version;
  std::string m_list;
};

class Preprocessor::File
{
public:
  File(const char* name, const char* text);
  const char* name;
  const char* text;
  const char* base;
  const char* pos;
  unsigned int line;
  bool first;
};

} /*namespace nori*/

