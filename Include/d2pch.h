/**************************************************************************************\
** File: d2pch.h
** Project: Space
** Author: David Leksen
** Date: 
**
** Header file for the precompiled header
**
\**************************************************************************************/
#pragma once
// SDL_ASSERT_LEVEL:
//		0 Disable all
//		1 enable SDL_assert_release
//		2 enable SDL_assert
//		3 enable SDL_assert_paranoid 

#ifdef D2_ASSERT_LEVEL
#define SDL_ASSERT_LEVEL D2_ASSERT_LEVEL
#else
#define SDL_ASSERT_LEVEL 2
#endif

#define d2AssertRelease SDL_assert_release
#define d2Assert SDL_assert
#define d2AssertParanoid SDL_assert_paranoid

#include <SDL.h>
#include <SDL_opengl.h>
#include <SDL_image.h>
#include <GL/GLU.h>
#include <Box2D/Box2D.h>
#include "drawtext.h"
#include "hjson.h"

#include <boost/algorithm/string.hpp>
#include <boost/property_tree/xml_parser.hpp>
#include <boost/log/core.hpp>
#include <boost/log/trivial.hpp>
#include <boost/log/utility/setup/file.hpp>
#include <boost/log/utility/setup/common_attributes.hpp>
#include <boost/lexical_cast.hpp>

#include <vector>
#include <array>
#include <list>
#include <queue>
#include <stack>
#include <map>
#include <sstream>
#include <fstream>
#include <random>
#include <string>
using namespace std::string_literals;


