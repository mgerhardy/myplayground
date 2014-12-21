#pragma once

#include <vector>
#include <algorithm>

#ifndef MINAREA
#define MINAREA 10.0f
#endif

class Rect {
private:
	float _x;
	float _z;
	float _width;
	float _height;
public:
	Rect(float x, float z, float width, float height) :
			_x(x), _z(z), _width(width), _height(height) {
	}

	float getMaxZ() const;
	bool isEmpty() const;
	float getMinX() const;
	float getMaxX() const;
	float getMinZ() const;
	float getX() const;
	float getWidth() const;
	float getZ() const;
	float getHeight() const;
	bool operator==(const Rect& other) const;
	bool contains(const Rect& rect) const;
	bool intersectsWith(const Rect& rect) const;
	void offset(float dx, float dz);
};

static inline Rect fromMaxCoordinates(float minX, float minZ, float maxX, float maxZ) {
	return Rect(minX, minZ, maxX - minX, maxZ - minZ);
}

static inline Rect intersection(const Rect& r1, const Rect& r2) {
	static const Rect empty(0.0f, 0.0f, 0.0f, 0.0f);
	if (!r1.intersectsWith(r2)) {
		return empty;
	}

	return fromMaxCoordinates(std::max(r1.getMinX(), r2.getMinX()), std::max(r1.getMinZ(), r2.getMinZ()), std::min(r1.getMaxX(), r2.getMaxX()),
			std::min(r1.getMaxZ(), r2.getMaxZ()));
}

static inline Rect unionWith(const Rect& r1, const Rect& r2) {
	return fromMaxCoordinates(std::min(r1.getMinX(), r2.getMinX()), std::min(r1.getMinZ(), r2.getMinZ()), std::max(r1.getMaxX(), r2.getMaxX()),
			std::max(r1.getMaxZ(), r2.getMaxZ()));
}

inline float Rect::getMaxZ() const {
	return _z + _height;
}

inline bool Rect::isEmpty() const {
	return _width == 0 || _height == 0;
}

inline float Rect::getMinX() const {
	return _x;
}

inline float Rect::getMaxX() const {
	return _x + _width;
}

inline float Rect::getMinZ() const {
	return _z;
}

inline float Rect::getX() const {
	return _x;
}

inline float Rect::getWidth() const {
	return _width;
}

inline float Rect::getZ() const {
	return _z;
}

inline float Rect::getHeight() const {
	return _height;
}

inline bool operator==(const Rect& other) const {
	return _x == other._x && _z == other._z && _width == other._width && _height == other._height;
}

inline bool Rect::contains(const Rect& other) const {
	return rect == intersection(*this, other);
}

inline bool Rect::intersectsWith(const Rect& other) const {
	return !(_x >= other.getMaxX() || getMaxX() <= other._x || _z >= other.getMaxZ() || getMaxZ() <= other._z);
}

inline void Rect::offset(float dx, float dz) {
	_x += dx;
	_z += dz;
}

template<class T>
class QuadTree {
public:
	typedef typename std::vector<T> Contents;
private:
	class QuadTreeNode;
	typedef typename std::vector<QuadTreeNode> Nodes;
	class QuadTreeNode {
		friend class QuadTree;
	private:
		const Rect _area;
		Contents _contents;
		Nodes _nodes;

		void createSubNodes() {
			const float width = _area.getWidth();
			const float height = _area.getHeight();
			if (height * width <= MINAREA) {
				return;
			}

			const float minX = _area.getMinX();
			const float minZ = _area.getMinZ();
			const float halfWidth = width / 2.0f;
			const float halfHeight = height / 2.0f;

			_nodes.push_back(QuadTreeNode(Rect(minX, minZ, halfWidth, halfHeight)));
			_nodes.push_back(QuadTreeNode(Rect(minX, minZ + halfHeight, halfWidth, halfHeight)));
			_nodes.push_back(QuadTreeNode(Rect(minX + halfWidth, minZ, halfWidth, halfHeight)));
			_nodes.push_back(QuadTreeNode(Rect(minX + halfWidth, minZ + halfHeight, halfWidth, halfHeight)));
		}

		QuadTreeNode(const Rect& bounds) :
				_area(bounds) {
		}

		int count() const {
			int count = 0;
			for (auto node : _nodes) {
				count += node.count();
			}
			count += _contents.size();
			return count;
		}

		inline const Rect& getBounds() const {
			return _area;
		}

		inline const Contents& getContents() const {
			return _contents;
		}

		Contents getSubTreeContents() const {
			Contents results;

			for (auto node : _nodes) {
				const Contents& childNodes = node.getSubTreeContents();
				results.insert(results.begin(), childNodes.begin(), childNodes.end());
			}

			results.insert(results.begin(), _contents.begin(), _contents.end());
			return results;
		}

		bool insert(const T& item) {
			const Rect& area = item.getBounds();
			if (!_area.contains(area)) {
				return false;
			}

			if (_nodes.empty()) {
				createSubNodes();
			}

			for (auto node : _nodes) {
				if (node._area.contains(area)) {
					node.insert(item);
					return true;
				}
			}

			_contents.push_back(item);
			return true;
		}

		// is the node empty
		inline bool isEmpty() const {
			return _area.isEmpty() || _nodes.empty();
		}

		Contents query(const Rect& queryArea) const {
			Contents results;

			for (auto item : _contents) {
				const Rect& area = item.getBounds();
				if (queryArea.intersectsWith(area)) {
					results.push_back(item);
				}
			}

			for (auto node : _nodes) {
				if (node.isEmpty()) {
					continue;
				}

				if (node._area.contains(queryArea)) {
					const Contents& contents = node.query(queryArea);
					results.insert(results.begin(), contents.begin(), contents.end());
					break;
				}

				if (queryArea.contains(node._area)) {
					const Contents& contents = node.getSubTreeContents();
					results.insert(results.begin(), contents.begin(), contents.end());
					continue;
				}

				if (node._area.intersectsWith(queryArea)) {
					const Contents& contents = node.query(queryArea);
					results.insert(results.begin(), contents.begin(), contents.end());
				}
			}
			return results;
		}
	};

	QuadTreeNode _root;
public:
	QuadTree(const Rect& rectangle) :
			_root(rectangle) {
	}

	inline int count() const {
		return _root.count();
	}

	inline bool insert(const T& item) {
		return _root.insert(item);
	}

	inline Contents query(const Rect& area) const {
		return _root.query(area);
	}

	inline Contents getContents() const {
		return _root.getSubTreeContents();
	}
};
