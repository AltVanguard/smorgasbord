##stage vertex

##output {
	vec2 uv;
}

void main()
{
	o.uv = v_uv + sin(c_t) * 0.03;
	gl_Position = c_mvp * vec4(v_p, 1);
}

##stage fragment

void main()
{
	a_color = texture(s_texture, a.uv);
}
