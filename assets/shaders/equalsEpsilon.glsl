bool equalsEpsilon(vec4 left, vec4 right, float epsilon) {
	return all(lessThanEqual(abs(left - right), vec4(epsilon)));
}

bool equalsEpsilon(vec3 left, vec3 right, float epsilon) {
	return all(lessThanEqual(abs(left - right), vec3(epsilon)));
}

bool equalsEpsilon(vec2 left, vec2 right, float epsilon) {
	return all(lessThanEqual(abs(left - right), vec2(epsilon)));
}

bool equalsEpsilon(float left, float right, float epsilon) {
    return (abs(left - right) <= epsilon);
}
