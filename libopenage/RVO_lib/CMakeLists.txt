#
# src/CMakeLists.txt
# RVO2 Library
#
# Copyright 2008 University of North Carolina at Chapel Hill
#
# Licensed under the Apache License, Version 2.0 (the "License");
# you may not use this file except in compliance with the License.
# You may obtain a copy of the License at
#
#     http://www.apache.org/licenses/LICENSE-2.0
#
# Unless required by applicable law or agreed to in writing, software
# distributed under the License is distributed on an "AS IS" BASIS,
# WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
# See the License for the specific language governing permissions and
# limitations under the License.
#
# Please send all bug reports to <geom@cs.unc.edu>.
#
# The authors may be contacted via:
#
# Jur van den Berg, Stephen J. Guy, Jamie Snape, Ming C. Lin, Dinesh Manocha
# Dept. of Computer Science
# 201 S. Columbia St.
# Frederick P. Brooks, Jr. Computer Science Bldg.
# Chapel Hill, N.C. 27599-3175
# United States of America
#
# <http://gamma.cs.unc.edu/RVO2/>
#

set(RVO_HEADERS
	RVO.h
	RVOSimulator.h
	Vector2.h)

set(RVO_SOURCES
	Agent.cpp
	Agent.h
	Definitions.h
	KdTree.cpp
	KdTree.h
	Obstacle.cpp
	Obstacle.h
	RVOSimulator.cpp)

add_library(RVO SHARED ${RVO_HEADERS} ${RVO_SOURCES})

if(WIN32)
    set_target_properties(RVO PROPERTIES COMPILE_DEFINITIONS NOMINMAX)
endif()

install(FILES ${RVO_HEADERS} DESTINATION include)
install(TARGETS RVO DESTINATION lib)
