##stage vertex /////////////////////////////////////////////////////////

##output {
	vec2 uv;
}

vec4 vertices[3] = {
	vec4(3.0, 1.0, 0.0, 1.0),
	vec4(-1.0, 1.0, 0.0, 1.0),
	vec4(-1.0, -3.0, 0.0, 1.0)
};

vec2 texcoords[3] = {
	vec2(2.0, 1.0),
	vec2(0.0, 1.0),
	vec2(0.0, -1.0)
};

void main()
{
	gl_Position = vertices[gl_VertexID];
	o.uv = texcoords[gl_VertexID];
}

##stage fragment ///////////////////////////////////////////////////////

void main()
{
	a_color = texture(s_texture, a.uv);
}
