#include <vector>
#include <iostream>
#include <fstream>
#include <sstream>
#include <vector>
#include <stdexcept>
#include "OBJLoader.h"
#include "GeometricModel.h"

bool OBJLoader::loadModel(const std::string& filename, GeometricModel& model) {
	using Face = GeometricModel::Face;
	if (filename.find_last_of(".obj") == std::string::npos)
		throw std::logic_error(std::string("ERROR : OBJ Geometric Model Loader : ") + filename + std::string(" is not an .obj file.\n"));
	std::ifstream fp_in;
	fp_in.open(filename.c_str(), std::ios::in);
	if (!fp_in)  // file couldn't be opened
		throw std::logic_error(
				std::string("ERROR : OBJ Geometric Model Loader : ") + filename + std::string(" could not be opened or does not exist.\n"));


	std::string c, d, e;
	char linechar[200];
	glm::vec3 v;
	Face F;
	bool texcoords = false;

	while (!fp_in.eof()) {
		fp_in.getline(linechar, 200, '\n');
		std::string line(linechar);

		if (line.find("vt") == 0) // Texture coordinates
		{

			float x, y;
			std::istringstream iline(line.substr(3));
			iline >> x >> y; // u,v only for now
			glm::vec3 vLocal(x, y, 0.0);
			model.listCoords.push_back(vLocal);

		} else if (line.find("vn") == 0) // Texture coordinates
		{
			//ignore for now
		} else if (line.find('v') == 0) // Vertex
		{

			std::istringstream iline(line.substr(2));
			iline >> v[0] >> v[1] >> v[2];
			model.listVertex.push_back(v);
		} else if (line.find('f') == 0)  // Faces
		{

			std::vector<int> Vlist_per_face;
			std::vector<int> VTlist_per_face;
			int pos;
			int old_pos = 2;
			do {
				pos = static_cast<int>(line.find_first_of(' ', static_cast<unsigned long>(old_pos + 1)));

				std::string segment = line.substr(old_pos, pos - 1);
				std::istringstream isegment(segment);
				int S;
				isegment >> S;
				Vlist_per_face.push_back(S); // Format of a segment can be V, V/VT, V/VT/VN. Only interested in V right now
				int VT;
				int pp = static_cast<int>(segment.find_first_of('/'));

				if (pp != -1) {
					std::string segmentVt = segment.substr(static_cast<std::size_t>(pp + 1), std::string::npos);
					if (segmentVt.find_first_of('/') != 0) {
						std::istringstream isegmentVt(segmentVt);
						isegmentVt >> VT;
						VTlist_per_face.push_back(VT);
					} else
						VTlist_per_face.push_back(0); // Store 0 if no tex coord
				} else
					VTlist_per_face.push_back(0);// Store 0 if no tex coord

				old_pos = pos + 1;
			} while (pos != -1);

			if (Vlist_per_face.size() != 3)
				throw std::logic_error(std::string("ERROR : Geometric Model : ") + filename + std::string(" is not a triangle-based mesh.\n"));

			// Vertex numerotation begins at 1 in .obj definition
			F.s1 = Vlist_per_face[0] - 1;
			F.s2 = Vlist_per_face[1] - 1;
			F.s3 = Vlist_per_face[2] - 1;
			model.listFaces.push_back(F);


			F.s1 = VTlist_per_face[0] - 1;
			F.s2 = VTlist_per_face[1] - 1;
			F.s3 = VTlist_per_face[2] - 1;
			model.listCoordFaces.push_back(F);


		}
	}

	model.nb_vertex = static_cast<unsigned int>(model.listVertex.size());
	model.nb_faces = static_cast<unsigned int>(model.listFaces.size());
	fp_in.close();


	computeNormals(model);
	if (model.listCoords.empty())
		setupForTextureCoordinates(model);

	if (!model.listCoords.empty())
		computeTangents(model);

	return (true);
}

