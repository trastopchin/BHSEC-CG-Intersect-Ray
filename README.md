BHSEC-CG-Intersect-Ray

C++ project files relevant to the implementation of a basic ray tracer. These project files correspond to the final project of a computer graphics independent study at Bard High School Early College Queens the fall of 2016 with professor Matthew Carlberg.

<p align="center">
  <img src="/intersect_ray_examples/mirror_test2_big.png" alt="A scene with a plane and four differently colored diffuse and ideally reflective spheres." width="800">
</p>

This project was built upon Brown CS123's Introduction to Computer Graphics starter code. The majority of the relevant code takes place within the geometry class files (Mesh, ConeMesh, CylinderMesh etc.) and the RayScene class files. The geometry class files define tesselation and ray-surface intersection functionality whereas the RayScene class contains the actual implementation of the ray tracing algorithm.

The []() folder contains examples scenes rendered with the ray tracer.

## Implementation Details

This ray tracer renders implicitly defined surfaces such as cones, cubes, cylinders, and spheres with phong illumination, shadows, and mirror reflection. The ray tracer handles shadows and mirror reflection by using secondary rays to probe the scene for light occluders and compute the incoming illumination of an ideal reflection.

## Built With

* [OpenGL Mathematics](https://glm.g-truc.net/0.9.9/index.html) - A C++ mathematics library for graphics software based on the OpenGL Shading Language (GLSL) specifications.