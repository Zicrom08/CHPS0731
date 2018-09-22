//
// Created by postaron on 22/09/18.
//

#ifndef RAYTRACE_BASE_PLAN_HPP
#define RAYTRACE_BASE_PLAN_HPP


#include "objet.h"

/**
 * \class Plan
 * @brief Un plan affichable. Définit par son vecteur normal + distance de la caméra.
 */
class Plan : public Objet {
private:
	float distOrig;
	glm::vec3 normal;

public:
	Plan() = default;

	explicit Plan(float r, float g, float b, float distOrig, float x, float y, float z);

	explicit Plan(Material* material, float distOrig, float x, float y, float z);

	explicit Plan(float distOrig, float x, float y, float z);

	bool calculIntersection(const Rayon& rayon, const Scene& scene, std::vector<Intersection>&, int) override;
};


#endif //RAYTRACE_BASE_PLAN_HPP
