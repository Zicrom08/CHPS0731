//
// Created by Florent on 22/10/2018.
//

#include "bounding_box.h"

BoundingBox::BoundingBox(float xMin, float yMin, float zMin, float xMax, float yMax, float zMax) : xMin(xMin), yMin(yMin), zMin(zMin), xMax(xMax),
																								   yMax(yMax), zMax(zMax) {
	box[0] = Plan(nullptr, glm::vec3(0, 0, this->zMin), glm::vec3(0, 0, -1));
	box[1] = Plan(nullptr, glm::vec3(0, 0, this->zMax), glm::vec3(0, 0, 1));
	box[2] = Plan(nullptr, glm::vec3(this->xMin, 0, 0), glm::vec3(-1, 0, 0));
	box[3] = Plan(nullptr, glm::vec3(this->xMax, 0, 0), glm::vec3(1, 0, 0));
	box[4] = Plan(nullptr, glm::vec3(0, this->yMin, 0), glm::vec3(0, -1, 0));
	box[5] = Plan(nullptr, glm::vec3(0, this->yMax, 0), glm::vec3(0, 1, 0));
}

bool BoundingBox::calculIntersection(const Rayon& rayon, const Scene& scene, std::vector<Intersection>& I, int complexite) {
	for (auto&& plan : box) {
		if (plan.calculIntersection(rayon, scene, I, complexite)) {
			auto& intersect = I.back();
			glm::vec3 point = rayon.Orig() + rayon.Vect() * intersect.getDist();
			I.pop_back();
			bool condX = (point.x <= this->xMax) && (point.x >= this->xMin),
					condY = (point.y <= this->yMax) && (point.y >= this->yMin),
					condZ = (point.z <= this->zMax) && (point.z >= this->zMin);
			if (condX && condY && condZ) //If point is inside/on the box, then intersection is true.
				return true;
		}
	}
	return false;
}
