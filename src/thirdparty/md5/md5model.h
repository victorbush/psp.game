/*
 * md5model.h -- md5mesh model loader + animation
 * last modification: aug. 14, 2007
 *
 * Copyright (c) 2005-2007 David HENRY
 *
 * Permission is hereby granted, free of charge, to any person
 * obtaining a copy of this software and associated documentation
 * files (the "Software"), to deal in the Software without
 * restriction, including without limitation the rights to use,
 * copy, modify, merge, publish, distribute, sublicense, and/or
 * sell copies of the Software, and to permit persons to whom the
 * Software is furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be
 * included in all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
 * EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF
 * MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND
 * NONINFRINGEMENT.
 * IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR
 * ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF
 * CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION
 * WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
 *
 * gcc -Wall -ansi -lGL -lGLU -lglut md5anim.c md5anim.c -o md5model
 */

#ifndef __MD5MODEL_H__
#define __MD5MODEL_H__

/* Vectors */
typedef float v2_t[2];
typedef float v3_t[3];

/* Quaternion (x, y, z, w) */
typedef float quat4_t[4];

enum {
  X, Y, Z, W
};

/* Joint */
typedef struct
{
  char name[64];
  int parent;

  v3_t pos;
  quat4_t orient;
} md5_joint_t;

/* Vertex */
typedef struct
{
  v2_t st;

  int start; /* start weight */
  int count; /* weight count */
} md5_vertex_t;

/* Triangle */
typedef struct
{
  int index[3];
} md5_triangle_t;

/* Weight */
typedef struct
{
  int joint;
  float bias;

  v3_t pos;
} md5_weight_t;

/* Bounding box */
typedef struct
{
  v3_t min;
  v3_t max;
} md5_bbox_t;

/* MD5 mesh */
typedef struct
{
  md5_vertex_t *vertices;
  md5_triangle_t *triangles;
  md5_weight_t *weights;

  int num_verts;
  int num_tris;
  int num_weights;

  char shader[256];
} md5_mesh_t;

/* MD5 model structure */
typedef struct
{
  md5_joint_t *baseSkel;
  md5_mesh_t *meshes;

  int num_joints;
  int num_meshes;
} md5_model_t;

/* Animation data */
typedef struct
{
  int num_frames;
  int num_joints;
  int frameRate;

  md5_joint_t **skelFrames;
  md5_bbox_t *bboxes;
} md5_anim_t;

/* Animation info */
typedef struct
{
  int curr_frame;
  int next_frame;

  double last_time;
  double max_time;
} anim_info_t;

/**
 * Quaternion prototypes
 */
void Quat_computeW (quat4_t q);
void Quat_normalize (quat4_t q);
void Quat_multQuat (const quat4_t qa, const quat4_t qb, quat4_t out);
void Quat_multVec (const quat4_t q, const v3_t v, quat4_t out);
void Quat_rotatePoint (const quat4_t q, const v3_t in, v3_t out);
float Quat_dotProduct (const quat4_t qa, const quat4_t qb);
void Quat_slerp (const quat4_t qa, const quat4_t qb, float t, quat4_t out);

/**
 * md5mesh prototypes
 */
int ReadMD5Model (const char *filename, md5_model_t *mdl);
void FreeModel (md5_model_t *mdl);
void PrepareMesh (const md5_mesh_t *mesh, const md5_joint_t *skeleton);
void AllocVertexArrays ();
void FreeVertexArrays ();
void DrawSkeleton (const md5_joint_t *skeleton, int num_joints);

/**
 * md5anim prototypes
 */
int CheckAnimValidity (const md5_model_t *mdl, const md5_anim_t *anim);
int ReadMD5Anim (const char *filename, md5_anim_t *anim);
void FreeAnim (md5_anim_t *anim);
void InterpolateSkeletons (const md5_joint_t *skelA,
			   const md5_joint_t *skelB,
			   int num_joints, float interp,
			   md5_joint_t *out);
void Animate (const md5_anim_t *anim,
	      anim_info_t *animInfo, double dt);

#endif /* __MD5MODEL_H__ */
