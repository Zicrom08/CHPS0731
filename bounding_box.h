//
// Created by Florent on 22/10/2018.
//

#ifndef PROJECT_BOUNDING_BOX_H
#define PROJECT_BOUNDING_BOX_H


#include <array>
#include "objet.h"
#include "plan.hpp"

/**
 * @class BoundingBox
 * Used to bound a mesh only.
 * @see Mesh
 */
class BoundingBox : public Objet {
private:
	float xMin;
	float yMin;
	float zMin;
	float xMax;
	float yMax;
	float zMax;

	std::array<Plan, 6> box;

public:
	BoundingBox() = default;

	explicit BoundingBox(float xMin, float yMin, float zMin, float xMax, float yMax, float zMax);

	~BoundingBox() override = default;

	/**
	 * Computes intersection with the box to check if we compute the mesh inside.
	 * @param rayon
	 * @param scene
	 * @param I
	 * @param complexite
	 * @return
	 */
	bool calculIntersection(const Rayon& rayon, const Scene& scene, std::vector<Intersection>& I, int complexite) override;


};


#endif //PROJECT_BOUNDING_BOX_H
