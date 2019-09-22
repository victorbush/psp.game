/*
 * GLUS - Modern OpenGL, OpenGL ES and OpenVG Utilities. Copyright (C) since 2010 Norbert Nopper
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU Lesser General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

#ifndef GLUS_WAVEFRONT_H_
#define GLUS_WAVEFRONT_H_

/**
 * Structure for holding material data.
 */
typedef struct _GLUSmaterial
{
	/**
	 * Name of the material.
	 */
	GLUSchar name[GLUS_MAX_STRING];

	/**
	 * Emissive color.
	 */
	GLUSfloat emissive[4];

	/**
	 * Ambient color.
	 */
	GLUSfloat ambient[4];

	/**
	 * Diffuse color.
	 */
	GLUSfloat diffuse[4];

	/**
	 * Specular color.
	 */
	GLUSfloat specular[4];

	/**
	 * Shininess.
	 */
	GLUSfloat shininess;

	/**
	 * Transparency, which is the alpha value.
	 */
	GLUSfloat transparency;

	/**
	 * Reflection.
	 */
	GLUSboolean reflection;

	/**
	 * Refraction.
	 */
	GLUSboolean refraction;

	/**
	 * Index of refraction.
	 */
	GLUSfloat indexOfRefraction;

	/**
	 * Emissive color texture filename.
	 */
	GLUSchar emissiveTextureFilename[GLUS_MAX_STRING];

	/**
	 * Ambient color texture filename.
	 */
	GLUSchar ambientTextureFilename[GLUS_MAX_STRING];

	/**
	 * Diffuse color texture filename.
	 */
	GLUSchar diffuseTextureFilename[GLUS_MAX_STRING];

	/**
	 * Specular color texture filename.
	 */
	GLUSchar specularTextureFilename[GLUS_MAX_STRING];

	/**
	 * Transparency texture filename.
	 */
	GLUSchar transparencyTextureFilename[GLUS_MAX_STRING];

	/**
	 * Bump texture filename.
	 */
	GLUSchar bumpTextureFilename[GLUS_MAX_STRING];

	/**
	 * Can be used to store the emissive texture name.
	 */
    GLUSuint emissiveTextureName;

	/**
	 * Can be used to store the ambient texture name.
	 */
    GLUSuint ambientTextureName;

	/**
	 * Can be used to store the diffuse texture name.
	 */
    GLUSuint diffuseTextureName;

	/**
	 * Can be used to store the specular texture name.
	 */
    GLUSuint specularTextureName;

	/**
	 * Can be used to store the transparency texture name.
	 */
    GLUSuint transparencyTextureName;

	/**
	 * Can be used to store the bump texture name.
	 */
    GLUSuint bumpTextureName;

} GLUSmaterial;

/**
 * Structure for holding material data list.
 */
typedef struct _GLUSmaterialList
{
	/**
	 * The material data.
	 */
	GLUSmaterial material;

	/**
	 * The pointer to the next element.
	 */
	struct _GLUSmaterialList* next;

} GLUSmaterialList;

/**
 * Group of geometry.
 */
typedef struct _GLUSgroup
{
		/**
		 * Name of the group.
		 */
		GLUSchar name[GLUS_MAX_STRING];

		/**
		 * Name of the material.
		 */
		GLUSchar materialName[GLUS_MAX_STRING];

		/**
		 * Pointer to the material.
		 */
		GLUSmaterial* material;

	    /**
	     * Indices.
	     */
	    GLUSindex* indices;

	    /**
	     * Indices VBO.
	     */
	    GLUSuint indicesVBO;

	    /**
	     * VAO of this group.
	     */
	    GLUSuint vao;

	    /**
	     * Number of indices.
	     */
	    GLUSuint numberIndices;

	    /**
	     * Triangle render mode - could be either:
	     *
	     * GLUS_TRIANGLES
	     * GLUS_TRIANGLE_STRIP
	     */
	    GLUSenum mode;

} GLUSgroup;

/**
 * Structure for holding the group data list.
 */
typedef struct _GLUSgroupList
{
		/**
		 * The group data.
		 */
		GLUSgroup group;

	    /**
	     * The pointer to the next group element.
	     */
	    struct _GLUSgroupList* next;

} GLUSgroupList;

/**
 * Structure for a complete wavefront object file.
 */
typedef struct _GLUSwavefront
{
		/**
		 * Name of the object.
		 */
		GLUSchar name[GLUS_MAX_STRING];

		/**
		 * Vertices in homogeneous coordinates.
		 */
	    GLUSfloat* vertices;

	    /**
	     * Vertices VBO.
	     */
	    GLUSuint verticesVBO;

	    /**
	     * Normals.
	     */
	    GLUSfloat* normals;

	    /**
	     * Normals VBO.
	     */
	    GLUSuint normalsVBO;

	    /**
	     * Tangents.
	     */
	    GLUSfloat* tangents;

	    /**
	     * Tangents VBO.
	     */
	    GLUSuint tangentsVBO;

	    /**
	     * Bitangents.
	     */
	    GLUSfloat* bitangents;

	    /**
	     * Bitangents VBO.
	     */
	    GLUSuint bitangentsVBO;

	    /**
	     * Texture coordinates.
	     */
	    GLUSfloat* texCoords;

	    /**
	     * Texture corrdinates VBO.
	     */
	    GLUSuint texCoordsVBO;

	    /**
	     * Number of vertices.
	     */
	    GLUSuint numberVertices;

	    /**
	     * Pointer to the first element of the groups.
	     */
	    GLUSgroupList* groups;

	    /**
	     * Pointer to the first element of the materials.
	     */
	    GLUSmaterialList* materials;

} GLUSwavefront;

/**
 * Structure for holding the object data list.
 */
typedef struct _GLUSobjectList
{
		/**
		 * The object data.
		 */
		GLUSwavefront object;

	    /**
	     * The pointer to the next group element.
	     */
	    struct _GLUSobjectList* next;

} GLUSobjectList;

/**
 * Structure for holding the scene.
 */
typedef struct _GLUSscene
{
		/**
		 * The root object list data.
		 */
		GLUSobjectList* objectList;

} GLUSscene;

/**
 * Loads a wavefront object file with groups and materials.
 *
 * @param filename The name of the wavefront file including extension.
 * @param wavefront The data is stored into this structure.
 *
 * @return GLUS_TRUE, if loading succeeded.
 */
GLUSAPI GLUSboolean GLUSAPIENTRY glusWavefrontLoad(const GLUSchar* filename, GLUSwavefront* wavefront);

/**
 * Destroys the wavefront structure by freeing the allocated memory. VBOs, VAOs and textures are not freed.
 *
 * @param wavefront The structure which contains the dynamic allocated wavefront data, which will be freed by this function.
 */
GLUSAPI GLUSvoid GLUSAPIENTRY glusWavefrontDestroy(GLUSwavefront* wavefront);

/**
 * Loads a wavefront scene file with objects, groups and materials.
 *
 * @param filename The name of the wavefront file including extension.
 * @param scene The data is stored into this structure.
 *
 * @return GLUS_TRUE, if loading succeeded.
 */
GLUSAPI GLUSboolean GLUSAPIENTRY glusWavefrontLoadScene(const GLUSchar* filename, GLUSscene* scene);

/**
 * Destroys the wavefront structure by freeing the allocated memory. VBOs, VAOs and textures are not freed.
 *
 * @param scene The structure which contains the dynamic allocated wavefront data, which will be freed by this function.
 */
GLUSAPI GLUSvoid GLUSAPIENTRY glusWavefrontDestroyScene(GLUSscene* scene);

#endif /* GLUS_WAVEFRONT_H_ */