void OBJLoader::setupForTextureCoordinates(GeometricModel& model) {
	using Face = GeometricModel::Face;
	std::vector<glm::vec3> packcoords;
	for (std::size_t i = 0; i < model.nb_vertex; i++)
		packcoords.emplace_back(-1.0, -1.0, -1.0);

	for (std::size_t i = 0; i < model.nb_faces; i++) {
		Face c_face = model.listFaces[i];
		Face vt_face = model.listCoordFaces[i];

		glm::vec3 ctex = model.listCoords[vt_face.s1];
		if (packcoords[c_face.s1][0] == -1.0)    // pack vide :OK
			packcoords[c_face.s1] = ctex;
		else if (packcoords[c_face.s1] != ctex)    // deja un vertex assign� : On dedouble le sommet
		{
			packcoords.push_back(ctex);
			model.listFaces[i].s1 = static_cast<int>(model.listVertex.size());
			model.listCoordFaces[i].s1 = static_cast<int>(model.listVertex.size());
			model.listVertex.push_back(model.listVertex[c_face.s1]);
			model.listNormals.push_back(model.listNormals[c_face.s1]);
		}

		ctex = model.listCoords[vt_face.s2];
		if (packcoords[c_face.s2][0] == -1.0)    // pack vide :OK
			packcoords[c_face.s2] = ctex;
		else if (packcoords[c_face.s2] != ctex)    // deja un vertex assign� : On dedouble le sommet
		{
			packcoords.push_back(ctex);
			model.listFaces[i].s2 = static_cast<int>(model.listVertex.size());
			model.listCoordFaces[i].s2 = static_cast<int>(model.listVertex.size());
			model.listVertex.push_back(model.listVertex[c_face.s2]);
			model.listNormals.push_back(model.listNormals[c_face.s2]);
		}

		ctex = model.listCoords[vt_face.s3];
		if (packcoords[c_face.s3][0] == -1.0)    // pack vide :OK
			packcoords[c_face.s3] = ctex;
		else if (packcoords[c_face.s3] != ctex)    // deja un vertex assign� : On dedouble le sommet
		{
			packcoords.push_back(ctex);
			model.listFaces[i].s3 = static_cast<int>(model.listVertex.size());
			model.listCoordFaces[i].s3 = static_cast<int>(model.listVertex.size());
			model.listVertex.push_back(model.listVertex[c_face.s3]);
			model.listNormals.push_back(model.listNormals[c_face.s3]);
		}
	}
	model.nb_vertex = static_cast<unsigned int>(model.listVertex.size());
	model.listCoords = packcoords;
}

void OBJLoader::computeNormals(GeometricModel& model) {

	for (unsigned int i = 0; i < model.listVertex.size(); i++)
		model.listNormals.emplace_back(0.0, 0.0, 0.0);

	for (unsigned int i = 0; i < model.listFaces.size(); i++) {
		glm::vec3 v1 = glm::normalize(model.listVertex[model.listFaces[i].s2] - model.listVertex[model.listFaces[i].s1]);
		glm::vec3 v2 = glm::normalize(model.listVertex[model.listFaces[i].s3] - model.listVertex[model.listFaces[i].s1]);
		glm::vec3 v = glm::cross(v1, v2);
		model.listNormals[model.listFaces[i].s1] += v;
		model.listNormals[model.listFaces[i].s2] += v;
		model.listNormals[model.listFaces[i].s3] += v;
	}
	for (auto&& listNormal : model.listNormals)
		listNormal = glm::normalize(listNormal);

}

void OBJLoader::computeTangents(GeometricModel& model) {
	using Face = GeometricModel::Face;
	///////////////// Compute Tangent space - Nvidia Code
	model.listTangents.resize(model.listVertex.size() + 1);
	auto tan1 = new glm::vec3[model.listVertex.size()];
	auto tan2 = new glm::vec3[model.listVertex.size()];

	for (std::size_t i = 0; i < model.listFaces.size(); i++) {
		Face c_face = model.listFaces[i];
		glm::vec3 v1 = model.listVertex[c_face.s1];
		glm::vec3 v2 = model.listVertex[c_face.s2];
		glm::vec3 v3 = model.listVertex[c_face.s3];

		glm::vec3 w1 = model.listCoords[c_face.s1];
		glm::vec3 w2 = model.listCoords[c_face.s2];
		glm::vec3 w3 = model.listCoords[c_face.s3];
		float x1 = v2.x - v1.x;
		float x2 = v3.x - v1.x;
		float y1 = v2.y - v1.y;
		float y2 = v3.y - v1.y;
		float z1 = v2.z - v1.z;
		float z2 = v3.z - v1.z;

		float s1 = w2.x - w1.x;
		float s2 = w3.x - w1.x;
		float t1 = w2.y - w1.y;
		float t2 = w3.y - w1.y;
		float r = 1.0F / (s1 * t2 - s2 * t1);
		glm::vec3 sdir((t2 * x1 - t1 * x2) * r, (t2 * y1 - t1 * y2) * r,
					   (t2 * z1 - t1 * z2) * r);
		glm::vec3 tdir((s1 * x2 - s2 * x1) * r, (s1 * y2 - s2 * y1) * r,
					   (s1 * z2 - s2 * z1) * r);
		tan1[c_face.s1] += sdir;
		tan1[c_face.s2] += sdir;
		tan1[c_face.s3] += sdir;

		tan2[c_face.s1] += tdir;
		tan2[c_face.s2] += tdir;
		tan2[c_face.s3] += tdir;
	}
	for (std::size_t a = 0; a < model.listVertex.size(); a++) {
		glm::vec3 n = model.listNormals[a];
		glm::vec3 t = tan1[a];

		// Gram-Schmidt orthogonalize calculate T'
		float dotnt = glm::dot(n, t);
		model.listTangents[a] = glm::vec4(glm::normalize((t - (dotnt * n))), (glm::dot(glm::cross(n, t), tan2[a]) < 0.0f) ? -1.0f : 1.0f);

	}

	delete[] tan1, delete[] tan2;
}
