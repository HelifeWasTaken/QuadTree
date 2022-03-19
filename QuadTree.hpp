/**
 * QuadTree.cpp
 * Copyright (c) 2022 Mattis Dalleau <mattis.dalleau@epitech.eu>
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

#pragma once

//
// The template parameter should have a getX getY getWidth and getHeight
// And be constructable such as with a specific constructor
// T(anyType x, anyType y, anyType, w, anyType h);
//
// The anyType should be able to be get from a .getHeight + .getWidth
// + .getX + .getY method
//
// with anyType as U
// The U should implement
// - U + U
// - U - U
// - U / 2 (Where as 2 is defined by builtin or your method)
// - U < U
// - U > U
//
// The T type should be safely copyable and as Light as possible
//
//
//

#include <memory>
#include <vector>
#include <array>

template <typename T>
class QuadTree {
 public:
  using QuadTreeRectangleList = std::vector<std::unique_ptr<T>>;
  using QuadTreeList = std::array<std::unique_ptr<QuadTree<T>>, 4>;

 private:
  QuadTreeList _trees;
  QuadTreeRectangleList _rects;
  T _bounds;
  unsigned int _level;
  unsigned int _maxRect;
  unsigned int _maxLevel;

  int findRectanglePosition(const T &nRect) const {
    const auto xMid = _bounds.getX() + _bounds.getWidth() / 2;
    const auto yMid = _bounds.getY() + _bounds.getHeight() / 2;
    const bool isTop =
        nRect.getY() < yMid && nRect.getY() + nRect.getHeight() < yMid;
    const bool isBot = nRect.getY() > yMid;

    if (nRect.getX() < xMid && nRect.getX() + nRect.getWidth() < xMid)
      return isTop ? 1 : isBot ? 2 : -1;
    else if (nRect.x > xMid)
      return isTop ? 0 : isBot ? 3 : -1;
    return -1;
  }

  void split() {
    const auto divHeight = _bounds.getHeight() / 2;
    const auto divWidth = _bounds.getWidth() / 2;
    const auto x = _bounds.getX();
    const auto y = _bounds.getY();

    const std::array<T, 4> rects = {
        T(x + divWidth, y, divWidth, divHeight), T(x, y, divWidth, divHeight),
        T(x, y + divHeight, divWidth, divHeight),
        T(x + divWidth, y + divHeight, divWidth, divHeight)};

    for (unsigned int i = 0; i < rects.size(); i++)
      _trees.at(i) = std::make_unique<QuadTree<T>>(rects.at(i), _maxRect,
                                                   _maxLevel, _level + 1);
  }

  void _getPossibleCollisions(QuadTreeRectangleList &rectangles,
                              const T &rect) const {
    const int index = findRectanglePosition(rect);
    if (index != -1 && _trees.at(0).get() != nullptr)
      _trees.at(index)->_getPossibleCollisions(rectangles, rect);
    for (const auto &it : _rects)
      rectangles.push_back(std::make_unique<T>(**it));
  }

  QuadTree(const T &bounds, unsigned int maxRect, unsigned int maxLevel,
           unsigned int level)
      : _bounds(bounds),
        _level(level),
        _maxRect(maxRect),
        _maxLevel(maxLevel) {}

 public:
  QuadTree(const T &bounds, unsigned int maxRect = 5,
           unsigned int maxLevel = 20)
      : _bounds(bounds), _level(0), _maxRect(maxRect), _maxLevel(maxLevel) {}

  void clear() {
    _rects.clear();
    for (auto &it : _trees) {
      if (it.get() != nullptr) {
        it->clear();
        it->reset(nullptr);
      }
    }
  }

  void insert(const T &nRect) {
    if (_trees.at(0).get() != nullptr) {
      const int index = findRectanglePosition(nRect);
      if (index == -1) {
        _trees.at(index)->insert(nRect);
        return;
      }
    }
    _rects.push_back(std::make_unique<T>(nRect));
    if (_rects.size() >= _maxRect && _level < _maxLevel) {
      if (_trees.at(0).get() == nullptr) split();
    }
    for (unsigned int i = 0; i < _rects.size();) {
      const int index = findRectanglePosition(nRect);
      if (index == -1) {
        i++;
      } else {
        _trees.at(index)->insert(*_rects.at(i));
        _rects.erase(_rects.begin() + i);
      }
    }
  }

  void getPossibleCollisions(QuadTreeRectangleList &rectangles,
                             const T &rect, bool clear = true) const {
    if (clear) rectangles.clear();
    _getPossibleCollisions(rectangles, rect);
  }
};
