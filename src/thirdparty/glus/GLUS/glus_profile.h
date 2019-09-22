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

#ifndef GLUS_PROFILE_H_
#define GLUS_PROFILE_H_

/**
 * Reset FPS profiling.
 */
GLUSAPI GLUSvoid GLUSAPIENTRY glusProfileResetFPSf();

/**
 * Update FPS profiling and log FPS.
 *
 * @param time Passed time per frame.
 * @param frames Stores passed frames. If this is a null pointer, the output is logged.
 *
 * @return GLUS_TRUE, if one second passed.
 */
GLUSAPI GLUSboolean GLUSAPIENTRY glusProfileUpdateFPSf(GLUSfloat time, GLUSuint* frames);

#endif /* GLUS_PROFILE_H_ */
