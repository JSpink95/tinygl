
struct Ray {
	vec3 origin, direction;
};

Ray createRay(vec3 o, vec3 d) {
	Ray ray;
	ray.origin = o;
	ray.direction = d;

	return ray;
}

Ray createRayFromCameraUV(vec2 uv, in mat4 cameraToWorld, in mat4 invCameraProjection) {
    // Transform the camera origin to world space
    vec3 origin = (cameraToWorld * vec4(0.0f, 0.0f, 0.0f, 1.0f)).xyz;
    
    // Invert the perspective projection of the view-space position
    vec3 direction = (invCameraProjection * vec4(uv, 1.0f, 1.0f)).xyz;

    // Transform the direction from camera to world space and normalize
    direction = (cameraToWorld * vec4(direction, 0.0f)).xyz;
    direction = normalize(direction);

    return createRay(origin, direction);
}
