
// constants
const int PLANE_TYPE = 0;
const int SPHERE_TYPE = 1;
const int BOX_TYPE = 2;

// unions

float opUnion(float d1, float d2) {
	return min(d1, d2);
}

float opSmoothUnion( float d1, float d2, float k ) {
    float h = clamp( 0.5 + 0.5*(d2-d1)/k, 0.0, 1.0 );
    return mix( d2, d1, h ) - k*h*(1.0-h);
}

// intersections

float opIntersection( float d1, float d2 ) {
	return max(d1,d2);
}

// plane

float sdPlane( vec3 p, vec4 n ) {
	return dot(p, n.xyz) + n.w;
}

// sphere

float sdSphere(vec3 p, float s) {
	return length(p) - s;
}

float opSphere(vec3 p, mat4 transform, float s) {
	return sdSphere((inverse(transform) * vec4(p, 1.0f)).xyz, s);
}

// box

float sdBox(vec3 p, vec3 b) {
	vec3 q = abs(p) - b;
	return length(max(q, 0.0f)) + min(max(q.x, max(q.y, q.z)), 0.0);
}

float map(in vec3 p, in vec3 translate, in vec4 shapeData, in int type) {
	float d = 100000.0f;
	
	if (type == PLANE_TYPE)
	{
		d = sdPlane(p, shapeData);
	}
	else if (type == SPHERE_TYPE)
	{
		d = sdSphere(p - translate, shapeData.x);
	}
	else if (type == BOX_TYPE)
	{
		d = sdBox(p - translate, shapeData.xyz);
	}

	return d;
}
