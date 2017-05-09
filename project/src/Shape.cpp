#include "Shape.h"
#include <iostream>

#define EIGEN_DONT_ALIGN_STATICALLY
#include <Eigen/Dense>

#include "GLSL.h"
#include "Program.h"

#define TINYOBJLOADER_IMPLEMENTATION
#include "tiny_obj_loader.h"

#include "types.h"

using namespace std;

Shape::Shape() :
	eleBufID(0),
	posBufID(0),
	norBufID(0),
	texBufID(0),
   vaoID(0)
{
}

Shape::~Shape()
{
}

void Shape::loadMesh(const string &meshName)
{
	// Load geometry
	// Some obj files contain material information.
	// We'll ignore them for this assignment.
	vector<tinyobj::shape_t> shapes;
	vector<tinyobj::material_t> objMaterials;
	string errStr;
	bool rc = tinyobj::LoadObj(shapes, objMaterials, errStr, meshName.c_str());
   //cout << shapes.size() << " shapes " << endl;
	if(!rc) {
		cerr << errStr << endl;
	} else {
		posBuf = shapes[0].mesh.positions;
		norBuf = shapes[0].mesh.normals;
		texBuf = shapes[0].mesh.texcoords;
		eleBuf = shapes[0].mesh.indices;

      for (size_t v = 0; v < posBuf.size(); v++) {
         norBuf.push_back(0);
      }
	}
}

void normalize(float v[3])
{
   float length = sqrt((v[0] * v[0]) + (v[1] * v[1]) + (v[2] * v[2]));
   v[0] = v[0] / length;
   v[1] = v[1] / length;
   v[2] = v[2] / length;

}

void calcNormal(float v[3][3], float out[3])
{
   float v1[3],v2[3];
   static const int x = 0;
   static const int y = 1;
   static const int z = 2;
   // Calculate two vectors from the three points
   v1[x] = v[0][x] - v[1][x];
   v1[y] = v[0][y] - v[1][y];
   v1[z] = v[0][z] - v[1][z];
   v2[x] = v[1][x] - v[2][x];
   v2[y] = v[1][y] - v[2][y];
   v2[z] = v[1][z] - v[2][z];
   // Take the cross product of the two vectors to get
   // the normal vector which will be stored in out
   out[x] = v1[y]*v2[z] - v1[z]*v2[y];
   out[y] = v1[z]*v2[x] - v1[x]*v2[z];
   out[z] = v1[x]*v2[y] - v1[y]*v2[x];
   // Normalize the vector
   //cout << "before " << out[x] << out[y] << out[z] << endl;
   normalize(out);
   //cout << "after " << out[x] << out[y] << out[z] << endl;
}

void Shape::compute_normals(const string &meshName)
{
   //clear out norBuf;

   vector<tinyobj::shape_t> shapes;
   vector<tinyobj::material_t> objMaterials;
   string errStr;
   bool rc = tinyobj::LoadObj(shapes, objMaterials, errStr, meshName.c_str());
   if(!rc) {
      cerr << errStr << endl;
   } else {
      int idx1, idx2, idx3;
      float v[3][3];
      float norm[3];

      for (size_t s = 0; s < shapes.size(); s++) {
         assert((shapes[s].mesh.indices.size() % 3) == 0);
         for (int i = 0; i < shapes[s].mesh.indices.size()/3; i++) {

            idx1 = shapes[s].mesh.indices[3*i+0];
            idx2 = shapes[s].mesh.indices[3*i+1];
            idx3 = shapes[s].mesh.indices[3*i+2];

            v[0][0] = shapes[s].mesh.positions[3*idx1+0];
            v[0][1] = shapes[s].mesh.positions[3*idx1+1];
            v[0][2] = shapes[s].mesh.positions[3*idx1+2];

            v[1][0]= shapes[s].mesh.positions[3*idx2+0];
            v[1][1]= shapes[s].mesh.positions[3*idx2+1];
            v[1][2]= shapes[s].mesh.positions[3*idx2+2];

            v[2][0]= shapes[s].mesh.positions[3*idx3+0];
            v[2][1]= shapes[s].mesh.positions[3*idx3+1];
            v[2][2]= shapes[s].mesh.positions[3*idx3+2];

            calcNormal(v, norm);

            norBuf[3*idx1+0] += norm[0];
            norBuf[3*idx2+0] += norm[0];
            norBuf[3*idx3+0] += norm[0];

            norBuf[3*idx1+1] += norm[1];
            norBuf[3*idx2+1] += norm[1];
            norBuf[3*idx3+1] += norm[1];

            norBuf[3*idx1+2] += norm[2];
            norBuf[3*idx2+2] += norm[2];
            norBuf[3*idx3+2] += norm[2];
         }
      }
   }

}

