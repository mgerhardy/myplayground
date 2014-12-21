#include "QuadTree.h"
#include <iostream>

class Entity {
private:
	int _id;
	Rect _area;
public:
	Entity(int id, const Rect& area) :
			_id(id), _area(area) {
	}

	const int getId() const {
		return _id;
	}

	const Rect& getBounds() const {
		return _area;
	}
};

typedef QuadTree<Entity> EntityTree;

static void render(const Entity& entity) {
	std::cout << entity.getId() << std::endl;
}

int main(const int argc, char *argv[]) {
	const Rect world(0, 0, 100, 100);
	EntityTree tree(world);
	for (int i = 0; i < 10000; ++i) {
		const int x = (i * 10) / 1000;
		const int y = (i / 1000) * 10;
		const Entity e(i, Rect(x, y, 10, 10));
		if (tree.insert(e)) {
			std::cout << "Add " << x << ":" << y << std::endl;
		}
	}
	const Rect screen(world.getWidth() / 2.0f, world.getHeight() / 2.0f, 10, 10);
	const EntityTree::Contents& query = tree.query(screen);
	std::cout << "Queried " << query.size() << " entries at " << screen.getX() << ":" << screen.getZ() << " with area " << screen.getWidth() << ":"
			<< screen.getHeight() << std::endl;
	for (auto entity : query) {
		render(entity);
	}
	const EntityTree::Contents& contents = tree.getContents();
	for (auto entity : contents) {
	//	render(entity);
	}
}