void Shape::resize() {
  float minX, minY, minZ;
   float maxX, maxY, maxZ;
   float scaleX, scaleY, scaleZ;
   float shiftX, shiftY, shiftZ;
   float epsilon = 0.001;

   minX = minY = minZ = 1.1754E+38F;
   maxX = maxY = maxZ = -1.1754E+38F;

   //Go through all vertices to determine min and max of each dimension
   for (size_t v = 0; v < posBuf.size() / 3; v++) {
		if(posBuf[3*v+0] < minX) minX = posBuf[3*v+0];
		if(posBuf[3*v+0] > maxX) maxX = posBuf[3*v+0];

		if(posBuf[3*v+1] < minY) minY = posBuf[3*v+1];
		if(posBuf[3*v+1] > maxY) maxY = posBuf[3*v+1];

		if(posBuf[3*v+2] < minZ) minZ = posBuf[3*v+2];
		if(posBuf[3*v+2] > maxZ) maxZ = posBuf[3*v+2];
	}

	//From min and max compute necessary scale and shift for each dimension
   float maxExtent, xExtent, yExtent, zExtent;
   xExtent = maxX-minX;
   yExtent = maxY-minY;
   zExtent = maxZ-minZ;
   if (xExtent >= yExtent && xExtent >= zExtent) {
      maxExtent = xExtent;
   }
   if (yExtent >= xExtent && yExtent >= zExtent) {
      maxExtent = yExtent;
   }
   if (zExtent >= xExtent && zExtent >= yExtent) {
      maxExtent = zExtent;
   }
   scaleX = 2.0 /maxExtent;
   shiftX = minX + (xExtent/ 2.0);
   scaleY = 2.0 / maxExtent;
   shiftY = minY + (yExtent / 2.0);
   scaleZ = 2.0/ maxExtent;
   shiftZ = minZ + (zExtent)/2.0;

   //Go through all verticies shift and scale them
	for (size_t v = 0; v < posBuf.size() / 3; v++) {
		posBuf[3*v+0] = (posBuf[3*v+0] - shiftX) * scaleX;
		assert(posBuf[3*v+0] >= -1.0 - epsilon);
		assert(posBuf[3*v+0] <= 1.0 + epsilon);
		posBuf[3*v+1] = (posBuf[3*v+1] - shiftY) * scaleY;
		assert(posBuf[3*v+1] >= -1.0 - epsilon);
		assert(posBuf[3*v+1] <= 1.0 + epsilon);
		posBuf[3*v+2] = (posBuf[3*v+2] - shiftZ) * scaleZ;
		assert(posBuf[3*v+2] >= -1.0 - epsilon);
		assert(posBuf[3*v+2] <= 1.0 + epsilon);
	}
}

void Shape::init()
{
   // Initialize the vertex array object
   glGenVertexArrays(1, &vaoID);
   glBindVertexArray(vaoID);

	// Send the position array to the GPU
	glGenBuffers(1, &posBufID);
	glBindBuffer(GL_ARRAY_BUFFER, posBufID);
	glBufferData(GL_ARRAY_BUFFER, posBuf.size()*sizeof(float), &posBuf[0], GL_STATIC_DRAW);

	// Send the normal array to the GPU 
	if(norBuf.empty()) {
		norBufID = 0;
	} else {
		glGenBuffers(1, &norBufID);
		glBindBuffer(GL_ARRAY_BUFFER, norBufID);
		glBufferData(GL_ARRAY_BUFFER, norBuf.size()*sizeof(float), &norBuf[0], GL_STATIC_DRAW);
	}

	// Send the texture array to the GPU
	if(texBuf.empty()) {
		texBufID = 0;
	} else {
		glGenBuffers(1, &texBufID);
		glBindBuffer(GL_ARRAY_BUFFER, texBufID);
		glBufferData(GL_ARRAY_BUFFER, texBuf.size()*sizeof(float), &texBuf[0], GL_STATIC_DRAW);
	}

	// Send the element array to the GPU
	glGenBuffers(1, &eleBufID);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, eleBufID);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, eleBuf.size()*sizeof(unsigned int), &eleBuf[0], GL_STATIC_DRAW);

	// Unbind the arrays
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);

	assert(glGetError() == GL_NO_ERROR);
}

void Shape::draw(const shared_ptr<Program> prog) const
{
	int h_pos, h_nor, h_tex;
	h_pos = h_nor = h_tex = -1;

   glBindVertexArray(vaoID);
	// Bind position buffer
	h_pos = prog->getAttribute("vertPos");
	GLSL::enableVertexAttribArray(h_pos);
	glBindBuffer(GL_ARRAY_BUFFER, posBufID);
	glVertexAttribPointer(h_pos, 3, GL_FLOAT, GL_FALSE, 0, (const void *)0);

	// Bind normal buffer
	h_nor = prog->getAttribute("vertNor");
	if(h_nor != -1 && norBufID != 0) {
		GLSL::enableVertexAttribArray(h_nor);
		glBindBuffer(GL_ARRAY_BUFFER, norBufID);
		glVertexAttribPointer(h_nor, 3, GL_FLOAT, GL_FALSE, 0, (const void *)0);
	}

	if (texBufID != 0) {
		// Bind texcoords buffer
		h_tex = prog->getAttribute("vertTex");
		if(h_tex != -1 && texBufID != 0) {
			GLSL::enableVertexAttribArray(h_tex);
			glBindBuffer(GL_ARRAY_BUFFER, texBufID);
			glVertexAttribPointer(h_tex, 2, GL_FLOAT, GL_FALSE, 0, (const void *)0);
		}
	}

	// Bind element buffer
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, eleBufID);

	// Draw
	glDrawElements(GL_TRIANGLES, (int)eleBuf.size(), GL_UNSIGNED_INT, (const void *)0);

	// Disable and unbind
	if(h_tex != -1) {
		GLSL::disableVertexAttribArray(h_tex);
	}
	if(h_nor != -1) {
		GLSL::disableVertexAttribArray(h_nor);
	}
	GLSL::disableVertexAttribArray(h_pos);
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
}
